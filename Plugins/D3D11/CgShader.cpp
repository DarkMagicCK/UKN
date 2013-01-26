#include "CgShader.h"

#include "D3D11GraphicDevice.h"
#include "D3D11Debug.h"
#include "D3D11Texture.h"
#include "D3D11Helper.h"

#include "Cg/cgD3D11.h"

#include "mist/Logger.h"

#include <D3DX10math.h>
#include <d3d.h>
#include <D3Dcompiler.h>

namespace ukn {

    bool _check_error(CGcontext context) {
        CGerror error;
        const char* str = cgGetLastErrorString(&error);
        if(error != CG_NO_ERROR) {
            if(error == CG_COMPILER_ERROR) {
                log_error(std::string(str) + cgGetLastListing(context));
            } else 
                log_error(str);
            return false;
        }
        return true;
    }

    CgDxEffect::CgDxEffect(D3D11GraphicDevice* device):
    mContext(0),
    mDevice(device),
    mLayout(0) {
            mContext = cgCreateContext();
            mist_assert(mContext);

            cgD3D11SetDevice(mContext, device->getD3DDevice());
    }

    CgDxEffect::~CgDxEffect() {
        if(mContext) {
            cgDestroyContext(mContext);
        }
    }

    void CgDxEffect::setVertexFormat(const VertexFormat& format) {
        if(mVertexShader) {
            CgDxShader* vertexShader = checked_cast<CgDxShader*>(mVertexShader.get());
            if(vertexShader) {
                ID3DBlob* vsBlob = cgD3D11GetCompiledProgram(vertexShader->getProgram());
                mLayout = D3D11ShaderUtilities::CreateLayout(mDevice->getD3DDevice(),
                    vsBlob->GetBufferPointer(),
                    vsBlob->GetBufferSize(),
                    format);

            }
        }
    }

    void CgDxEffect::bind(uint32 pass) {
        if(mLayout) {
            mDevice->getD3DDeviceContext()->IASetInputLayout(mLayout);
            if(mVertexShader)
                mVertexShader->bind();
            if(mFragmentShader) {
                mFragmentShader->bind();
            }
            if(mGeometryShader)
                mGeometryShader->bind();
        }
    }

    void CgDxEffect::unbind() {
        if(mVertexShader)
            mVertexShader->unbind();
        if(mFragmentShader)
            mFragmentShader->unbind();
        if(mGeometryShader)
            mGeometryShader->unbind();
    }

    ShaderPtr CgDxEffect::createShader(const ResourcePtr& resource, const ShaderDesc& desc) {
        CgDxShader* shader = new CgDxShader(mContext);
        if(shader->initialize(resource, desc)) {
            return ShaderPtr(shader);
        }
        return ShaderPtr();
    }
    
    uint32 CgDxEffect::getPasses() const {
        return 1;
    }

    CGcontext CgDxEffect::getContext() const {
        return mContext;
    }

    CgDxShader::CgDxShader(CGcontext context):
        mContext(context),
        mProgram(0) {

    }

    CgDxShader::~CgDxShader() {
        if(mProgram) {
            cgD3D11UnloadProgram(mProgram);
            cgDestroyProgram(mProgram);
        }
    }

    bool CgDxShader::initialize(const ResourcePtr& resource, const ShaderDesc& desc) {
        CGprofile profile;
        mDesc = desc;
        switch (desc.type)
        {
        case ukn::ST_FragmentShader:
            profile = cgD3D11GetLatestPixelProfile();;
            break;
        case ukn::ST_VertexShader:
            profile = cgD3D11GetLatestVertexProfile();
            break;
        case ukn::ST_GeometryShader:
            profile = CG_PROFILE_GS_4_0;
            break;
        }
        StreamPtr stream = resource->readIntoMemory();
        MemoryStream* memStream = ((MemoryStream*)stream.get());
        std::string content(memStream->data(), memStream->data() + memStream->size());
        mProgram = cgCreateProgram(mContext, 
            CG_SOURCE, 
            content.c_str(), 
            profile, 
            desc.entry.c_str(), 
            cgD3D11GetOptimalOptions(profile));
        if(_check_error(mContext) &&
            D3D11Debug::CHECK_RESULT( cgD3D11LoadProgram(mProgram, 0))) {
                return true;
        }
        return true;
    }

    void CgDxShader::bind() {
        if(mProgram)
            D3D11Debug::CHECK_RESULT(cgD3D11BindProgram(mProgram));
    }

    void CgDxShader::unbind() {
        if(mProgram)
            cgD3D11UnbindProgram(mProgram);
    }

    bool CgDxShader::setMatrixVariable(const char* name, const Matrix4& mat) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgSetParameterValuefc(param, 16, (const float*)mat.x); 
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgDxShader::setFloatVectorVariable(const char* name, const float4& vec) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgSetParameter4fv(param, (const float*)vec.value);
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgDxShader::setIntVectorVariable(const char* name, const int4& vec) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgSetParameter4iv(param, (const int*)vec.value);
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgDxShader::setTextureVariable(const char* name, const TexturePtr& tex) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgD3D11SetTextureParameter(param, (ID3D11Resource*)tex->getTextureId());
                cgD3D11SetSamplerStateParameter(param, 0);
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgDxShader::getMatrixVariable(const char* name, Matrix4& mat) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgGetParameterValuefc(param, 16, (float*)mat.x);
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgDxShader::getFloatVectorVariable(const char* name, float4& vec) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgGetParameterValuefc(param, 4, (float*)vec.value);
                return _check_error(mContext);

            }
        }
        return false;
    }

    bool CgDxShader::getIntVectorVariable(const char* name, int4& vec) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgGetParameterValueic(param, 4, (int*)vec.value);
                return _check_error(mContext);
            }
        }
        return false;
    }

    CGprogram CgDxShader::getProgram() const {
        return mProgram;
    }

    CGcontext CgDxShader::getContext() const {
        return mContext;
    }

    const ShaderDesc& CgDxShader::getDesc() const {
        return mDesc;
    }


}
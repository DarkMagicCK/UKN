#include "CgShader.h"

#include "D3D11GraphicDevice.h"
#include "D3D11Debug.h"
#include "D3D11Texture.h"
#include "D3D11Helper.h"

#include "Cg/cgD3D11.h"

#include "mist/Logger.h"
#include "mist/Convert.h"

namespace ukn {
    
#define LOG_CG_ERROR(str, file)                                 \
        if(file) {                                              \
            log_error((FormatString(L"{0}: {1}"), file, str));   \
        }                                                       \
        else {                                                  \
            log_error(str);                                     \
        }                                                       \

    bool _check_error(CGcontext context, const wchar_t* file = 0) {
        CGerror error;
        const char* str = cgGetLastErrorString(&error);
        if(error != CG_NO_ERROR) {
            if(error == CG_COMPILER_ERROR) {
                const char* listing =  cgGetLastListing(context);
                if(listing) {
                    LOG_CG_ERROR(std::string(str) + listing, file);
                }
                else {
                    LOG_CG_ERROR(str, file);
                }
            } else {
                LOG_CG_ERROR(str, file);
            }
            return false;
        }
        return true;
    }

    CGprofile _d3d_feature_level_to_cgprofile(D3D_FEATURE_LEVEL level, ShaderType type) {
        switch(level) {
            case D3D_FEATURE_LEVEL_10_0:
            case D3D_FEATURE_LEVEL_10_1: {
                /* d3d10 shader model 4, 10.1 shader model 4.1 */
                switch(type){
                case ST_FragmentShader: return CG_PROFILE_PS_4_0;
                case ST_VertexShader: return CG_PROFILE_VS_4_0;
                case ST_GeometryShader: return CG_PROFILE_GS_4_0;
                }
                break;
            }

            case D3D_FEATURE_LEVEL_9_1:
                if(type == ST_FragmentShader)
                    return CG_PROFILE_PS_3_0;
                else
                    return CG_PROFILE_VS_3_0;
                // no geometry shader
                return CG_PROFILE_UNKNOWN;
                break;

            case D3D_FEATURE_LEVEL_11_0: {
                /* d3d11 shader model 5 */
                switch(type){
                case ST_FragmentShader: return CG_PROFILE_PS_5_0;
                case ST_VertexShader: return CG_PROFILE_VS_5_0;
                case ST_GeometryShader: return CG_PROFILE_GS_5_0;
                }
                break;
            }
        }
        
        switch(type){
            case ST_FragmentShader: return CG_PROFILE_PS_3_0;
            case ST_VertexShader: return CG_PROFILE_VS_3_0;
            case ST_GeometryShader: return CG_PROFILE_UNKNOWN;
        }
    }

    CgDxEffect::CgDxEffect(D3D11GraphicDevice* device):
        mContext(0),
        mDevice(device) {
            mContext = cgCreateContext();
            mist_assert(mContext);

            cgD3D11SetDevice(mContext, device->getD3DDevice());
          //  cgD3D11SetManageTextureParameters(mContext, CG_TRUE);
    }

    CgDxEffect::~CgDxEffect() {
        if(mContext) {
            cgDestroyContext(mContext);
        }
    }


    ShaderPtr CgDxEffect::createShader(const ResourcePtr& resource, const ShaderDesc& desc) {
        CgDxShader* shader = new CgDxShader(mContext);
        if(shader->initialize(mDevice, resource, desc)) {
            return ShaderPtr(shader);
        }
        return ShaderPtr();
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
            cgDestroyProgram(mProgram);
        }
    }

    bool CgDxShader::initialize(D3D11GraphicDevice* device, const ResourcePtr& resource, const ShaderDesc& desc) {
        mDesc = desc;
        mDevice = device;

        StreamPtr stream = resource->readIntoMemory();
        if(!stream)
            return false;
        MemoryStream* memStream = ((MemoryStream*)stream.get());
        std::string content(memStream->data(), memStream->data() + memStream->size());
        
        CGprofile profile = _d3d_feature_level_to_cgprofile(device->getDeviceFeatureLevel(), desc.type);
        mProgram = cgCreateProgram(mContext, 
                                   CG_SOURCE, 
                                   content.c_str(), 
                                   profile, 
                                   desc.entry.c_str(), 
                                   cgD3D11GetOptimalOptions(profile));
        if(_check_error(mContext, resource->getName().c_str()) &&
            D3D11Debug::CHECK_RESULT( cgD3D11LoadProgram(mProgram, 0))) {
                return true;
        }
        return false;
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
                cgSetMatrixParameterfc(param, (const float*)mat.x); 
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
        if(mProgram && tex) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            
            if(_check_error(mContext) && param) {
                cgD3D11SetTextureSamplerStateParameter(param, 
                                                       (ID3D11Resource*)tex->getTextureId(),
                                                       mDevice->getD3DSamplerState());
                return _check_error(mContext);
            }
        }
        return false;
    }


    bool CgDxShader::setFloatVariable(const char* name, uint32 len, const float* vals) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgSetParameterValuefc(param, len, (const float*)vals);
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgDxShader::setIntVariable(const char* name, uint32 len, const int* vals) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgSetParameterValueic(param, len, (const int*)vals);
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgDxShader::getMatrixVariable(const char* name, Matrix4& mat) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgGetMatrixParameterfc(param, (float*)mat.x);
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

    ID3DBlob* CgDxShader::getCompiledProgram() const {
        return cgD3D11GetCompiledProgram(this->getProgram());
    }

}
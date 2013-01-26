#include "CgShader.h"

#include "D3D11GraphicDevice.h"
#include "D3D11Debug.h"
#include "D3D11Texture.h"

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
    mContext(0) {
        mContext = cgCreateContext();
        mist_assert(mContext);

        cgD3D11SetDevice(mContext, device->getD3DDevice());
    }

    CgDxEffect::~CgDxEffect() {
        if(mContext) {
            cgDestroyContext(mContext);
        }
    }

    void CgDxEffect::onBind(uint32 pass) {
        if(mVertexShader)
            mVertexShader->bind();
        if(mFragmentShader)
            mFragmentShader->bind();
        if(mGeometryShader)
            mGeometryShader->bind();
    }

    void CgDxEffect::onUnbind() {

    }

    ShaderPtr CgDxEffect::createShader(const ResourcePtr& resource, const ShaderDesc& desc) {
        CgDxShader* shader = new CgDxShader(mContext);
        if(shader->initialize(resource, desc)) {
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
            profile = CG_PROFILE_PS_4_0;
            break;
        case ukn::ST_VertexShader:
            profile = CG_PROFILE_VS_4_0;
            break;
        case ukn::ST_GeometryShader:
            profile = cgD3D11GetLatestGeometryProfile();
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
                                   0);
        if(_check_error(mContext) &&
            D3D11Debug::CHECK_RESULT( cgD3D11LoadProgram(mProgram, D3DCOMPILE_DEBUG))) {
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
                D3DXMATRIX dxmat;
                Matrix4ToD3DMatrix(mat, dxmat);
                cgSetParameterValuefc(param, 16, (const float*)&dxmat); 
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
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgDxShader::getMatrixVariable(const char* name, Matrix4& mat) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                D3DXMATRIX dxmat;
                cgGetParameterValuefc(param, 16, (float*)&dxmat); 
                D3DXMatrixToMatrix4(dxmat, mat);
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

    void CgDxEffect::setLayout(const VertexFormat& format) {

    }

    struct D3D11ShaderUtilities {
		static ID3D11InputLayout* CreateLayout(ID3D11Device* device, const void* signature, SIZE_T size, const ukn::VertexFormat& format) {
			D3D11_INPUT_ELEMENT_DESC layoutDesc[5];

			UINT numElements = 0;

			if(format.checkFormat(VF_XYZ)) {
				layoutDesc[numElements].SemanticName = "POSITION";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetXYZ();
				layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_Color0)) {
				layoutDesc[numElements].SemanticName = "COLOR";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R8G8B8A8_UINT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetColor0();
				layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_UV)) {
				layoutDesc[numElements].SemanticName = "TEXCOORD";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R32G32_FLOAT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetUV();
				layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_Color1)) {
				layoutDesc[numElements].SemanticName = "COLOR1";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R8G8B8A8_UINT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetColor1();
				layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}
			if(format.checkFormat(VF_Normal)) {
				layoutDesc[numElements].SemanticName = "NORMAL";
				layoutDesc[numElements].SemanticIndex = 0;
				layoutDesc[numElements].Format = DXGI_FORMAT_R32G32B32_FLOAT;
				layoutDesc[numElements].InputSlot = 0;
				layoutDesc[numElements].AlignedByteOffset = format.offsetNormal();
				layoutDesc[numElements].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				layoutDesc[numElements].InstanceDataStepRate = 0;
				numElements += 1;
			}

			ID3D11InputLayout* layout = 0;

			HRESULT result = device->CreateInputLayout(layoutDesc,
				numElements,
				signature,
				size,
				&layout);
			if(!D3D11Debug::CHECK_RESULT(result)) {
				return 0;
			}
			return layout;
		}
		static void LogError(ID3D10Blob* error) {
			if(error) {
				std::string error_mssg((char*)error->GetBufferPointer(), (char*)error->GetBufferPointer() + error->GetBufferSize());
				log_error(error_mssg);
			}
		}

	};

    void CgDxShader::setLayout(const VertexFormat& format) {
        ID3D10Blob* blob = cgD3D11GetIASignatureByPass(0);
        mLayout = D3D11ShaderUtilities::CreateLayout(
            cgD3D11GetDevice(mContext), 
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            format);
		
    }


}
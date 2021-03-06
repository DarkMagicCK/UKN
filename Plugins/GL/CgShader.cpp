#include "CgShader.h"

#include "GLGraphicDevice.h"
#include "GLTexture.h"

#include "Cg/cgGL.h"

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

    CgGLEffect::CgGLEffect(GLGraphicDevice* device):
        mContext(0),
        mDevice(device) {
            mContext = cgCreateContext();
            mist_assert(mContext);
            
            cgGLSetManageTextureParameters(mContext, true);
    }

    CgGLEffect::~CgGLEffect() {
        if(mContext) {
            cgDestroyContext(mContext);
        }
    }

    ShaderPtr CgGLEffect::createShader(const ResourcePtr& resource, const ShaderDesc& desc) {
        if(resource) {
            CgGLShader* shader = new CgGLShader(mContext);
            if(shader->initialize(resource, desc)) {
                return ShaderPtr(shader);
            }
        } else {
            log_error("CgGLEffect::createShader: invalid resource");
        }
        return ShaderPtr();
    }

    CGcontext CgGLEffect::getContext() const {
        return mContext;
    }

    CgGLShader::CgGLShader(CGcontext context):
        mContext(context),
        mProgram(0) {

    }

    CgGLShader::~CgGLShader() {
        if(mProgram) {
            cgDestroyProgram(mProgram);
        }
    }

    bool CgGLShader::initialize(const ResourcePtr& resource, const ShaderDesc& desc) {
        CGprofile profile;
        mDesc = desc;
        switch (desc.type) {
        case ukn::ST_FragmentShader:
            profile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
            break;
        case ukn::ST_VertexShader:
            profile = cgGLGetLatestProfile(CG_GL_VERTEX);
            break;
        case ukn::ST_GeometryShader:
            profile = cgGLGetLatestProfile(CG_GL_GEOMETRY);
            
            break;
        }
        StreamPtr stream = resource->readIntoMemory();
        if(!stream)
            return false;
        
        MemoryStream* memStream = ((MemoryStream*)stream.get());
        std::string content(memStream->data(), memStream->data() + memStream->size());
        mProgram = cgCreateProgram(mContext, 
            CG_SOURCE, 
            content.c_str(), 
            profile, 
            desc.entry.c_str(), 
            cgGLGetOptimalOptions(profile));
        if(_check_error(mContext)) {
            cgGLLoadProgram(mProgram);
            mProfile = profile;
            return _check_error(mContext);
        }
        return false;;
    }

    void CgGLShader::bind() {
        if(mProgram) {
            cgGLEnableProfile(mProfile);
            cgGLBindProgram(mProgram);
        }
    }

    void CgGLShader::unbind() {
        if(mProgram) {
            cgGLUnbindProgram(mProfile);
        }
    }

    bool CgGLShader::setMatrixVariable(const char* name, const Matrix4& mat) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgSetMatrixParameterfc(param, (const float*)mat.x); 
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgGLShader::setFloatVectorVariable(const char* name, const float4& vec) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgSetParameter4fv(param, (const float*)vec.value);
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgGLShader::setIntVectorVariable(const char* name, const int4& vec) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgSetParameter4iv(param, (const int*)vec.value);
                return _check_error(mContext);
            }
        }
        return false;
    }

     bool CgGLShader::setFloatVariable(const char* name, uint32 len, const float* vals) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgSetParameterValuefc(param, len, (const float*)vals);
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgGLShader::setIntVariable(const char* name, uint32 len, const int* vals) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgSetParameterValueic(param, len, (const int*)vals);
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgGLShader::setTextureVariable(const char* name, const TexturePtr& tex) {
        if(mProgram && tex) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgGLSetTextureParameter(param, (GLuint)tex->getTextureId());
                cgSetSamplerState(param);
                
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgGLShader::getMatrixVariable(const char* name, Matrix4& mat) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgGetMatrixParameterfc(param, (float*)mat.x);
                return _check_error(mContext);
            }
        }
        return false;
    }

    bool CgGLShader::getFloatVectorVariable(const char* name, float4& vec) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgGetParameterValuefc(param, 4, (float*)vec.value);
                return _check_error(mContext);

            }
        }
        return false;
    }

    bool CgGLShader::getIntVectorVariable(const char* name, int4& vec) {
        if(mProgram) {
            CGparameter param = cgGetNamedParameter(mProgram, name);
            if(_check_error(mContext) && param) {
                cgGetParameterValueic(param, 4, (int*)vec.value);
                return _check_error(mContext);
            }
        }
        return false;
    }

    CGprogram CgGLShader::getProgram() const {
        return mProgram;
    }

    CGcontext CgGLShader::getContext() const {
        return mContext;
    }

    const ShaderDesc& CgGLShader::getDesc() const {
        return mDesc;
    }


}
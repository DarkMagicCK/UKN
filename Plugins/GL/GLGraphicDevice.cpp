//
//  GLRenderDevice.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLGraphicDevice.h"

#include "GLWindow.h"
#include "GLConvert.h"

#include "mist/Common.h"
#include "mist/Logger.h"
#include "mist/TimeUtil.h"
#include "mist/SysUtil.h"
#include "mist/Stream.h"
#include "mist/Profiler.h"
#include "mist/Convert.h"

#include "UKN/GraphicBuffer.h"
#include "UKN/RenderBuffer.h"
#include "UKN/Texture.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Window.h"
#include "UKN/Context.h"
#include "UKN/App.h"
#include "UKN/CgHelper.h"
#include "UKN/BlendStateObject.h"
#include "UKN/SamplerStateObject.h"
#include "UKN/RasterizerStateObject.h"

#include "GLFrameBuffer.h"
#include "GLRenderView.h"
#include "GLTexture.h"
#include "GLPreq.h"
#include "GLGraphicFactory.h"

#include "CgShader.h"


namespace ukn {

    GLGraphicDevice::GLGraphicDevice():
    mCurrGLFrameBuffer(0) {
    }

    GLGraphicDevice::~GLGraphicDevice() {

    }

    UknString GLGraphicDevice::description() const {
        static UknString des = string::StringToWString(
            format_string("OpenGL Graphic Device\nOpenGL Version: %s Vender: %s GLSL Version: %s",
            (char*)glGetString(GL_VERSION),
            (char*)glGetString(GL_VENDOR),
            (char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
        /* #ifdef UKN_DEBUG

        #if UKN_OPENGL_VERSION >= 30 && defined(UKN_REQUEST_OPENGL_3_2_PROFILE)
        int NumberOfExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);

        des += "\n\nGL Extensions:\n";
        for(int i=0; i<NumberOfExtensions; i++) {
        des += (const char*)glGetStringi(GL_EXTENSIONS, i);
        des += "\n";
        }
        #else
        des += "GL Extensions:\n ";
        des += (char*)glGetString(GL_EXTENSIONS);
        des += "\n\n";
        #endif

        #endif
        */
        return des;
    }

    WindowPtr GLGraphicDevice::doCreateRenderWindow(const UknString& name, const RenderSettings& settings) {
        try {
            mWindow = MakeSharedPtr<GLWindow>(name, settings);
        } catch(Exception& e) {
            MessageBox::Show(UknString(L"GLGraphic Device: Error creating opengl window and context.\n") + e.what(),
                L"Fatal Error",
                MBO_OK | MBO_IconError);
            Context::Instance().getApp().terminate();
            return WindowPtr();
        }

        GraphicDevice::bindFrameBuffer(((GLWindow*)mWindow.get())->getFrameBuffer());
        mScreenFrameBuffer = mCurrFrameBuffer;

        glCullFace(GL_NONE);
        //
        
        GraphicFactory& factory = Context::Instance().getGraphicFactory();
        
        this->setBlendState(factory.createBlendStateObject(BlendStateDesc()));
        this->setRasterizerState(factory.createRasterizerStateObject(RasterizerStateDesc()));
        this->setSamplerState(factory.createSamplerStateObject(SamplerStateDesc()));

        return mWindow;
    }

    void GLGraphicDevice::fillGraphicCaps(GraphicDeviceCaps& caps) {
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES, (GLint*)&caps.max_indices);
        glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, (GLint*)&caps.max_vertices);

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&caps.max_texture_height);
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&caps.max_texture_width);

        glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, (GLint*)&caps.max_texture_cube_map_size);

        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, (GLint*)&caps.max_pixel_texture_units);
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, (GLint*)&caps.max_vertex_texture_units);
    }

    void GLGraphicDevice::bindTexture(const TexturePtr& texture) {
        if(texture->type() == TT_Texture2D) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, (GLuint)texture->getTextureId());
        } else if(texture->type() == TT_Texture3D) {
            glEnable(GL_TEXTURE_3D);
            glBindTexture(GL_TEXTURE_3D, (GLuint)texture->getTextureId());
        } else {
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_3D);
        }
    }

    void GLGraphicDevice::renderBuffer(const RenderBufferPtr& buffer) {
#define BUFFER_OFFSET(buffer, i) (buffer->isInMemory() ? ((char*)buffer->map() + (i)) : ((char *)NULL + (i)))

        if(buffer.isValid()) {
            EffectPtr effect = mBindedEffect;
            GraphicBufferPtr vertexBuffer = buffer->getVertexStream();
            if(!vertexBuffer.isValid()) {
                log_error("ukn::GLGraphicDevice::onRenderBuffer: invalid vertex buffer stream");
                return;
            }

            vertexBuffer->activate();

            /* this need to be changed?
                since glVertexAttribPointer is introduced in 3.0 and 
                things like glVertexPointer is removed in 3.1
                but if we want backward-campability, it's better to stick with glVertexPointer ... ?
                and osx only have ogl 3.x after 10.7 lion...
                also attribute locations is causing problems in Cg shader, semantic names no longer working
                we have to write different shaders for gl and directx,
            */
            const vertex_elements_type& format = buffer->getVertexFormat();
            uint32 total_size = GetVertexElementsTotalSize(format);
            uint32 offset = 0;
            for(vertex_elements_type::const_iterator it = format.begin(),
                end = format.end();
                it != end;
                ++it) {
                // deprecated way of transmission vertex arrays
                // though backward compatible and avoid generic attrib locations in shaders that causes different versions of shaders
                // but also lack of some vertex usage support
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
                switch(it->usage) {
                    case VU_Position:
                        glVertexPointer(GetElementComponentSize(it->format),
                                        element_format_to_gl_element_type(it->format),
                                        total_size,
                                        BUFFER_OFFSET(vertexBuffer, offset));
                        glEnableClientState(GL_VERTEX_ARRAY);
                        break;
                            
                    case VU_Diffuse:
                        glColorPointer(GetElementComponentSize(it->format),
                                        element_format_to_gl_element_type(it->format),
                                        total_size,
                                        BUFFER_OFFSET(vertexBuffer, offset));
                        glEnableClientState(GL_COLOR_ARRAY);
                        break;
                            
                    case VU_Specular:
                        glColorPointer(GetElementComponentSize(it->format),
                                        element_format_to_gl_element_type(it->format),
                                        total_size,
                                        BUFFER_OFFSET(vertexBuffer, offset));
                        glEnableClientState(GL_COLOR_ARRAY);
                        break;
                            
                    case VU_Normal:
                        glNormalPointer(element_format_to_gl_element_type(it->format),
                                        total_size,
                                        BUFFER_OFFSET(vertexBuffer, offset));
                        glEnableClientState(GL_NORMAL_ARRAY);
                        break;
                            
                    case VU_UV:
                        glTexCoordPointer(GetElementComponentSize(it->format),
                                            element_format_to_gl_element_type(it->format),
                                            total_size,
                                            BUFFER_OFFSET(vertexBuffer, offset));
                        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                        break;
                            
                    default:
                        break;
                }
#else
                // ogl 3.x+
                    int attribLoc = vertex_usage_to_attribute_location(it->usage);
                    glVertexAttribPointer(attribLoc,
                                            GetElementComponentSize(it->format),
                                            element_format_to_gl_element_type(it->format),
                                            (it->usage == VU_Specular || it->usage == VU_Diffuse) ? GL_TRUE : GL_FALSE,
                                            total_size,
                                            BUFFER_OFFSET(vertexBuffer, offset));
                    glEnableVertexAttribArray(attribLoc);
                        
                    offset += it->size();
#endif
            }

                
            GraphicBufferPtr indexBuffer = buffer->getIndexStream();
            if(buffer->isUseIndexStream() &&
                indexBuffer.isValid()) {
                    indexBuffer->activate();
            }

            if(buffer->isUseIndexStream()) {
                if(!indexBuffer.isValid()) {
                    log_error("ukn::GLGraphicDevice::onRenderBuffer: invalid index stream");
                    return;
                }
                glDrawRangeElements(
                    render_mode_to_gl_mode(buffer->getRenderMode()),
                    buffer->getIndexStartIndex(),
                    0xffffffff,
                    buffer->getIndexCount(),
                    GL_UNSIGNED_INT,
                    BUFFER_OFFSET(vertexBuffer, buffer->getVertexStartIndex()));

            } else {
                glDrawArrays(
                    render_mode_to_gl_mode(buffer->getRenderMode()),
                                            buffer->getVertexStartIndex(),
                                            buffer->getVertexCount());
            }

            for(vertex_elements_type::const_iterator it = format.begin(),
                end = format.end();
                it != end;
                ++it) {
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
                switch(it->usage) {
                    case VU_Position:
                        glDisableClientState(GL_VERTEX_ARRAY);
                        break;
                            
                    case VU_Diffuse:
                        glDisableClientState(GL_COLOR_ARRAY);
                        break;
                            
                    case VU_Specular:
                        glDisableClientState(GL_COLOR_ARRAY);
                        break;
                            
                    case VU_Normal:
                        glDisableClientState(GL_NORMAL_ARRAY);
                        break;
                            
                    case VU_UV:
                        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                        break;
                            
                    default:
                        break;
                }
#else
                // ogl 3.x+
                glDisableVertexAttribArray(vertex_usage_to_attribute_location(it->usage) );
#endif
                }
        }


    }

    void GLGraphicDevice::bindGLFrameBuffer(GLuint fbo) {
        if(mCurrGLFrameBuffer != fbo) {
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
#else
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#endif
            mCurrGLFrameBuffer = fbo;
        }
    }

    void GLGraphicDevice::beginFrame() {
        mWindow->onFrameStart().raise(mWindow.get(), _NullEventArgs);
    }

    void GLGraphicDevice::endFrame() {
        mWindow->onFrameEnd().raise(mWindow.get(), _NullEventArgs);
    }

    void GLGraphicDevice::onBindFrameBuffer(const FrameBufferPtr& frameBuffer) {

    }

    void GLGraphicDevice::beginRendering() {
        FrameBuffer& fb = *this->getScreenFrameBuffer();

        FrameCounter& counter = FrameCounter::Instance();
        AppLauncher& app = Context::Instance().getApp();

        while(true) {

            if(fb.isActive()) {
                counter.waitToNextFrame();

                {            
                    MIST_PROFILE(L"__MainFrame__");

                    glViewport(fb.getViewport().left,
                        fb.getViewport().top,
                        fb.getViewport().width,
                        fb.getViewport().height);

                    fb.getViewport().camera->update();

                    setViewMatrix(fb.getViewport().camera->getViewMatrix());
                    setProjectionMatrix(fb.getViewport().camera->getProjMatrix());

                    app.update();
                    app.render();

                    if(mWindow->pullEvents())
                        break;

                    fb.swapBuffers();
                }

            }
        }
    }

    GLuint GLGraphicDevice::getBindedGLFrameBuffer() const {
        return mCurrGLFrameBuffer;
    }

    void GLGraphicDevice::setViewMatrix(const Matrix4& matx) {
        mViewMat = matx;
    }

    void GLGraphicDevice::setProjectionMatrix(const Matrix4& mat) {
        mProjectionMat = mat;
    }

    void GLGraphicDevice::getViewMatrix(Matrix4& mat) const {
        mat = mViewMat;
    }

    void GLGraphicDevice::getProjectionMatrix(Matrix4& mat) const {
        mat = mProjectionMat;
    }

    void GLGraphicDevice::setRenderState(RenderStateType type, uint32 func) {
        switch(type) {
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
        case RS_TextureWrap0:
        case RS_TextureWrap1:
        case RS_TextureWrap2:
        case RS_ColorOp:
            glTexEnvf(GL_TEXTURE_2D, 
                render_state_to_gl_state(type), 
                render_state_param_to_gl_state_param((RenderStateParam)func));
            break;
#endif

        case RS_Filter: {
            GLenum target = GL_TEXTURE_2D;
            switch(func) {
                case RSP_FilterMinLinearMagMipPoint:
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                    
                case RSP_FilterMinLinearMagPointMipLinear:
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                    
                case RSP_FilterMinMagLinearMipPoint:
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
                    
                case RSP_FilterMinMagMipLinear:
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
                    
                case RSP_FilterMinMagPointMipLinear:
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                    
                case RSP_FilterMinMapMipPoint:
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                    break;
                    
                case RSP_FilterMinPointMagLinearMipPoint:
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
                    
                case RSP_FilterMinPointMagMipLinear:
                    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    break;
                    
                    break;
            }
        }
            break;

        case RS_StencilOp:
        case RS_StencilFunc:
            break;

        case RS_DepthOp:
            glDepthFunc(render_state_param_to_gl_state_param((RenderStateParam)func));
            break;
        case RS_DepthMask:
            glDepthMask((func == RSP_Enable) ? GL_TRUE : GL_FALSE);
            break;

        case RS_SrcBlend:
        case RS_DstBlend:
        case RS_SrcAlpha:
        case RS_DstAlpha:
            glBlendFunc(render_state_to_gl_state(type), 
                render_state_param_to_gl_state_param((RenderStateParam)func));
            break;

        case RS_Blend:
            if(func == RSP_Enable)
                glEnable(GL_BLEND);
            else
                glDisable(GL_BLEND);
            break;

        case RS_DepthTest:
            if(func == RSP_Enable)
                glEnable(GL_DEPTH_TEST);
            else
                glDisable(GL_DEPTH_TEST);
            break;
                
        case RS_BlendOp:
            glBlendEquation(render_state_param_to_blend_equation((RenderStateParam)func));
            break;

        case RS_PointSize:
            glPointSize(mist::Convert::ReinterpretConvert<uint32, float>(func));
            break;
        }
    }

    void GLGraphicDevice::adjustPerspectiveMat(Matrix4& mat) {
        mat *= (Matrix4::ScaleMat(1.f, 1.f, 2.f) * Matrix4::TransMat(0.f, 0.f, -1.f));
    }

    void GLGraphicDevice::adjustOrthoMat(Matrix4& mat) {
        mat = Matrix4::LHToRH(mat);
    }

    void GLGraphicDevice::onSetBlendState(const BlendStatePtr& blendState) {
        const BlendStateDesc& desc = blendState->getDesc();
        
        this->setRenderState(RS_Blend, desc.blend_state.enabled ? RSP_Enable : RSP_Disable);
        this->setRenderState(RS_SrcBlend, desc.blend_state.src);
        this->setRenderState(RS_DstBlend, desc.blend_state.dst);
        this->setRenderState(RS_SrcAlpha, desc.blend_state.src_alpha);
        this->setRenderState(RS_DstAlpha, desc.blend_state.dst_alpha);
        this->setRenderState(RS_BlendOp, desc.blend_state.op);
        
        /* rgba blend factor */
        glBlendColor(desc.blend_factor.value[0],
                     desc.blend_factor.value[1],
                     desc.blend_factor.value[2],
                     desc.blend_factor.value[3]);
    }
    
    void GLGraphicDevice::onSetSamplerState(const SamplerStatePtr& samplerState, uint32 index) {
        const SamplerStateDesc& desc = samplerState->getDesc();
        this->setRenderState(RS_Filter, desc.filter);
        this->setRenderState(RS_TextureWrap0, desc.address_u);
        this->setRenderState(RS_TextureWrap1, desc.address_v);
        this->setRenderState(RS_TextureWrap2, desc.address_w);
    }

    void GLGraphicDevice::onSetRasterizerState(const RasterizerStatePtr& rasterizerState) {
        const RasterizerStateDesc& desc = rasterizerState->getDesc();
        this->setRenderState(RS_CullFace, desc.cullface);
        this->setRenderState(RS_FillMode, desc.fillmode);
        this->setRenderState(RS_FrontFace, desc.frontface);
    }

} // namespace ukn
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
#include "UKN/DepthStencilStateObject.h"

#include "GLFrameBuffer.h"
#include "GLRenderView.h"
#include "GLTexture.h"
#include "GLPreq.h"
#include "GLGraphicFactory.h"
#include "GLError.h"

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

    std::vector<int> GLGraphicDevice::applyBuffer(const vertex_elements_type& format, uint32 size, uint32 instance_freq) {
        #define BUFFER_OFFSET(off) (((char*)0) + off)
             
        if(instance_freq > 0 &&
            !glVertexAttribDivisor) {
            // if instancing is required but not supported
            // just return
                return std::vector<int>();
        }
        /* this need to be changed?
            since glVertexAttribPointer is introduced in 3.0 and 
            things like glVertexPointer is removed in 3.1
            but if we want backward-campability, it's better to stick with glVertexPointer ... ?
            and osx only have ogl 3.x after 10.7 lion...
            also attribute locations is causing problems in Cg shader, semantic names no longer working
            we have to write different shaders for gl and directx,
            but ... instancing requires vertex attribute..
            so anyway we need a shader compiler first
        */
        uint32 offset = 0;
        std::vector<int> attributes;
        for(vertex_elements_type::const_iterator it = format.begin(),
            end = format.end();
            it != end;
            ++it) {
            // deprecated way of transmission vertex arrays
            // though backward compatible and avoid generic attrib locations in shaders
            // but also lack of some vertex usage support

#if 1
            switch(it->usage) {
                case VU_Position: {
                    glEnableClientState(GL_VERTEX_ARRAY);

                    CHECK_GL_CALL(glVertexPointer(GetElementComponentSize(it->format),
                                    GL_FLOAT,
                                    size,
                                    BUFFER_OFFSET(offset)));
                   
                    attributes.push_back(GL_VERTEX_ARRAY);
                    break;
                }
                    
                case VU_Diffuse:
                    glEnableClientState(GL_COLOR_ARRAY);

                    CHECK_GL_CALL(glColorPointer(GetElementComponentSize(it->format),
                                    element_format_to_gl_data_type(it->format),
                                    size,
                                    BUFFER_OFFSET(offset)));
                    
                    attributes.push_back(GL_COLOR_ARRAY);
                    break;
                            
                case VU_Specular:
                    glEnableClientState(GL_COLOR_ARRAY);

                    glColorPointer(GetElementComponentSize(it->format),
                                    element_format_to_gl_data_type(it->format),
                                    size,
                                    BUFFER_OFFSET(offset));

                    attributes.push_back(GL_COLOR_ARRAY);
                    break;
                            
                case VU_Normal:
                    glEnableClientState(GL_NORMAL_ARRAY);

                    CHECK_GL_CALL(glNormalPointer(element_format_to_gl_data_type(it->format),
                                    size,
                                    BUFFER_OFFSET(offset)));

                    attributes.push_back(GL_NORMAL_ARRAY);
                    break;
                            
                case VU_UV:
                    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

                    CHECK_GL_CALL(glTexCoordPointer(GetElementComponentSize(it->format),
                                      GL_FLOAT,
                                        size,
                                        BUFFER_OFFSET(offset)));

                    attributes.push_back(GL_TEXTURE_COORD_ARRAY);
                    break;
                            
                default:
                    break;
            }
#else
            // ogl 3.x+
                int attribLoc = vertex_usage_to_attribute_location(it->usage);
                CHECK_GL_CALL(glVertexAttribPointer(attribLoc,
                                        GetElementComponentSize(it->format),
                                        element_format_to_gl_element_type(it->format),
                                        (it->usage == VU_Specular || it->usage == VU_Diffuse) ? GL_TRUE : GL_FALSE,
                                        size,
                                        BUFFER_OFFSET(offset)));
                glEnableVertexAttribArray(attribLoc);
                CHECK_GL_CALL(glVertexAttribDivisor(attribLoc, instance_freq));
                        

                attributes.push_back(attribLoc);
#endif
            offset += it->size();
            
        }
        return attributes;

    }

    void GLGraphicDevice::renderBuffer(const EffectTechniquePtr& technique, const RenderBufferPtr& buffer) {
        if(buffer) {
            const RenderBuffer::VertexStreamVec& vertex_streams = buffer->getVertexStreams();

            uint32 total_stream_size = 0;
            std::vector<int> attributes;
            for(const RenderBuffer::StreamObject& so: vertex_streams) {
                const GraphicBufferPtr& vertexBuffer = so.stream;
                
                // bind vbo and vertex attributes
                vertexBuffer->activate();
                
                std::vector<int> appliedAttr = this->applyBuffer(so.format, so.vertex_size, 0);
                attributes.insert(attributes.end(), appliedAttr.begin(), appliedAttr.end());

                total_stream_size += so.vertex_size;
            }

            if(buffer->hasInstanceStream()) {
                const GraphicBufferPtr& instanceStream = buffer->getInstanceStream();
                instanceStream->activate();
                std::vector<int> appliedAttr = this->applyBuffer(buffer->getInstanceFormat(), 
                                  buffer->getInstanceFormatSize(), 
                                  1);
                attributes.insert(attributes.end(), appliedAttr.begin(), appliedAttr.end());

            }
               
            const GraphicBufferPtr& indexBuffer = buffer->getIndexStream();
            if(buffer->isUseIndexStream() &&
                indexBuffer) {
                    indexBuffer->activate();
            }

            for(uint32 i=0; i<technique->getNumPasses(); ++i) {
                const EffectPassPtr& pass = technique->getPass(i);
                   
                pass->begin();
                if(!buffer->hasInstanceStream()) {
                    if(buffer->isUseIndexStream()) {
                        CHECK_GL_CALL(glDrawRangeElements(
                            render_mode_to_gl_mode(buffer->getRenderMode()),
                            buffer->getIndexStartIndex(),
                            0xffffffff,
                            buffer->getIndexCount(),
                            GL_UNSIGNED_INT,
                            BUFFER_OFFSET(0)));

                    } else {
                        CHECK_GL_CALL(glDrawArrays(
                            render_mode_to_gl_mode(buffer->getRenderMode()),
                            buffer->getVertexStartIndex(),
                            buffer->getVertexCount()));
                    }
                } else {
                    if(glDrawElementsInstanced &&
                        glDrawArraysInstanced) {
                        if(buffer->isUseIndexStream()) {
                            CHECK_GL_CALL(glDrawElementsInstanced(
                                render_mode_to_gl_mode(buffer->getRenderMode()),
                                buffer->getInstanceCount(),
                                GL_UNSIGNED_INT,
                                BUFFER_OFFSET(0),
                                buffer->getInstanceCount()));

                        } else {
                            CHECK_GL_CALL(glDrawArraysInstanced(
                                render_mode_to_gl_mode(buffer->getRenderMode()),
                                buffer->getVertexStartIndex(),
                                buffer->getVertexCount(),
                                buffer->getInstanceCount()));
                        }
                    } else {
                        // fallback, assume that we are using software instancing...
                        for(uint32 i=0; i<buffer->getInstanceCount(); ++i) {
                            if(buffer->isUseIndexStream()) {
                                CHECK_GL_CALL(glDrawRangeElements(
                                    render_mode_to_gl_mode(buffer->getRenderMode()),
                                    buffer->getIndexStartIndex(),
                                    0xffffffff,
                                    buffer->getIndexCount(),
                                    GL_UNSIGNED_INT,
                                    BUFFER_OFFSET(0)));

                            } else {
                                CHECK_GL_CALL(glDrawArrays(
                                    render_mode_to_gl_mode(buffer->getRenderMode()),
                                    buffer->getVertexStartIndex(),
                                    buffer->getVertexCount()));
                            }
                        }
                    }
                }
                pass->end();
            }
            for(const RenderBuffer::StreamObject& so: vertex_streams) {
                so.stream->deactivate();
            }
            if(buffer->isUseIndexStream() &&
               indexBuffer) {
                indexBuffer->deactivate();
            }
            for(auto attr: attributes) {
#if 1
                glDisableClientState(attr);
#else
                // ogl 3.x+
                glDisableVertexAttribArray(attr);
#endif
            }
        }  
    }

    void GLGraphicDevice::bindGLFrameBuffer(GLuint fbo) {
        if(mCurrGLFrameBuffer != fbo) {
#ifndef UKN_OSX_REQUEST_OPENGL_32_CORE_PROFILE
            CHECK_GL_CALL(glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo));
#else
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
#endif
            mCurrGLFrameBuffer = fbo;
            check_framebuffer_status();
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
        case RS_ColorOp:
            CHECK_GL_CALL(glTexEnvf(GL_TEXTURE_2D, 
                render_state_to_gl_state(type), 
                render_state_param_to_gl_state_param((RenderStateParam)func)));
            break;
#endif
                
        case RS_TextureWrap0:
        case RS_TextureWrap1:
        case RS_TextureWrap2:
                CHECK_GL_CALL(glTexParameteri(GL_TEXTURE_2D,
                                render_state_to_gl_state(type),
                                render_state_param_to_gl_state_param((RenderStateParam)func)));

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
                    
                case RSP_FilterMinMagMipPoint:
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
            CHECK_GL_CALL(glDepthFunc(render_state_param_to_gl_state_param((RenderStateParam)func)));
            break;
        case RS_DepthMask:
            glDepthMask((func == RSP_Enable) ? GL_TRUE : GL_FALSE);
            break;

        case RS_SrcBlend:
        case RS_DstBlend:
        case RS_SrcAlpha:
        case RS_DstAlpha:
            CHECK_GL_CALL(glBlendFunc(render_state_to_gl_state(type), 
                render_state_param_to_gl_state_param((RenderStateParam)func)));
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
                
        case RS_CullFace:
        case RS_FillMode:
        case RS_FrontFace:
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

    void GLGraphicDevice::onSetDepthStencilState(const DepthStencilStatePtr& depthStencilState) {
        const DepthStencilStateDesc& desc = depthStencilState->getDesc();

        if(desc.depth_enable)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        glDepthMask(desc.depth_write_mask);
        glDepthFunc(render_state_param_to_gl_state_param(desc.depth_func));

        if(desc.stencil_enable)
            glEnable(GL_STENCIL_TEST);
        else
            glDisable(GL_STENCIL_TEST);
        glStencilMask(desc.stencil_write_mask);

        CHECK_GL_CALL(glStencilOpSeparate(GL_FRONT,
                                          render_state_param_to_gl_state_param(desc.front.stencil_fail_op),
                                          render_state_param_to_gl_state_param(desc.front.depth_fail_op),
                                          render_state_param_to_gl_state_param(desc.front.pass_op)));
        CHECK_GL_CALL(glStencilOpSeparate(GL_BACK,
                                          render_state_param_to_gl_state_param(desc.back.stencil_fail_op),
                                          render_state_param_to_gl_state_param(desc.back.depth_fail_op),
                                          render_state_param_to_gl_state_param(desc.back.pass_op)));
        CHECK_GL_CALL(glStencilFuncSeparate(GL_FRONT,
                                            render_state_param_to_gl_state_param(desc.front.func),
                                            desc.stencil_ref,
                                            0xFFFFFFFF));
        CHECK_GL_CALL(glStencilFuncSeparate(GL_BACK,
                                            render_state_param_to_gl_state_param(desc.back.func),
                                            desc.stencil_ref,
                                            0xFFFFFFFF));

    }

} // namespace ukn
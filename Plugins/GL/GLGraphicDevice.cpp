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

#include "UKN/Common.h"
#include "UKN/GraphicBuffer.h"
#include "UKN/RenderBuffer.h"
#include "UKN/Logger.h"
#include "UKN/Texture.h"
#include "UKN/FrameBuffer.h"
#include "UKN/Window.h"
#include "UKN/TimeUtil.h"
#include "UKN/Context.h"
#include "UKN/App.h"
#include "UKN/SysUtil.h"
#include "UKN/Stream.h"
#include "UKN/Profiler.h"

#include "GLFrameBuffer.h"
#include "GLRenderView.h"
#include "GLTexture.h"

namespace ukn {
    
    GLGraphicDevice::GLGraphicDevice():
    mCurrTexture(TexturePtr()),
    mCurrGLFrameBuffer(0) {
    }
    
    GLGraphicDevice::~GLGraphicDevice() {
        
    }
    
    ukn_string GLGraphicDevice::description() const {
        static ukn_string des = format_string("OpenGL Graphic Device\nOpenGL Version: %s Vender: %s GLSL Version: %s",
                                              (char*)glGetString(GL_VERSION),
                                              (char*)glGetString(GL_VENDOR),
                                             (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
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
    
    WindowPtr GLGraphicDevice::doCreateRenderWindow(const ukn_string& name, const RenderSettings& settings) {
        try {
            mWindow = MakeSharedPtr<GLWindow>(name, settings);
        } catch(Exception& e) {
			message_box(format_string("GLGraphic Device: Error creating opengl window, error %s", e.what()), "Fatal Error", MBO_OK | MBO_IconError);
			Context::Instance().getApp().terminate();
            return WindowPtr();
        }
        
        GraphicDevice::bindFrameBuffer(((GLWindow*)mWindow.get())->getFrameBuffer());
        mScreenFrameBuffer = mCurrFrameBuffer;
        
        //
        
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
    
    void GLGraphicDevice::bindTexture(TexturePtr texture) {
        mCurrTexture = texture;
        
        if(mCurrTexture) {
            if(mCurrTexture->getType() == TT_Texture2D) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, (GLuint)mCurrTexture->getTextureId());
            } else if(mCurrTexture->getType() == TT_Texture3D) {
                glEnable(GL_TEXTURE_3D);
                glBindTexture(GL_TEXTURE_3D, (GLuint)mCurrTexture->getTextureId());
            }
        } else {
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_TEXTURE_3D);
        }
    }
    
    void GLGraphicDevice::onRenderBuffer(const RenderBufferPtr& buffer) {
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
        
        ukn_assert(buffer.isValid());
        
        GraphicBufferPtr vertexBuffer = buffer->getVertexStream();
        if(!vertexBuffer.isValid()) {
            log_error("ukn::GLGraphicDevice::onRenderBuffer: invalid vertex buffer stream");
            return;
        }
        
        const VertexFormat& format = buffer->getVertexFormat();
        if(format == Vertex2D::Format()) {
            // acceleration for 2d vertices
            Array<Vertex2D> vtxArr((Vertex2D*)vertexBuffer->map(), vertexBuffer->count());
            vertexBuffer->unmap();
            
            glInterleavedArrays(GL_T2F_C4UB_V3F, 0, vtxArr.begin());
            glDrawArrays(render_mode_to_gl_mode(buffer->getRenderMode()),
                         0,
                         vertexBuffer->count());
            return;
        }

        vertexBuffer->activate();
        
        if(format.checkFormat(VF_XYZ)) {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3,
                            GL_FLOAT,
                            format.totalSize(),
                            BUFFER_OFFSET(format.offsetXYZ()));
        }
        
        if(format.checkFormat(VF_Normal)) {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT,
                            format.totalSize(),
                            BUFFER_OFFSET(format.offsetNormal()));
        }
        
        if(format.checkFormat(VF_Color0)) {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4,
                           GL_UNSIGNED_BYTE,
                           format.totalSize(),
                           BUFFER_OFFSET(format.offsetColor0()));
        }
        
        if(format.checkFormat(VF_Color1)) {
            glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
            glSecondaryColorPointer(4,
                                    GL_UNSIGNED_BYTE,
                                    format.totalSize(),
                                    BUFFER_OFFSET(format.offsetColor1()));
        }
        
        if(format.checkFormat(VF_UV)) {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2,
                              GL_FLOAT,
                              format.totalSize(),
                              BUFFER_OFFSET(format.offsetUV()));
        } else {
            glDisable(GL_TEXTURE_2D);
        }
        
        if(buffer->isUseIndexStream()) {
            GraphicBufferPtr indexBuffer = buffer->getIndexStream();
            if(!indexBuffer.isValid()) {
                log_error("ukn::GLGraphicDevice::onRenderBuffer: invalid index stream");
                return;
            }
            
            indexBuffer->activate();
            
            glDrawRangeElements(render_mode_to_gl_mode(buffer->getRenderMode()),
                                0,
                                0xffffffff,
                                indexBuffer->count(),
                                GL_UNSIGNED_INT,
                                BUFFER_OFFSET(0));
        } else {
            glDrawArrays(render_mode_to_gl_mode(buffer->getRenderMode()),
                         0,
                         vertexBuffer->count());
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
    }
    
    void GLGraphicDevice::bindGLFrameBuffer(GLuint fbo) {
        if(mCurrGLFrameBuffer != fbo) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
            mCurrGLFrameBuffer = fbo;
        }
    }
    
    void GLGraphicDevice::beginFrame() {
        mWindow->onFrameStart().raise(this, _NullEventArgs);
    }
    
    void GLGraphicDevice::endFrame() {
        mWindow->onFrameEnd().raise(this, _NullEventArgs);
    }
    
    void GLGraphicDevice::onBindFrameBuffer(const FrameBufferPtr& frameBuffer) {
        
    }
    
    void GLGraphicDevice::beginRendering() {
        FrameBuffer& fb = *this->getScreenFrameBuffer();
        
        FrameCounter& counter = FrameCounter::Instance();
        AppInstance& app = Context::Instance().getApp();
        
        while(true) {

            if(fb.isActive()) {
                counter.waitToNextFrame();
                
                {            
                    UKN_PROFILE("MainFrame");

                    glViewport(fb.getViewport().left,
                               fb.getViewport().top,
                               fb.getViewport().width,
                               fb.getViewport().height);
                    
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
    
    void GLGraphicDevice::setViewMatrix(const Matrix4& mat) {
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(&mat.x[0]);
    }
    
    void GLGraphicDevice::setProjectionMatrix(const Matrix4& mat) {
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(&mat.x[0]);
    }
    
    void GLGraphicDevice::getViewMatrix(Matrix4& mat) {
        glGetFloatv(GL_MODELVIEW_MATRIX, &mat.x[0]);
    }
    
    void GLGraphicDevice::getProjectionMatrix(Matrix4& mat) {
        glGetFloatv(GL_PROJECTION_MATRIX, &mat.x[0]);
    }
    
    void GLGraphicDevice::setRenderState(RenderStateType type, RenderStateParam func) {
        switch(type) {
            case RS_TextureWrap0:
            case RS_TextureWrap1:
            case RS_ColorOp:
                glTexEnvf(GL_TEXTURE_2D, 
                          render_state_to_gl_state(type), 
                          render_state_param_to_gl_state_param(func));
                break;
                
                
            case RS_MinFilter:
            case RS_MagFilter:
                glTexParameteri(GL_TEXTURE_2D, 
                                render_state_to_gl_state(type), 
                                render_state_param_to_gl_state_param(func));
                break;
                
            case RS_StencilOp:
            case RS_StencilFunc:
                break;
                
            case RS_DepthOp:
                glDepthFunc(render_state_param_to_gl_state_param(func));
                break;
            case RS_DepthMask:
                glDepthMask((func == RSP_Enable) ? GL_TRUE : GL_FALSE);
                break;
                
            case RS_SrcBlend:
            case RS_DstBlend:
            case RS_SrcAlpha:
            case RS_DstAlpha:
                glBlendFunc(render_state_to_gl_state(type), 
                            render_state_param_to_gl_state_param(func));
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
        }
    }
    
} // namespace ukn
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
//#ifdef UKN_DEBUG

#if UKN_OPENGL_VERSION >= 30
		int NumberOfExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
		
		des += "\n\nGL Extensions:\n";
        for(int i=0; i<NumberOfExtensions; i++) {
            des += (const char*)glGetStringi(GL_EXTENSIONS, i);
			des += "\n";
		}
#else
		des += format_string("\n\nGL Extensions:\n %s\n", glGetString(GL_EXTENSIONS));
#endif

//#endif
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
        
        bindFrameBuffer(checked_cast<GLWindow*>(mWindow.get()));
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
    
    void GLGraphicDevice::bindTexture(TexturePtr texture) {
        mCurrTexture = texture;
    }
    
    void GLGraphicDevice::onRenderBuffer(const RenderBufferPtr& buffer) {
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
        
        ukn_assert(buffer.isValid());
        
        GraphicBufferPtr vertexBuffer = buffer->getVertexStream();
        if(!vertexBuffer.isValid()) {
            log_error("ukn::GLGraphicDevice::onRenderBuffer: invalid vertex buffer stream");
            return;
        }
        
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
        }
        
        const VertexFormat& format = buffer->getVertexFormat();
        
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
                log_error("ukn::GLGraphicDevice::onRenderBuffer: invalid vertex buffer format");
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
    
    void GLGraphicDevice::setViewMatrix(const Matrix4& mat) {
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(&mat.x[0]);
    }
    
    void GLGraphicDevice::bindGLFrameBuffer(GLuint fbo) {
        if(mCurrGLFrameBuffer != fbo) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
            mCurrGLFrameBuffer = fbo;
        }
    }
    
    void GLGraphicDevice::beginFrame() {
        mWindow->onFrameStart()(*mWindow);
    }
    
    void GLGraphicDevice::endFrame() {
        mWindow->onFrameEnd()(*mWindow);
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
                
                app.update();
                app.render();
                
                if(mWindow->pullEvents())
                    break;
                
                fb.swapBuffers();
            }
        }
    }
    
    GLuint GLGraphicDevice::getBindedGLFrameBuffer() const {
        return mCurrGLFrameBuffer;
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
    
} // namespace ukn
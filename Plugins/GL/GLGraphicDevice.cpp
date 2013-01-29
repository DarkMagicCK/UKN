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

#include "GLFrameBuffer.h"
#include "GLRenderView.h"
#include "GLTexture.h"
#include "GLPreq.h"

#include "CgShader.h"

namespace ukn {

    GLGraphicDevice::GLGraphicDevice():
        mCurrTexture(TexturePtr()),
        mCurrGLFrameBuffer(0) {
    }

    GLGraphicDevice::~GLGraphicDevice() {

    }

    UknString GLGraphicDevice::description() const {
        static UknString des = string::StringToWString(format_string("OpenGL Graphic Device\nOpenGL Version: %s Vender: %s GLSL Version: %s",
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
            MessageBox::Show(string::StringToWString(format_string("GLGraphic Device: Error creating opengl window, error %s",
                e.what())),
                L"Fatal Error",
                MBO_OK | MBO_IconError);
            Context::Instance().getApp().terminate();
            return WindowPtr();
        }

        GraphicDevice::bindFrameBuffer(((GLWindow*)mWindow.get())->getFrameBuffer());
        mScreenFrameBuffer = mCurrFrameBuffer;

        glCullFace(GL_NONE);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
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

    void GLGraphicDevice::bindTexture(const TexturePtr& texture) {
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

    void GLGraphicDevice::renderBuffer(const RenderBufferPtr& buffer) {
#define BUFFER_OFFSET(buffer, i) (buffer->isInMemory() ? ((char*)buffer->map() + (i)) : ((char *)NULL + (i)))

        if(buffer.isValid()) {
            EffectPtr effect = buffer->getEffect();
            if(effect) {
                /* temporary */
                if(effect->getVertexShader()) {
                    if(!effect->getVertexShader()->setMatrixVariable("worldMatrix", mWorldMat))
                        log_error("error setting world matrix in effect");
                    if(!effect->getVertexShader()->setMatrixVariable("projectionMatrix", mProjectionMat))
                        log_error("error setting projection matrix in effect");
                    if(!effect->getVertexShader()->setMatrixVariable("viewMatrix", mViewMat))
                        log_error("error setting view matrix in effect");
                }
                if(effect->getFragmentShader()) {
                    if(mCurrTexture) {
                        if(!effect->getFragmentShader()->setTextureVariable("tex", mCurrTexture))
                            log_error("error setting texture in effect");
                    }
                }

                GraphicBufferPtr vertexBuffer = buffer->getVertexStream();
                if(!vertexBuffer.isValid()) {
                    log_error("ukn::GLGraphicDevice::onRenderBuffer: invalid vertex buffer stream");
                    return;
                }

                const VertexFormat& format = buffer->getVertexFormat();
                if(format == Vertex2D::Format() &&
                    !buffer->isUseIndexStream()) {
                        // acceleration for 2d vertices

                        if(vertexBuffer->isInMemory()) {
                            glInterleavedArrays(GL_T2F_C4UB_V3F, 0, vertexBuffer->map());

                            for(uint32 i=0; i<effect->getPasses(); ++i) {
                                effect->bind(i);

                                glDrawArrays(render_mode_to_gl_mode(buffer->getRenderMode()),
                                    buffer->getVertexStartIndex(),
                                    buffer->getVertexCount());
                            }
                            return;
                        }
                }

                vertexBuffer->activate();

                if(format.checkFormat(VF_XYZ)) {
                    glEnableClientState(GL_VERTEX_ARRAY);
                    glVertexPointer(3,
                        GL_FLOAT,
                        format.totalSize(),
                        BUFFER_OFFSET(vertexBuffer, format.offsetXYZ()));
                }

                if(format.checkFormat(VF_Normal)) {
                    glEnableClientState(GL_NORMAL_ARRAY);
                    glNormalPointer(GL_FLOAT,
                        format.totalSize(),
                        BUFFER_OFFSET(vertexBuffer, format.offsetNormal()));
                }

                if(format.checkFormat(VF_Color0)) {
                    glEnableClientState(GL_COLOR_ARRAY);
                    glColorPointer(4,
                        GL_UNSIGNED_BYTE,
                        format.totalSize(),
                        BUFFER_OFFSET(vertexBuffer, format.offsetColor0()));
                }

                if(format.checkFormat(VF_Color1)) {
                    glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
                    glSecondaryColorPointer(4,
                        GL_UNSIGNED_BYTE,
                        format.totalSize(),
                        BUFFER_OFFSET(vertexBuffer, format.offsetColor1()));
                }

                if(format.checkFormat(VF_UV)) {
                    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                    glTexCoordPointer(2,
                        GL_FLOAT,
                        format.totalSize(),
                        BUFFER_OFFSET(vertexBuffer, format.offsetUV()));
                }

                GraphicBufferPtr indexBuffer = buffer->getIndexStream();
                if(buffer->isUseIndexStream() &&
                    indexBuffer.isValid()) {
                        indexBuffer->activate();
                }

                for(uint32 i=0; i<effect->getPasses(); ++i) {
                    effect->bind(i);

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
                }
            }
        }


    }

    void GLGraphicDevice::bindGLFrameBuffer(GLuint fbo) {
        if(mCurrGLFrameBuffer != fbo) {
            glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
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

    SharedPtr<uint8> GLGraphicDevice::readFrameBufferData(const FrameBufferPtr& buffer, int32 x, int32 y, uint32 width, uint32 height, ElementFormat format) {
        return ((GLFrameBuffer*)(buffer.get()))->readFrameBufferData(x,
            y,
            width,
            height,
            format);
    }

    void GLGraphicDevice::setViewMatrix(const Matrix4& matx) {
        mViewMat = matx;
    }

    void GLGraphicDevice::setWorldMatrix(const Matrix4& mat) {
        mWorldMat = mat;
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

    void GLGraphicDevice::getWorldMatrix(Matrix4& mat) const {
        mat = mWorldMat;
    }

    void GLGraphicDevice::bindEffect(const EffectPtr& effect) {
        mEffect = effect;
    }

    void GLGraphicDevice::setRenderState(RenderStateType type, uint32 func) {
        switch(type) {
        case RS_TextureWrap0:
        case RS_TextureWrap1:
        case RS_ColorOp:
            glTexEnvf(GL_TEXTURE_2D, 
                render_state_to_gl_state(type), 
                render_state_param_to_gl_state_param((RenderStateParam)func));
            break;


        case RS_MinFilter:
        case RS_MagFilter:
            glTexParameteri(GL_TEXTURE_2D, 
                render_state_to_gl_state(type), 
                render_state_param_to_gl_state_param((RenderStateParam)func));
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

        case RS_PointSize:
            glPointSize(mist::Convert::ReinterpretConvert<uint32, float>(func));
            break;
        }
    }

    void GLGraphicDevice::adjustPerspectiveMat(Matrix4& mat) {
        mat *= (Matrix4::ScaleMat(1.f, 1.f, 2.f) * Matrix4::TransMat(0.f, 0.f, -1.f));
    }

    void GLGraphicDevice::switchTo2D(const OrthogonalParams& params) {

    }

} // namespace ukn
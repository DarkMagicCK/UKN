//
//  GLGraphicBufer.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/7/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLGraphicBuffer.h"
#include "GLConvert.h"
#include "GLPreq.h"
#include "GLError.h"

namespace ukn {

    inline GLenum _usage_to_gl(GraphicBuffer::Usage usage) {
        switch(usage) {
        case GraphicBuffer::Dynamic:
            return GL_DYNAMIC_DRAW;
        case GraphicBuffer::Static:
            return GL_STATIC_DRAW;
        case GraphicBuffer::Staging:
            return GL_DYNAMIC_COPY;
        }
    }

    GLVertexBuffer::GLVertexBuffer(GraphicBuffer::Access access,
                                   GraphicBuffer::Usage usage,
                                   uint32 desired_count,
                                   const void* initData,
                                   const vertex_elements_type& format):
    GraphicBuffer(access, usage),
    mFormat(format),
    mMaped(false) {

        glGenBuffers(1, &mId);
        glBindBuffer(GL_ARRAY_BUFFER, mId);

        if(initData) {
            CHECK_GL_CALL(glBufferData(GL_ARRAY_BUFFER,
                                  static_cast<GLsizeiptr>(desired_count * GetVertexElementsTotalSize(format)),
                                  static_cast<const GLvoid*>(initData),
                                  _usage_to_gl(usage)));

        } else {
            CHECK_GL_CALL(glBufferData(GL_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(desired_count * GetVertexElementsTotalSize(format)),
                         0,
                         _usage_to_gl(usage)));
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        mCount = desired_count;
    }

    GLVertexBuffer::~GLVertexBuffer() {
        glDeleteBuffers(1, &mId);
    }

    void* GLVertexBuffer::map() {
        if(access() == None) {
            log_error("GLVertexBuffer: trying to map buffer with access = None");
            return 0;
        }
        if(mMaped)
            unmap();
        
        GLenum gl_acess = GL_READ_ONLY;

        switch (access()) {
            case GraphicBuffer::ReadOnly:
                gl_acess = GL_READ_ONLY;
                break;

            case GraphicBuffer::ReadWrite:
                gl_acess = GL_READ_WRITE;
                break;

            case GraphicBuffer::WriteOnly:
                gl_acess = GL_WRITE_ONLY;
                break;
                
            default:
                break;
        }

        glBindBuffer(GL_ARRAY_BUFFER, mId);
        void* p = CHECK_GL_CALL(glMapBuffer(GL_ARRAY_BUFFER, gl_acess));
        if(p)
            mMaped = true;
        return p;
    }

    void GLVertexBuffer::unmap() {
        if(mMaped) {
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            mMaped = false;
        }
    }

    void GLVertexBuffer::activate() {
        glBindBuffer(GL_ARRAY_BUFFER, mId);

    }
    
    void GLVertexBuffer::deactivate() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    void GLVertexBuffer::resize(uint32 desired_count) {
        glBindBuffer(GL_ARRAY_BUFFER, mId);

        CHECK_GL_CALL(glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(desired_count * GetVertexElementsTotalSize(mFormat)),
                     0,
                     usage() == Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        mCount = desired_count;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    vertex_elements_type& GLVertexBuffer::format() {
        return mFormat;
    }

    const vertex_elements_type& GLVertexBuffer::format() const {
        return mFormat;
    }

    uint32 GLVertexBuffer::count() const {
        return mCount;
    }

    GLint GLVertexBuffer::getGLBuffer() const {
        return mId;
    }
    
    void GLVertexBuffer::copyBuffer(const GraphicBufferPtr& to) {
        if(to) {
            GLVertexBuffer* dest = checked_cast<GLVertexBuffer*>(to.get());
            if(glCopyBufferSubData) {
                glBindBuffer(GL_COPY_READ_BUFFER, this->getGLBuffer());
                glBindBuffer(GL_COPY_WRITE_BUFFER, dest->getGLBuffer());

                CHECK_GL_CALL(glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                    GL_COPY_WRITE_BUFFER,
                                    0,
                                    0,
                                    this->count() * GetVertexElementsTotalSize(this->format())));

                glBindBuffer(GL_COPY_READ_BUFFER, 0);
                glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            } else {
                log_error(L"GLVertexBuffer::copyBuffer: GL Copy Buffer Sub Data not supported");
            }
        }
    }

    GLIndexBuffer::GLIndexBuffer(GraphicBuffer::Access access,
                                 GraphicBuffer::Usage usage,
                                 uint32 desired_count,
                                 const void* initData):
    GraphicBuffer(access, usage),
    mMaped(false) {
        glGenBuffers(1, &mId);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
        if(initData) {
            CHECK_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(desired_count * sizeof(uint32)),
                         static_cast<const GLvoid*>(initData),
                         usage == Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        } else {
            CHECK_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(desired_count * sizeof(uint32)),
                         0,
                         usage == Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        mCount = desired_count;
    }

    GLIndexBuffer::~GLIndexBuffer() {
        glDeleteBuffers(1, &mId);
    }

    uint32 GLIndexBuffer::count() const {
        return mCount;
    }

    void* GLIndexBuffer::map() {
        if(access() == None) {
            log_error("GLIndexBuffer: trying to map buffer with access = None");
            return 0;
        }
        if(mMaped)
            unmap();
        
        GLenum gl_acess;

        switch (access()) {
            case GraphicBuffer::ReadOnly:
                gl_acess = GL_READ_ONLY;
                break;

            case GraphicBuffer::ReadWrite:
                gl_acess = GL_READ_WRITE;
                break;

            case GraphicBuffer::WriteOnly:
                gl_acess = GL_WRITE_ONLY;
                break;
                
            default:
                break;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
        void* p = CHECK_GL_CALL(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_acess));
        if(p)
            mMaped = true;

        return p;
    }

    void GLIndexBuffer::resize(uint32 desired_count) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);

        CHECK_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(desired_count * sizeof(uint32)),
                     0,
                     usage() == Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
        mCount = desired_count;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void GLIndexBuffer::copyBuffer(const GraphicBufferPtr& to) {
        if(to) {
            GLIndexBuffer* dest = checked_cast<GLIndexBuffer*>(to.get());

            if(glCopyBufferSubData) {
                glBindBuffer(GL_COPY_READ_BUFFER, this->getGLBuffer());
                glBindBuffer(GL_COPY_WRITE_BUFFER, dest->getGLBuffer());

                CHECK_GL_CALL(glCopyBufferSubData(GL_COPY_READ_BUFFER,
                                    GL_COPY_WRITE_BUFFER,
                                    0,
                                    0,
                                    this->count() * sizeof(uint32)));

                glBindBuffer(GL_COPY_READ_BUFFER, 0);
                glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
            } else {
                log_error(L"GLIndexBuffer::copyBuffer: GL Copy Buffer Sub Data not supported");
            }
        }
    }

    GLint GLIndexBuffer::getGLBuffer() const {
        return mId;
    }
    
    void GLIndexBuffer::unmap() {
        if(mMaped) {
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }

    void GLIndexBuffer::activate() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
    }
    
    void GLIndexBuffer::deactivate() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

} // namespace ukn

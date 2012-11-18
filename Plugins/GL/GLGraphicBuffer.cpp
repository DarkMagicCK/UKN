//
//  GLGraphicBufer.cpp
//  Project Unknown
//
//  Created by Robert Bu on 12/7/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "GLGraphicBuffer.h"

#include "GLPreq.h"

namespace ukn {

    GLVertexBuffer::GLVertexBuffer(GraphicBuffer::Access access,
                                   GraphicBuffer::Usage usage,
                                   uint32 desired_count,
                                   const void* initData,
                                   const VertexFormat& format):
    GraphicBuffer(access, usage),
    mFormat(format),
    mMaped(false) {

        glGenBuffers(1, &mId);
        glBindBuffer(GL_ARRAY_BUFFER, mId);

        if(initData) {
            glBufferData(GL_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(desired_count * format.totalSize()),
                         static_cast<const GLvoid*>(initData),
                         usage == Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

        } else {
            glBufferData(GL_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(desired_count * format.totalSize()),
                         0,
                         usage == Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        mCount = desired_count;
    }

    GLVertexBuffer::~GLVertexBuffer() {
        glDeleteBuffers(1, &mId);
    }

    void* GLVertexBuffer::map() {
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
        }

        glBindBuffer(GL_ARRAY_BUFFER, mId);
        void* p = glMapBuffer(GL_ARRAY_BUFFER, gl_acess);
        if(p)
            mMaped = true;
        return p;
    }

    void GLVertexBuffer::unmap() {
        glBindBuffer(GL_ARRAY_BUFFER, mId);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        mMaped = false;
    }

    void GLVertexBuffer::activate() {
        glBindBuffer(GL_ARRAY_BUFFER, mId);
    }
    
    void GLVertexBuffer::deactivate() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    void GLVertexBuffer::resize(uint32 desired_count) {
        glBindBuffer(GL_ARRAY_BUFFER, mId);

        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(desired_count * mFormat.totalSize()),
                     0,
                     usage() == Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        mCount = desired_count;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    VertexFormat& GLVertexBuffer::format() {
        return mFormat;
    }

    const VertexFormat& GLVertexBuffer::format() const {
        return mFormat;
    }

    uint32 GLVertexBuffer::count() const {
        return mCount;
    }
    
    bool GLVertexBuffer::isInMemory() const {
        return false;
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
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(desired_count * sizeof(uint32)),
                         static_cast<const GLvoid*>(initData),
                         usage == Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        } else {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(desired_count * sizeof(uint32)),
                         0,
                         usage == Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
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
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
        void* p = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_acess);
        if(p)
            mMaped = true;
        return p;
    }

    void GLIndexBuffer::resize(uint32 desired_count) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(desired_count * sizeof(uint32)),
                     0,
                     usage() == Dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        mCount = desired_count;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    bool GLIndexBuffer::isInMemory() const {
        return false;
    }
    
    void GLIndexBuffer::unmap() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void GLIndexBuffer::activate() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mId);
    }
    
    void GLIndexBuffer::deactivate() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

} // namespace ukn

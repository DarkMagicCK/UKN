//
//  Camera.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Camera_h
#define Project_Unknown_Camera_h

#include "Platform.h"
#include "MathUtil.h"
#include "Frustum.h"

namespace ukn {
    
    class UKN_API Camera {
    public:
        Camera() { }
        virtual ~Camera() { }
        
        virtual void update() = 0;
        
        // GraphicDevice->setViewMatrix
        virtual const Matrix4& getViewMatrix() const = 0;
        // GraphicDevice->setProjMatrix
        virtual const Matrix4& getProjMatrix() const = 0;
    };
    
    // a 2d ortho camera implementation
    // default ortho box depth from 0 to 1
    class UKN_API Camera2D: public Camera {
    public:
        Camera2D();
        virtual ~Camera2D();
        
        void setTransformParams(float x, float y, float displaceX, float displaceY, float rot, float scaleV, float scaleH);
        void setOrthoParams(float left, float right, float bottom, float top, float near=0.f, float far=1.f);
        
        float getLeft() const;
        float getRight() const;
        float getTop() const;
        float getBottom() const;
        
        float getX() const;
        float getY() const;
        float getDisplacementX() const;
        float getDisplacementY() const;
        float getRotation() const;
        float getScaleV() const;
        float getScaleH() const;
        
        virtual void update();
        virtual const Matrix4& getViewMatrix() const;
        virtual const Matrix4& getProjMatrix() const;
        
    private:
        float mX, mY;
        float mDX, mDY;
        float mRotation;
        float mScaleV, mScaleH;
        
        float mLeft, mRight;
        float mTop, mBottom;
        float mNear, mFar;
        
        Matrix4 mViewMatrix;
        Matrix4 mProjMatrix;
    };
    
    class UKN_API Camera3D: public Camera {
    public:
        Camera3D();
        virtual ~Camera3D();
    
        void setViewParams(const Vector3& eye, const Vector3& lookat, const Vector3& up = Vector3(0, 1, 0));
        void setProjParams(float fov, float aspect, float near, float far);
    
        const Vector3& getEyePos() const;
        const Vector3& getLookAt() const;
        const Vector3& getUpVec() const;
        const Vector3& getViewVec() const;
        
        float getFOV() const;
        float getAspect() const;
        float getNearPlane() const;
        float getFarPlane() const;
        
        const Matrix4& getPrevViewMatrix() const;
        const Matrix4& getPrevProjMatrix() const;
        
        const Frustum& getViewFrustum() const;
        
        virtual void update();
        virtual const Matrix4& getViewMatrix() const;
        virtual const Matrix4& getProjMatrix() const;
        
        // stereo mode?
        
    private:
        Vector3 mEysPos;
        Vector3 mLookAt;
        Vector3 mUpVec;
        Vector3 mViewVec;
        Matrix4 mViewMat[2];
        
        float mFOV;
        float mAspect;
        float mNearPlane;
        float mFarPlane;
        Matrix4 mProjMat[2];
        
        mutable Frustum mFrustum[2];
        mutable bool mFrustumDirty;
    };
    
} // namespace ukn


#endif

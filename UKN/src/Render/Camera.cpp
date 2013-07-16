//
//  Camera.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Camera.h"
#include "UKN/GraphicDevice.h"
#include "UKN/GraphicFactory.h"
#include "UKN/Context.h"

namespace ukn {
   
    Camera::Camera() {
        this->setViewParams(Vector3(0, 0, 0),
                            Vector3(0, 0, 1), 
                            Vector3(0, 1, 0));
        this->setProjParams(pi / 4, 1, 0, 1.0);
    }
    
    Camera::~Camera() {
        
    }
    
    void Camera::setViewParams(const Vector3& eye, const Vector3& lookat, const Vector3& up) {
        mEysPos = eye;
        mLookAt = lookat;
        mUpVec = up;
        
        mViewVec = (mLookAt - mEysPos).normalize();
        mViewMat[1] = mViewMat[0];
        mViewMat[0] = Matrix4::LookAtMatLH(eye, lookat, up);
        mFrustumDirty = true;
    }
    
    void Camera::setProjParams(float fov, float aspect, float nearx, float farx) {
        mFOV = fov;
        mAspect = aspect;
        mNearPlane = nearx;
        mFarPlane = farx;
        
        mProjMat[1] = mProjMat[0];
        mProjMat[0] = Matrix4::PerspectiveFovMatLH(fov, aspect, nearx, farx);
        mFrustumDirty = true;
        
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.adjustPerspectiveMat(mProjMat[0]);
    }

    void Camera::setProjParamsOrtho(float l, float r, float b, float t, float n, float f) {
        mNearPlane = n;
        mFarPlane = f;
        mFOV = pi_2;
        mAspect = (r - l) / (t - b);

        mProjMat[1] = mProjMat[0];
        mProjMat[0] = Matrix4::OrthoOffCenterMatLH(l, r, b, t, n, f);
        mFrustumDirty = true;

        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        gd.adjustPerspectiveMat(mProjMat[0]);
    }

    void Camera::setViewMat(const Matrix4& mat) {
        mViewMat[1] = mViewMat[0];
        mViewMat[0] = mat;
    }

    void Camera::setProjMat(const Matrix4& mat) {
        mProjMat[1] = mProjMat[0];
        mProjMat[0] = mat;

    }
    
    Vector3 Camera::getEyePos() const {
        return mEysPos;
    }
    
    Vector3 Camera::getLookAt() const {
        return mLookAt;
    }
    
    Vector3 Camera::getUpVec() const {
        return mUpVec;
    }
    
    Vector3 Camera::getViewVec() const {
        return mViewVec;
    }
    
    float Camera::getFOV() const {
        return mFOV;
    }
    
    float Camera::getAspect() const {
        return mAspect;
    }
    
    float Camera::getNearPlane() const {
        return mNearPlane;
    }
    
    float Camera::getFarPlane() const {
        return mFarPlane;
    }
    
    const Matrix4& Camera::getPrevViewMatrix() const {
        return mViewMat[1];
    }
    
    const Matrix4& Camera::getPrevProjMatrix() const {
        return mProjMat[1];
    }
    
    const Frustum& Camera::getViewFrustum() const {
        if(mFrustumDirty) {
            mFrustum[0].clipMatrix(mViewMat[0] * mProjMat[0]);
            mFrustum[1].clipMatrix(mViewMat[1] * mProjMat[1]);
            mFrustumDirty = false;
        }
        return mFrustum[0];
    }
    
    void Camera::update() {
    }
    
    const Matrix4& Camera::getViewMatrix() const {
        return mViewMat[0];
    }
    
    const Matrix4& Camera::getProjMatrix() const {
        return mProjMat[0];
    }
    
} // namespace ukn
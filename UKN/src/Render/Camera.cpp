//
//  Camera.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Camera.h"
#include "UKN/GraphicDevice.h"

namespace ukn {
    
    Camera2D::Camera2D():
    mX(0.f),
    mY(0.f),
    mDX(0.f),
    mDY(0.f),
    mRotation(0.f),
    mScaleV(1.f),
    mScaleH(1.f),
    mLeft(0.f),
    mRight(0.f),
    mTop(0.f),
    mBottom(0.f),
    mNear(0.f),
    mFar(1.f){
        
    }
    
    Camera2D::~Camera2D() {
        
    }
    
    void Camera2D::setTransformParams(float x, float y, float displaceX, float displaceY, float rot, float scaleV, float scaleH) {
        mX = x;
        mY = y;
        mDX = displaceX;
        mDY = displaceY;
        mRotation = rot;
        mScaleV = scaleV;
        mScaleH = scaleH;
    }
    
    void Camera2D::setOrthoParams(float left, float right, float bottom, float top, float nearx, float farx) {
        mLeft = left;
        mRight = right;
        mTop = top;
        mBottom = bottom;
        mNear = nearx;
        mFar = farx;
    }
    
    float Camera2D::getLeft() const {
        return mLeft;
    }
    
    float Camera2D::getRight() const {
        return mRight;
    }
    
    float Camera2D::getTop() const {
        return mTop;
    }
    
    float Camera2D::getBottom() const {
        return mBottom;
    }
    
    float Camera2D::getX() const {
        return mX;
    }
    
    float Camera2D::getY() const {
        return mY;
    }
    
    float Camera2D::getDisplacementX() const {
        return mDX;
    }
    
    float Camera2D::getDisplacementY() const {
        return mDY;
    }
    
    float Camera2D::getRotation() const {
        return mRotation;
    }
    
    float Camera2D::getScaleV() const {
        return mScaleV;
    }
    
    float Camera2D::getScaleH() const {
        return mScaleH;
    }
    
    void Camera2D::update() {
        mViewMatrix = Matrix4();
        
        // transform follows rotate -> scale -> displacement
        mViewMatrix.translate(-mX, -mY, 0.f)
                    .rotate(0.f, 0.f, mRotation)
                    .scale(mScaleV, mScaleH, 0.f)
                    .translate(mX + mDX, mY + mDY, 0.f);
        
        mProjMatrix = Matrix4::OrthoMat(mLeft, mRight, mBottom, mTop, mNear, mFar);
    }
    
    const Matrix4& Camera2D::getViewMatrix() const {
        return mViewMatrix;
    }
    
    const Matrix4& Camera2D::getProjMatrix() const {
        return mProjMatrix;
    }
    
    Camera3D::Camera3D() {
        this->setViewParams(Vector3(0, 0, 0),
                            Vector3(0, 0, 1), 
                            Vector3(0, 1, 0));
        this->setProjParams(pi / 4, 1, 1, 1000);
    }
    
    Camera3D::~Camera3D() {
        
    }
    
    void Camera3D::setViewParams(const Vector3& eye, const Vector3& lookat, const Vector3& up) {
        mEysPos = eye;
        mLookAt = lookat;
        mUpVec = up;
        
        mViewVec = (mLookAt - mEysPos).normalize();
        mViewMat[0] = look_at_mat_lh(eye, lookat, up);
        mFrustumDirty = true;
    }
    
    void Camera3D::setProjParams(float fov, float aspect, float nearx, float farx) {
        mFOV = fov;
        mAspect = aspect;
        mNearPlane = nearx;
        mFarPlane = farx;
        
        mProjMat[0] = Matrix4::PerspectiveMat(fov, aspect, nearx, farx);
        mFrustumDirty = true;
    }
    
    const Vector3& Camera3D::getEyePos() const {
        return mEysPos;
    }
    
    const Vector3& Camera3D::getLookAt() const {
        return mLookAt;
    }
    
    const Vector3& Camera3D::getUpVec() const {
        return mUpVec;
    }
    
    const Vector3& Camera3D::getViewVec() const {
        return mViewVec;
    }
    
    float Camera3D::getFOV() const {
        return mFOV;
    }
    
    float Camera3D::getAspect() const {
        return mAspect;
    }
    
    float Camera3D::getNearPlane() const {
        return mNearPlane;
    }
    
    float Camera3D::getFarPlane() const {
        return mFarPlane;
    }
    
    const Matrix4& Camera3D::getPrevViewMatrix() const {
        return mViewMat[0];
    }
    
    const Matrix4& Camera3D::getPrevProjMatrix() const {
        return mProjMat[0];
    }
    
    const Frustum& Camera3D::getViewFrustum() const {
        if(mFrustumDirty) {
            mFrustum[0].clipMatrix(mViewMat[0] * mProjMat[0]);
            mFrustum[1].clipMatrix(mViewMat[1] * mProjMat[1]);
            mFrustumDirty = false;
        }
        return mFrustum[0];
    }
    
    void Camera3D::update() {
        
    }
    
    const Matrix4& Camera3D::getViewMatrix() const {
        return mViewMat[0];
    }
    
    const Matrix4& Camera3D::getProjMatrix() const {
        return mProjMat[0];
    }
    
} // namespace ukn
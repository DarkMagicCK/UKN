//
//  Camera.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Camera_h
#define Project_Unknown_Camera_h

#include "mist/Platform.h"
#include "mist/MathUtil.h"
#include "UKN/Frustum.h"

namespace ukn {
    
    class UKN_API Camera {
    public:
        Camera();
        virtual ~Camera();
            
        void setViewParams(const Vector3& eye, const Vector3& lookat, const Vector3& up = Vector3(0, 1, 0));
        void setProjParams(float fov, float aspect, float near, float far);
    
        Vector3 getEyePos() const;
        Vector3 getLookAt() const;
        Vector3 getUpVec() const;
        Vector3 getViewVec() const;
        
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

//
//  Frustum.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Frustum.h"

#include "mist/Preprocessor.h"

namespace ukn {
    
    void Frustum::clipMatrix(const Matrix4& clip) {
        Vector4 col1 = math::get_matrix_col(clip, 0);
        Vector4 col2 = math::get_matrix_col(clip, 1);
        Vector4 col3 = math::get_matrix_col(clip, 2);
        Vector4 col4 = math::get_matrix_col(clip, 3);
        
        this->mPlanes[0] = col4 - col1;
        this->mPlanes[1] = col4 + col1;
        this->mPlanes[2] = col4 - col2;
        this->mPlanes[3] = col4 + col2;
        this->mPlanes[4] = col4 - col3;
        this->mPlanes[5] = col4 + col3;
        
        MIST_FOR_EACH(Plane& plane, this->mPlanes) {
            plane.set(plane.get().normalize());
        }
        
        for(int i=0; i<6; ++i) {
            this->mLookupTable[i] = ((this->mPlanes[i].a() < 0) ? 1 : 0) | 
                                    ((this->mPlanes[i].b() < 0) ? 2 : 0) |
                                    ((this->mPlanes[i].c() < 0) ? 4 : 0);
        }
    }

    Frustum::Visibility Frustum::isCubeVisible(const Vector3& pos, float r) const {
        for(int i=0; i<6; ++i) {
            if(mPlanes[i].dot(Vector3(pos - r)) >= 0)
                continue;
            if(mPlanes[i].dot(Vector3(pos + r)) >= 0)
                continue;
            if(mPlanes[i].dot(Vector3(pos.x() + r, pos.y() - r, pos.z() - r)) >= 0)
                continue;
            if(mPlanes[i].dot(Vector3(pos.x() - r, pos.y() + r, pos.z() - r)) >= 0)
                continue;
            if(mPlanes[i].dot(Vector3(pos.x() + r, pos.y() + r, pos.z() - r)) >= 0)
                continue;
            if(mPlanes[i].dot(Vector3(pos.x() - r, pos.y() - r, pos.z() + r)) >= 0)
                continue;
            if(mPlanes[i].dot(Vector3(pos.x() - r, pos.y() + r, pos.z() + r)) >= 0)
                continue;
            if(mPlanes[i].dot(Vector3(pos.x() + r, pos.y() - r, pos.z() + r)) >= 0)
                continue;
            return Visibility::No;
        }
        return Visibility::Yes;
    }
    
    Frustum::Visibility Frustum::isBoxVisible(const Box& box) const {
        bool intersect = false;
        for(int i=0; i<6; ++i) {
            const int n = mLookupTable[i];
            
            Vector3 v0((n & 1) ? box.getMin().x() : box.getMax().x(), 
                       (n & 2) ? box.getMin().y() : box.getMax().y(),
                       (n & 4) ? box.getMin().z() : box.getMax().z());
            Vector3 v1((n & 1) ? box.getMax().x() : box.getMin().x(), 
                       (n & 2) ? box.getMax().y() : box.getMin().y(),
                       (n & 4) ? box.getMax().z() : box.getMin().z());
            
            if(this->mPlanes[i].dot(v0) < 0)
                return No;
            if(this->mPlanes[i].dot(v1) < 0)
                intersect = true;
        }
        
        return intersect ? Part : Yes;
    }

    Frustum::Visibility Frustum::isSphereVisible(const Sphere& sphere) const {
        for(int i=0; i<6; ++i) {
            if((mPlanes[i]).dot(sphere.center()) < -sphere.radius()) {
                return Visibility::No;
            }
        }
        return Visibility::Yes;

    }
    
} // namespace ukn

//
//  Frustum.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Frustum.h"
#include "UKN/Preprocessor.h"

namespace ukn {
    
    void Frustum::clipMatrix(const Matrix4& clip) {
        Vector4 col1 = get_matrix_col(clip, 0);
        Vector4 col2 = get_matrix_col(clip, 1);
        Vector4 col3 = get_matrix_col(clip, 2);
        Vector4 col4 = get_matrix_col(clip, 3);
        
        this->mPlanes[0] = col4 - col1;
        this->mPlanes[1] = col4 + col1;
        this->mPlanes[2] = col4 - col2;
        this->mPlanes[3] = col4 + col2;
        this->mPlanes[4] = col4 - col3;
        this->mPlanes[5] = col4 + col3;
        
        UKN_FOR_EACH(Plane& plane, this->mPlanes) {
            plane = static_cast<Vector4>(plane).normalize();
        }
        
        for(int i=0; i<6; ++i) {
            this->mLookupTable[i] = ((this->mPlanes[i].a() < 0) ? 1 : 0) | 
                                    ((this->mPlanes[i].a() < 0) ? 2 : 0) |
                                    ((this->mPlanes[i].a() < 0) ? 4 : 0);
        }
    }
    
    Frustum::Visiabilty Frustum::isVisiable(const Box& box) const {
        bool intersect = false;
        for(int i=0; i<6; ++i) {
            const int n = mLookupTable[i];
            
            Vector3 v0((n & 1) ? box.getMin().x : box.getMax().x, 
                       (n & 2) ? box.getMin().y : box.getMax().y,
                       (n & 4) ? box.getMin().z : box.getMax().z);
            Vector3 v1((n & 1) ? box.getMax().x : box.getMin().x, 
                       (n & 2) ? box.getMax().y : box.getMin().y,
                       (n & 4) ? box.getMax().z : box.getMin().z);
            
            if(this->mPlanes[i].normal().dot(v1) < 0)
                return No;
            if(this->mPlanes[i].normal().dot(v1) < 0)
                intersect = true;
        }
        
        return intersect ? Part : Yes;
    }
    
} // namespace ukn

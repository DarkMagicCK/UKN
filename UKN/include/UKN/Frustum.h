//
//  Frustum.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Frustum_h
#define Project_Unknown_Frustum_h

#include "UKN/PreDeclare.h"

namespace ukn {
    
    class UKN_API Frustum {
    public:
        enum Visibility {
            Yes,
            No,
            Part,
        };
        
    public:
        void clipMatrix(const Matrix4& clip);

        Visibility isBoxVisible(const Box& box) const;
        Visibility isSphereVisible(const Sphere& sphere) const;
        
    private:
        FixedArray<Plane, 6> mPlanes;
        FixedArray<int, 6> mLookupTable;
    };
    
} // namespace ukn

#endif

//
//  Frustum.h
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_Frustum_h
#define Project_Unknown_Frustum_h

#include "UKN/Platform.h"
#include "UKN/Util.h"
#include "UKN/MathUtil.h"

namespace ukn {
    
    class UKN_API Frustum {
    public:
        enum Visiabilty {
            Yes,
            No,
            Part,
        };
        
    public:
        void clipMatrix(const Matrix4& clip);
        Visiabilty isVisiable(const Box& box) const;
        
    private:
        FixedArray<Plane, 6> mPlanes;
        FixedArray<int, 6> mLookupTable;
    };
    
} // namespace ukn

#endif

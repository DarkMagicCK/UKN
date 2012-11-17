//
//  Transform.h
//  Mist
//
//  Created by Robert Bu on 12/10/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTTransform_h
#define MISTTransform_h

#include "mist/Platform.h"
#include "mist/MathUtil.h"

namespace mist  {
    
    /**
     * base transform class
     **/
    class Transform {
    public:
        Transform() {
            
        }
        
        Matrix4& getTransformMatrix() {
            if(mDirty)
                update();
            return mTransformMatrix;
        }
        
        void update() {
            mTransformMatrix = SoraMatrix4::RotMat(mRotation.x, 
                                                   mRotation.y, 
                                                   mRotation.z).scale(mScale.x, 
                                                                      mScale.y, 
                                                                      mScale.z).translate(mPosition.x, 
                                                                                          mPosition.y, 
                                                                                          mPosition.z);
            mDirty = false;
        }
        
        const Vector3&     position() const    { return mPosition; }
        const Vector3&     scale() const       { return mScale; }
        const Quaternion&  rotation() const    { return mRotation; }
        
        Vector3&     position()    { return mPosition;  }
        Vector3&     scale()       { return mScale;  }
        Quaternion&  rotation()    { return mRotation;  }
        
        void position(const Vector3& rhs)               { mPosition = rhs; mDirty = true;  };
        void scale(const Vector3& rhs)                  { mScale = scale; mDirty = true;  }
        void rotation(const Quaternion& rhs)            { mRotation = rhs; mDirty = true;  }
        void rotation(const Vector3& axis, real angle)  { mRotation = Quaternion(axis, angle); mDirty = true;  }
        void rotation(real x, real y, real z)           { mRotation = Quaternion(x, y, z, 0); mDirty = true;  }
        
    private:
        Vector3 mPosition;
        Vector3 mScale;
        Quaternion mRotation;
        
        Matrix4 mTransformMatrix;
        bool mDirty;
    };
    
} // namespace mist

#endif

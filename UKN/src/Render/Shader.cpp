//
//  Shader.cpp
//  Project Unknown
//
//  Created by Robert Bu on 9/5/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/Shader.h"

namespace ukn {

    class NullEffect: public Effect {
    public:
        void bind() { }
        void unbind() { }
        
        bool setMatrixVariable(const char* name, const Matrix4& worldMat) { return true; }
		bool setFloatVectorVariable(const char* name, float* vec) { return true; }
		bool setIntVectorVariable(const char* name, int* vec) { return true; }
		bool setTextureVariable(const char* name, Texture* resource) { return true; }
		bool getMatrixVariable(const char* name, Matrix4* mat) { return true; }
		bool getFloatVectorVariable(const char* name, float* vec) { return true; }
		bool getIntVectorVariable(const char* name, int* vec) { return true; }

		uint32 getPasses() const { return 1; }
		void applyPass(uint32 pass) {  }
    };

    EffectPtr Effect::NullObject() {
        return SharedPtr<Effect>(new NullEffect());
    }

}
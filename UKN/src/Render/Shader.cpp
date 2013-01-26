//
//  Shader.cpp
//  Project Unknown
//
//  Created by Robert Bu on 9/5/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "UKN/Shader.h"
#include "UKN/Context.h"

namespace ukn {

    Effect::Effect() {

    }

    Effect::~Effect() {
    
    }

    void Effect::setFragmentShader(const ShaderPtr& shader) {
        mFragmentShader = shader;
    }

    void Effect::setVertexShader(const ShaderPtr& shader) {
        mVertexShader = shader;
    }

    void Effect::setGeometryShader(const ShaderPtr& shader) {
        mGeometryShader = shader;
    }

    ShaderPtr Effect::getFragmentShader() const {
        return mFragmentShader;
    }

    ShaderPtr Effect::getVertexShader() const {
        return mVertexShader;
    }

    ShaderPtr Effect::getGeometryShader() const {
        return mGeometryShader;
    }

}
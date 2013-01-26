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

    bool Effect::loadEffectFile(const ResourcePtr& content) {
        return false;
    }

    void Effect::emitCode() {
        // null
    }

    uint32 Effect::getPasses() const {
        return 1;
    }

    void Effect::bind(uint32 pass) {
        if(mVertexShader)
            mVertexShader->bind();
        if(mFragmentShader)
            mFragmentShader->bind();

        this->onBind(pass);
    }

    void Effect::unbind() {

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

    void Effect::onBind(uint32 pass) {

    }

    void Effect::onUnbind() {

    }

}
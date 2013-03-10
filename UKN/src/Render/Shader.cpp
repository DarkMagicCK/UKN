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

    EffectPass::EffectPass(EffectTechnique* parent):
        mParent(parent) {

    }

    EffectPass::~EffectPass() {
    
    }
        
    void EffectPass::setFragmentShader(const ShaderPtr& shader) {
        mFragmentShader = shader;
    }

    void EffectPass::setVertexShader(const ShaderPtr& shader) {
        mVertexShader = shader;
    }

    void EffectPass::setGeometryShader(const ShaderPtr& shader) {
        mGeometryShader = shader;
    }

    const ShaderPtr& EffectPass::getFragmentShader() const {
        return mFragmentShader;
    }

    const ShaderPtr& EffectPass::getVertexShader() const {
        return mVertexShader;
    }

    const ShaderPtr& EffectPass::getGeometryShader() const {
        return mGeometryShader;
    }

    void EffectPass::begin() {
        if(mVertexShader)
            mVertexShader->bind();
        if(mFragmentShader)
            mFragmentShader->bind();
        if(mGeometryShader)
            mGeometryShader->bind();
    }

    void EffectPass::end() {
        if(mVertexShader)
            mVertexShader->unbind();
        if(mFragmentShader)
            mFragmentShader->unbind();
        if(mGeometryShader)
            mGeometryShader->unbind();
    }

    Effect::Effect() {

    }

    Effect::~Effect() {
    
    }

    EffectTechniquePtr Effect::createTechnique() {
        return MakeSharedPtr<EffectTechnique>(this);
    }

    const EffectTechniquePtr& Effect::appendTechnique() {
        mTechniques.push_back(this->createTechnique());
        return mTechniques.back();
    }

    const EffectTechniquePtr& Effect::appendTechnique(const ShaderPtr& fragment, const ShaderPtr& vertex, const ShaderPtr& geometry) {
        const EffectTechniquePtr& t = this->appendTechnique();
        t->appendPass(fragment, vertex, geometry);
        return t;
    }

    const EffectTechniquePtr& Effect::getTechnique(uint32 index) const {
        if(index < mTechniques.size()) 
            return mTechniques[index];
        return StaticEmptySharedPtr<EffectTechnique>();
    }

    uint32 Effect::getNumTechniques() const {
        return mTechniques.size();
    }

    EffectTechnique::EffectTechnique(Effect* parent):
    mParent(parent) {

    }

    EffectTechnique::~EffectTechnique() {

    }
    
    const EffectPassPtr& EffectTechnique::appendPass(const ShaderPtr& fragmentShader, const ShaderPtr& vertexShader, const ShaderPtr& geometryShader) {
        const EffectPassPtr& pass = this->appendPass();
        pass->setFragmentShader(fragmentShader);
        pass->setVertexShader(vertexShader);
        pass->setGeometryShader(geometryShader);
        return pass;
    }

    uint32 EffectTechnique::getNumPasses() const {
        return uint32(mPasses.size());
    }

    void EffectTechnique::appendPass(const EffectPassPtr& pass) {
        mPasses.push_back(pass);
    }

    EffectPassPtr EffectTechnique::createPass() {
        return MakeSharedPtr<EffectPass>(this);
    }

    const EffectPassPtr& EffectTechnique::getPass(uint32 pass) const {
        if(pass < mPasses.size())
            return mPasses.at(pass);
        else
            log_error(format_string("Effect::getPass, pass %d overflows", pass));
        return StaticEmptySharedPtr<EffectPass>();
    }

    const EffectPassPtr& EffectTechnique::appendPass() {
        this->appendPass(this->createPass());
        return mPasses.back();
    }

    void EffectTechnique::bind(uint32 pass) {
        if(pass < mPasses.size()) {
            mPasses[pass]->begin();
        } else {
            log_error(L"EffectTechnique::bind: invalid pass index");
        }
    }

    void EffectTechnique::unbind(uint32 pass) {
        if(pass < mPasses.size()) {
            mPasses[pass]->end();
        } else {
            log_error(L"EffectTechnique::bind: invalid pass index");
        }
    }

    bool Shader::setFloatVectorVariable(const char* name, const float3& vec) {
        return this->setFloatVariable(name, 3, vec.value);
    }

    bool Shader::setFloatVectorVariable(const char* name, const float2& vec) {
        return this->setFloatVariable(name, 2, vec.value);
    }
    
    bool Shader::setFloatVariable(const char* name, float v) {
        float tv = v;
        return this->setFloatVariable(name, 1, &tv);
    }

    bool Shader::setIntVectorVariable(const char* name, const int3& vec) {
        return this->setIntVariable(name, 3, vec.value);
    }

    bool Shader::setIntVectorVariable(const char* name, const int2& vec) {
        return this->setIntVariable(name, 2, vec.value);
    }

    bool Shader::setIntVariable(const char* name, int32 v) {
        int32 tv = v;
        return this->setIntVariable(name, 1, &tv);
    }
}
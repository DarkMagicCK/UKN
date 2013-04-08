#include "UKN/Renderable.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/Context.h"
#include "UKN/Texture.h"

namespace ukn {

    Renderable::Renderable() {

    }

    Renderable::~Renderable() {

    }

    const Matrix4& Renderable::getModelMatrix() const {
        return mModelMatrix;
    }

    void Renderable::setModelMatrix(const Matrix4& mat) {
        mModelMatrix = mat;
    }

    const MaterialPtr& Renderable::getMaterial() const {
        return mMaterial;
    }

    void Renderable::render(const EffectTechniquePtr& technique) {
       GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();

       RenderBufferPtr& gb = this->getRenderBuffer();

       this->onRenderBegin();
       gd.renderBuffer(technique, gb);
       this->onRenderEnd();
    }

    void Renderable::onRenderBegin() {

    }

    void Renderable::onRenderEnd() {
    
    }

    const TexturePtr& Renderable::getDiffuseTex() const {
        return mDiffuseTex;
    }

    const TexturePtr& Renderable::getSpecularTex() const {
        return mSpecularTex;
    }

    const TexturePtr& Renderable::getNormalTex() const {
        return mNormalTex;
    }

    const TexturePtr& Renderable::getHeightTex() const {
        return mHeightTex;
    }

    const TexturePtr& Renderable::getEmitTex() const {
        return mEmitTex;
    }

}
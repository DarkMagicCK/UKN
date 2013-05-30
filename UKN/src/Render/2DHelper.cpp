#include "UKN/2DHelper.h"
#include "UKN/CgHelper.h"
#include "UKN/Shader.h"
#include "UKN/Context.h"
#include "UKN/GraphicFactory.h"
#include "UKN/GraphicDevice.h"
#include "UKN/FrameBuffer.h"
#include "UKN/DepthStencilStateObject.h"

namespace ukn {

    Ukn2DHelper::Ukn2DHelper() {

    }

    Ukn2DHelper::~Ukn2DHelper() {

    }
    
    void Ukn2DHelper::initialize() {
        if(!mEffect) {
            mEffect = GetCgEffect2D();
            if(!mEffect)
                log_error("Ukn2DHelper::initialize: error creating effect for 2d rendering");
        }
    }

    void Ukn2DHelper::setupMat() {
        this->setupMat(mScreenTransformMat);
    }

    void Ukn2DHelper::setupMat(const mist::Matrix4& viewMat) {
        GraphicDevice& device = Context::Instance().getGraphicFactory().getGraphicDevice();
        mScreenOrthoMat = Matrix4::OrthoOffCenterMatLH(0.f,
                                                       device.getCurrFrameBuffer()->getViewport().width,
                                                       device.getCurrFrameBuffer()->getViewport().height,
                                                       0.f,
                                                       0.f,
                                                       1.0f);
        device.adjustOrthoMat(mScreenOrthoMat);

        this->setupMat(mScreenOrthoMat, viewMat);
    }

    void Ukn2DHelper::setupMat(const mist::Matrix4& orthoMat, const mist::Matrix4& viewMat) {
         if(!mEffect)
            this->initialize();
        
        if(mEffect) {
            EffectPassPtr pass0 = mEffect->getTechnique(0)->getPass(0);
            pass0->getVertexShader()->setMatrixVariable("projectionMatrix", orthoMat);
            pass0->getVertexShader()->setMatrixVariable("viewMatrix", viewMat);
        }
    }

    void Ukn2DHelper::bindTexture(const TexturePtr& texture) {
        if(mEffect) {
            mEffect->getTechnique(0)->getPass(0)->getFragmentShader()->setTextureVariable("tex", texture);
        }
    }

    const EffectPtr& Ukn2DHelper::getEffect() const {
        return mEffect;
    }
    

} // namespace ukn
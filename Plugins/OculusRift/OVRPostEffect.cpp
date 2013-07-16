#include "OVRPostEffect.h"
#include "OVRDevice.h"
#include "OVRDistortion.h"

#include "UKN/GraphicDevice.h"
#include "UKN/Context.h"
#include "UKN/RenderTarget.h"
#include "UKN/SpriteBatch.h"

namespace ukn {

    OVRPostEffect::OVRPostEffect() {

    }

    OVRPostEffect::~OVRPostEffect() {

    }

    void OVRPostEffect::render(const TexturePtr& color,
                               const TexturePtr& normap,
                               const TexturePtr& depth) {
        EffectTechniquePtr tech = mEffect->getTechnique(0);
        ShaderPtr fragment = tech->getPass(0)->getFragmentShader();

        OVRDevice& dev = OVRDevice::Instance();
        OVR::Util::Render::StereoConfig& cfg = dev.getStereoConfig();
        fragment->setFloatVectorVariable(
            "HmdWrapParam", 
            ukn::float4(cfg.GetDistortionK(0),
                        cfg.GetDistortionK(1),
                        cfg.GetDistortionK(2),
                        cfg.GetDistortionK(3))
       //      ukn::float4(1.0, 1.0, 1.0, 1.0)
        );
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        const CameraPtr& cam = gd.getCurrFrameBuffer()->getViewport().camera;
        const Viewport& vp = gd.getViewport();
        Viewport svp = vp;
        svp.width *= dev.getDistortionScale();
        svp.height *= dev.getDistortionScale();
        svp.top *= dev.getDistortionScale();
        svp.left *= dev.getDistortionScale();
   //    gd.setViewport(svp);

        Window& wnd = Context::Instance().getApp().getWindow();
        float w = (float)vp.width / wnd.width();
        float h = (float)vp.height / wnd.height();
        float x = (float)vp.left / wnd.width();
        float y = (float)vp.top / wnd.height();

        float as = (float)vp.width / vp.height;
        fragment->setFloatVectorVariable(
            "lensCenter", 
            float2(x + (w + cfg.GetDistortionConfig().XCenterOffset * 0.5f) * 0.5f,
                   y + h * 0.5f)
        );
        fragment->setFloatVectorVariable(
            "screenCenter",
            float2(x + w * 0.5f,
                   y + h * 0.5f)
        );
        float scaleFactor = 1.0f / cfg.GetDistortionConfig().Scale;
        fragment->setFloatVectorVariable(
            "scale",
            float2((w / 2.f) * scaleFactor,
                   (h / 2.f) * scaleFactor * as)
        );
        fragment->setFloatVectorVariable(
            "scaleIn",
            float2(2.f / w,
                   2.f / h / as)
        );
        fragment->setTextureVariable("tex", color);

        ShaderPtr vertex = tech->getPass(0)->getVertexShader();
        vertex->setMatrixVariable("viewMat",
            Matrix4(2, 0, 0, -1,
                    0, 2, 0, -1,
                    0, 0, 0, 0,
                    0, 0, 0, 1)
        );
        vertex->setMatrixVariable("texMat",
            Matrix4(w, 0, 0, x,
                    0, h, 0, y,
                    0, 0, 0, 0,
                    0, 0, 0, 1)
        );

        
        mRT->attachCamera(cam);
        mRT->attachToRender();

        gd.setDepthStencilState(DepthStencilStateObject::None());
        gd.setBlendState(BlendStateObject::BlendOff());

        gd.clear(CM_Color, color::Black, 1.0f, 0);
        
        SpriteBatch::DefaultObject().drawQuad(tech, Vector2(0, 1), Vector2(1, 0));
        
        mRT->detachFromRender();
    }

    bool OVRPostEffect::init(float2 size) {
        mRT = MakeSharedPtr<CompositeRenderTarget>();
        
        mTarget = MakeSharedPtr<ukn::RenderTarget>((uint32)size[0],
                                                    (uint32)size[1],
                                                    1,
                                                    ukn::EF_RGBA8);
        mRT->attach(ATT_Color0, mTarget);

        mEffect = GetOVRDistortionEffect();
        return true;
    }
        
    TexturePtr OVRPostEffect::getFinalTexture() const {
        return mTarget->getTexture();
    }

}
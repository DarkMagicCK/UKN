#include "UKN/Fog.h"
#include "UKN/Deferred.h"
#include "UKN/RenderTarget.h"
#include "UKN/Context.h"
#include "UKN/GraphicDevice.h"
#include "UKN/SpriteBatch.h"

namespace ukn {

    Fog::Fog():
    mColor(color::Lightgrey),
    mDensity(0.01f),
    mStart(0.f), mEnd(100.f),
    mType(Linear) {

    }

    Fog::~Fog() {

    }

    void Fog::render(const TexturePtr& color,
                    const TexturePtr& normal,
                    const TexturePtr& depth) {
        GraphicDevice& gd = Context::Instance().getGraphicFactory().getGraphicDevice();
        
        const CameraPtr& cam = gd.getCurrFrameBuffer()->getViewport().camera;

        mRT->attachCamera(cam);
        mRT->attachToRender();
        gd.clear(CM_Color | CM_Depth, color::White, 1.0f, 0);

        const ShaderPtr& fragmentShader = mFogTechnique->getPass(0)->getFragmentShader();
        const ShaderPtr& vertexShader = mFogTechnique->getPass(0)->getVertexShader();
            
        fragmentShader->setTextureVariable("colorMap", color);
        fragmentShader->setTextureVariable("depthMap", depth);

        fragmentShader->setMatrixVariable("inverseViewProj", (cam->getViewMatrix() * cam->getProjMatrix()).inverted());

        fragmentShader->setFloatVectorVariable("cameraPosition", cam->getEyePos());
        fragmentShader->setFloatVariable("fogStart", mStart);
        fragmentShader->setFloatVariable("fogEnd", mEnd);
        fragmentShader->setFloatVariable("fogDensity", mDensity);
        fragmentShader->setFloatVariable("fogType", mType);
        fragmentShader->setFloatVectorVariable("fogColor", mColor.getRGB());

        SpriteBatch::DefaultObject().drawQuad(mFogTechnique, Vector2(-1, 1), Vector2(1, -1));

        mRT->detachFromRender();
    }

    bool Fog::init(float2 size) {
        mRT = MakeSharedPtr<CompositeRenderTarget>();
        
        mFogTarget = MakeSharedPtr<ukn::RenderTarget>((uint32)size[0],
                                                      (uint32)size[1],
                                                      1,
                                                      ukn::EF_RGBA64);
        mRT->attach(ATT_Color0, mFogTarget);

        ukn::ShaderPtr vertexShader;
        ukn::ShaderPtr fragmentShader;
            
        GraphicFactory& gf = Context::Instance().getGraphicFactory();
        mEffect = gf.createEffect();

        if(mEffect) {
            vertexShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/fog_vert.cg"), 
                                                    VERTEX_SHADER_DESC("VertexProgram"));
            fragmentShader = mEffect->createShader(MIST_LOAD_RESOURCE(L"deferred/fog_frag.cg"), 
                                                    FRAGMENT_SHADER_DESC("FragmentProgram"));
            mFogTechnique = mEffect->appendTechnique(fragmentShader, vertexShader, ShaderPtr());
        }

        return true;
    }

    TexturePtr Fog::getFinalTexture() const {
        return mFogTarget->getTexture();
    }

    float Fog::getStart() const {
        return mStart;
    }

    float Fog::getEnd() const {
        return mEnd;
    }

    float Fog::getDensity() const {
        return mDensity;
    }

    Fog::FogType Fog::getType() const {
        return mType;
    }

    Color Fog::getColor() const {
        return mColor;
    }

    void Fog::setStart(float start) {
        mStart = start;
    }

    void Fog::setEnd(float end) {
        mEnd = end;
    }

    void Fog::setDensity(float density) {
        mDensity = density;
    }

    void Fog::setType(FogType type) {
        mType = type;
    }

    void Fog::setColor(const Color& color) {
       mColor = color;
    }


}
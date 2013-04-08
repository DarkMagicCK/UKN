#include "UKN/Fog.h"
#include "UKN/Deferred.h"
#include "UKN/RenderTarget.h"

namespace ukn {

    Fog::Fog():
    mColor(color::Transparent),
    mStart(0), mEnd(0),
    mType(Exponential) {

    }

    Fog::~Fog() {

    }

    void Fog::render(const TexturePtr& color,
                    const TexturePtr& normap,
                    const TexturePtr& depth,
                    const TexturePtr& target) {

    }

    bool Fog::init(float2 size) {
        return false;
    }

    const TexturePtr& Fog::getFinalTexture() const {
        return mTarget->getTexture();
    }


}
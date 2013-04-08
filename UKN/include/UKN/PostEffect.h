#ifndef UKN_POST_EFFECT_H_
#define UKN_POST_EFFECT_H_

#include "UKN/PreDeclare.h"

#include "mist/Factory.h"

namespace ukn {

    /* PostEffect base class for Deferred Renderer
        such as SSAO, Fog, HDR ... */
    class UKN_API PostEffect: Uncopyable {
    public:
        PostEffect();
        virtual ~PostEffect();

    public:
        virtual void render(const TexturePtr& color,
                            const TexturePtr& normap,
                            const TexturePtr& depth,
                            const TexturePtr& target) = 0;
        virtual bool init(float2 size) = 0;
        
        virtual const TexturePtr& getFinalTexture() const = 0;
    };

} // namespace ukn


#endif // UKN_POST_EFFECT_H_
#ifndef UKN_2D_HELPER_H_
#define UKN_2D_HELPER_H_

#include "mist/Singleton.h"
#include "mist/MathUtil.h"

#include "UKN/PreDeclare.h"

namespace ukn {

    /* 2d rendering utility class
       sets up a default 2d effect with Vertex2DFormat
       */
    class Ukn2DHelper: public mist::Singleton<Ukn2DHelper> {
    protected:
        Ukn2DHelper();
        ~Ukn2DHelper();

    public:
        /* setup with screen orthogonal mat(current framebuffer) & identity transform mat */
        void setupMat();
        /* setup with custom transform mat */
        void setupMat(const mist::Matrix4& viewMat);
        /* setup with custom orthogonal mat & transform mat*/
        void setupMat(const mist::Matrix4& orthoMat, const mist::Matrix4& viewMat);

        void bindTexture(const TexturePtr& texture);

        const EffectPtr& getEffect() const;

    private:
        void initialize();

        EffectPtr mEffect;
        mist::Matrix4 mScreenOrthoMat;
        mist::Matrix4 mScreenTransformMat;

        friend struct ClassInstantiator<Ukn2DHelper>;
    };


}


#endif // UKN_2D_HELPER_H_
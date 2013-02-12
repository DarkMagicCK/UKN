#ifndef PROJECT_UNKNOWN_SKYBOX_H_
#define PROJECT_UNKNOWN_SKYBOX_H_


#include "UKN/PreDeclare.h"

namespace ukn {

    /* 
        simple implementation without cubemap 
        since TextureCubemap is not done yet
    */

    class UKN_API Skybox {
    public:
        Skybox();
        ~Skybox();

        /* load an aligned cubemap skybox in a texture */
        bool load(const ResourcePtr& resource);

        void render();

    private:
        bool buildVertices(const GraphicFactory& factory);

        RenderBufferPtr mRenderBuffer;
        GraphicBufferPtr mVertexBuffer;

        TexturePtr mTexture;
    };

    typedef SharedPtr<Skybox> SkyboxPtr;

} // namespace ukn

#endif
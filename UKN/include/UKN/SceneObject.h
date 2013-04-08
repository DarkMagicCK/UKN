#ifndef Project_Unknown_SceneObject_h
#define Project_Unknown_SceneObject_h

#include "UKN/PreDeclare.h"
#include "UKN/Renderable.h"

namespace ukn {

    enum SceneObjectAttribute {
        SOA_Cullable    = 1UL << 0,
        SOA_Overlay     = 1UL << 1,
        SOA_Moveable    = 1UL << 2,
        SOA_Unvisible   = 1UL << 3,
        SOA_NotCastShadow = 1UL << 4,
    };
    
    class UKN_API SceneObject {
    public:
        explicit SceneObject(uint32 attribute);
        SceneObject(const RenderablePtr& renderable, uint32 attr);

        virtual ~SceneObject();
        
        virtual void setModelMatrix(const Matrix4& mat);
        virtual const Matrix4& getModelMatrix() const;
        virtual Box getBound() const;
        
        virtual void addToSceneManager();
        virtual void update();
        
    public:
        RenderablePtr& getRenderable();
        const RenderablePtr& getRenderable() const;
        
        uint32 getAttribute() const;
        bool isVisible() const;
        void setVisible(bool flag);
        
    protected:
        uint32 mAttribute;
        RenderablePtr mRenderable;
        Matrix4 mModelMat;
    };
    

}




#endif
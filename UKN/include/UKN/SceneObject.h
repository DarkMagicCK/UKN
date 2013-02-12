#ifndef Project_Unknown_SceneObject_h
#define Project_Unknown_SceneObject_h

#include "UKN/PreDeclare.h"

namespace ukn {

    enum SceneObjectAttribute {
        SOA_Cullable    = 1UL << 0,
        SOA_Overlay     = 1UL << 1,
        SOA_Moveable    = 1UL << 2,
        SOA_Unvisible   = 1UL << 3
    };
    
    class UKN_API SceneObject {
    public:
        explicit SceneObject(uint32 attribute);
        virtual ~SceneObject();
        
        const RenderablePtr& getRenderable() const;
        
        virtual const Matrix4& getModelMatrix() const;
        virtual Box getBound() const;
        
        virtual void addToSceneManager();
        virtual void update();
        
        uint32 getAttribute() const;
        bool isVisible() const;
        void setVisible(bool flag);
        
    protected:
        uint32 mAttribute;
        RenderablePtr mRenderable;
    };
    

}




#endif
//
//  Config.h
//  Mist
//
//  Created by Robert Bu on 11/24/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTConfigParser_h
#define MISTConfigParser_h

#include "mist/Platform.h"
#include "mist/Uncopyable.h"
#include "mist/Resource.h"
#include "mist/PreDeclare.h"

#include <vector>

namespace mist {
    
    /**
     * Abstract config loader/writer
     * Currently only supports xml
     **/
    enum ConfigParserType {
        CPT_XML,
        CPT_JSON,
    };
    
    class MIST_API ConfigParser: public IResource, Uncopyable {       
    public:
        virtual ~ConfigParser() { }
        
        virtual bool open(ResourcePtr resource) = 0;

        virtual bool create() = 0;
        
        virtual const MistString& getName() const = 0;
        
        virtual StreamPtr  writeToStream(const char* indent="\n") const = 0;
        virtual MistString writeToString(const char* indent="\n") const = 0;
        
        virtual void close() = 0;
        
        virtual bool hasNode(const MistString& node) const = 0;
        virtual MistString getCurrentNodeName() const = 0;
        virtual MistString getCurrentNodePath() const = 0;
        
        /**
         * set current node to a node, / for root, otherwise use relative path
         * /config/window/ etc
         **/
        virtual bool toNode(const MistString& node) = 0;
        
        /**
         * set current node to first node child
         **/
        virtual bool toFirstChild(const MistString& name=MistString()) = 0;
        /**
         * set current node to next slibling child
         **/
        virtual bool toNextChild(const MistString& name=MistString()) = 0;
        /**
         * set current node to node parent
         **/
        virtual bool toParent() = 0;
        
        
        virtual bool hasAttribute(const MistString& attr) const = 0;
        /**
         * get all attributes of current node
         **/
        virtual std::vector<MistString> getAttributes() const = 0;
        
        /**
         * get node attribute as optinal string
         */
        virtual MistString getString(const MistString& attr, const MistString& opt = MistString()) const = 0;
        /**
         * get node attribute as optinal bool
         */
        virtual bool getBool(const MistString& attr, bool opt = false) const = 0;
        /**
         * get node attribute as optinal bool
         */
        virtual int32 getInt(const MistString& attr, int32 opt = 0) const = 0;
        /**
         * get node attribute as optinal bool
         */
        virtual float getFloat(const MistString& attr, float opt = 0.f) const = 0;
        
        
        /**
         * Begin a new node under current node
         **/
        virtual bool beginNode(const MistString& nodeName) = 0;
        
        virtual void endNode() = 0;
        
        /**
         * set the value of current node
         **/
        virtual void setValue(const MistString& val) = 0;
        /**
         * set a string attribute under current node
         **/
        virtual void setString(const MistString& attr, const MistString& val) = 0;
        /**
         * set a bool attribute under current node
         **/
        virtual void setBool(const MistString& attr, bool val) = 0;
        /**
         * set a int attribute under current node
         **/
        virtual void setInt(const MistString& attr, int32 val) = 0;
        /**
         * set a float attribute under current node
         **/
        virtual void setFloat(const MistString& attr, float val) = 0;
        
        static ConfigParserPtr MakeEmptyParser(ConfigParserType type);
        static ConfigParserPtr MakeParser(ResourcePtr resource);
    };
    
    
    
} // namespace mist


#endif

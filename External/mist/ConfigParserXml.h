//
//  ConfigParserXml.h
//  Mist
//
//  Created by Robert Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef MISTConfigParserXml_h
#define MISTConfigParserXml_h

#include "mist/ConfigParser.h"
#include "pugixml/pugixml.hpp"

namespace mist {
    
    class ConfigParserXmlImpl: public ConfigParser {
    public:
        ConfigParserXmlImpl();
        virtual ~ConfigParserXmlImpl();
        
        virtual bool open(ResourcePtr resource);
        
        virtual bool create();
        
        virtual StreamPtr  writeToStream(const char* indent="\n") const;
        virtual MistString writeToString(const char* indent="\n") const;
        
        virtual void close();
        
        virtual bool        hasNode(const MistString& node) const;
        virtual MistString  getCurrentNodeName() const;
        virtual MistString  getCurrentNodePath() const;
        
        virtual bool toNode(const MistString& node);
        
        virtual bool toFirstChild(const MistString& name=MistString());
        virtual bool toNextChild(const MistString& name=MistString());
        virtual bool toParent();
        
        virtual bool hasAttribute(const MistString& attr) const;
        virtual std::vector<MistString> getAttributes() const;
        
        virtual MistString  getString(const MistString& attr, const MistString& opt) const;
        virtual bool        getBool(const MistString& attr, bool opt) const;
        virtual int32       getInt(const MistString& attr, int32 opt) const;
        virtual float       getFloat(const MistString& attr, float opt) const;
        
        MistString  getFormattedString() const;
        
        virtual bool beginNode(const MistString& name);
        virtual void endNode();
        
        virtual void setValue(const MistString& val);
        virtual void setString(const MistString& attr, const MistString& val);
        virtual void setBool(const MistString& attr, bool val);
        virtual void setInt(const MistString& attr, int32 val);
        virtual void setFloat(const MistString& attr, float val);
        
        const MistString& getName() const;
        
    private:
        struct myWritter: public pugi::xml_writer {
            void write(const void* data, size_t size);
            std::string str;
            
            myWritter(const char* indent);
            
            MistString indent;
        };
        
        struct myStreamWritter: public pugi::xml_writer {
            myStreamWritter(const char* indent);
            
            void write(const void* data, size_t size);            
            StreamPtr stream;
            MistString indent;
        };
        
        MistString mName;

        pugi::xml_document* mDocument;
        pugi::xml_node mCurrNode;
    };
    
} // namespace mist

#endif

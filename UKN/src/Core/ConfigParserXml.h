//
//  ConfigParserXml.h
//  Project Unknown
//
//  Created by Robert Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#ifndef Project_Unknown_ConfigParserXml_h
#define Project_Unknown_ConfigParserXml_h

#include "UKN/ConfigParser.h"
#include "pugixml/pugixml.hpp"

namespace ukn {
    
    class ConfigParserXmlImpl: public ConfigParser {
    public:
        ConfigParserXmlImpl();
        virtual ~ConfigParserXmlImpl();
        
        virtual bool open(ResourcePtr resource);
        
        virtual bool create();
        
        virtual StreamPtr  writeToStream(const char* indent="\n") const;
        virtual UknString writeToString(const char* indent="\n") const;
        
        virtual void close();
        
        virtual bool        hasNode(const UknString& node) const;
        virtual UknString  getCurrentNodeName() const;
        virtual UknString  getCurrentNodePath() const;
        
        virtual bool toNode(const UknString& node);
        
        virtual bool toFirstChild(const UknString& name=UknString());
        virtual bool toNextChild(const UknString& name=UknString());
        virtual bool toParent();
        
        virtual bool hasAttribute(const UknString& attr) const;
        virtual std::vector<UknString> getAttributes() const;
        
        virtual UknString  getString(const UknString& attr, const UknString& opt) const;
        virtual bool        getBool(const UknString& attr, bool opt) const;
        virtual int32       getInt(const UknString& attr, int32 opt) const;
        virtual float       getFloat(const UknString& attr, float opt) const;
        
        UknString  getFormattedString() const;
        
        virtual bool beginNode(const UknString& name);
        virtual void endNode();
        
        virtual void setValue(const UknString& val);
        virtual void setString(const UknString& attr, const UknString& val);
        virtual void setBool(const UknString& attr, bool val);
        virtual void setInt(const UknString& attr, int32 val);
        virtual void setFloat(const UknString& attr, float val);
        
        const String& getName() const;
        
    private:
        struct myWritter: public pugi::xml_writer {
            void write(const void* data, size_t size);
            std::string str;
            
            myWritter(const char* indent);
            
            UknString indent;
        };
        
        struct myStreamWritter: public pugi::xml_writer {
            myStreamWritter(const char* indent);
            
            void write(const void* data, size_t size);            
            StreamPtr stream;
            UknString indent;
        };
        
        String mName;

        pugi::xml_document* mDocument;
        pugi::xml_node mCurrNode;
    };
    
} // namespace ukn

#endif

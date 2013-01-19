//
//  ConfigParserXml.cpp
//  Mist
//
//  Created by Robert Bu on 11/30/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "ConfigParserXml.h"
#include "mist/Stream.h"
#include "mist/Resource.h"
#include "mist/StringUtil.h"
#include "mist/SysUtil.h"

namespace mist {
    
    ConfigParserXmlImpl::ConfigParserXmlImpl():
    mDocument(0) {
        
    }
    
    ConfigParserXmlImpl::~ConfigParserXmlImpl() {
        close();
    }
    
    bool ConfigParserXmlImpl::open(ResourcePtr resource) {
        StreamPtr dataStream = resource->getResourceStream()->readIntoMemory();
        if(dataStream) {
            if(!mDocument) {
                mDocument = new pugi::xml_document;
            }
            bool result = mDocument->load_buffer((void*)static_cast<MemoryStream*>(dataStream.get())->data(), dataStream->size(), pugi::encoding_wchar);
            if(result) {
                mName = resource->getName();
                mCurrNode = mDocument->root();
                return result;
            }
        }
        return false;
    }

    bool ConfigParserXmlImpl::create() {
        if(mDocument) {
            delete mDocument;
        }
        mDocument = new pugi::xml_document;
        mCurrNode = mDocument->root();
        return true;
    }
    
    void ConfigParserXmlImpl::close() {
        if(mDocument) {
            delete mDocument;
            mDocument = 0;
        }
    }
    
    bool ConfigParserXmlImpl::hasNode(const MistString& node) const {
        if(!mDocument)
            return false;
        
        pugi::xml_node nextNode = node[0] == '/' ? mCurrNode.root() : mCurrNode;
        
        StringTokenlizer tokens(node,
                                L"/");
        if(tokens.size() == 0)
            return false;
        
        StringTokenlizer::iterator it = tokens.begin();
        while(it != tokens.end()) {
            if((*it).size() == 0) {
                ++it;
                continue;
            }
            
            nextNode = nextNode.child(string::WStringToString((*it)).c_str());
            if(!nextNode)
                return false;
            ++it;
        }
        return true;
    }
    
    MistString ConfigParserXmlImpl::getCurrentNodeName() const {
        if(mDocument)
            return string::StringToWString(mCurrNode.name());
        return MistString();
    }
    
    MistString ConfigParserXmlImpl::getCurrentNodePath() const {
        if(mDocument)
            return string::StringToWString(mCurrNode.path());
        return MistString();
    }
    
    bool ConfigParserXmlImpl::toNode(const MistString& node) {
        if(!mDocument)
            return false;
        
        if(node.size() == 1 && node[0] == '/') {
            mCurrNode = mDocument->root();
            return true;
        }
        
        StringTokenlizer tokens(node,
                                L"/");
        if(tokens.size() == 0)
            return false;
        
        pugi::xml_node nextNode = node[0] == '/' ? mCurrNode.root() : mCurrNode;
        StringTokenlizer::iterator it = tokens.begin();
        while(it != tokens.end()) {
            if((*it).size() == 0) {
                ++it;
                continue;
            }
            
            nextNode = nextNode.child(string::WStringToString((*it)).c_str());
            if(!nextNode)
                return false;
            ++it;
        }
        
        mCurrNode = nextNode;
        return true;
    }
    
    bool ConfigParserXmlImpl::toFirstChild(const MistString& name) {
        if(!mDocument)
            return false;
        
        pugi::xml_node node;
        if(name.empty()) {
            node = mCurrNode.first_child();
        } else {
            node = mCurrNode.child(string::WStringToString(name).c_str());
        }
        if(node) {
            mCurrNode = node;
            return true;
        }
        return false;
    }
    
    bool ConfigParserXmlImpl::toNextChild(const MistString& name) {
        if(!mDocument)
            return false;
        
        pugi::xml_node node;
        if(name.empty()) {
            node = mCurrNode.next_sibling();
        } else {
            node = mCurrNode.next_sibling(string::WStringToString(name).c_str());
        }
        if(node) {
            mCurrNode = node;
            return true;
        }
        return false;
    }
    
    bool ConfigParserXmlImpl::toParent() {
        if(!mDocument)
            return false;
        
        pugi::xml_node node = mCurrNode.parent();
        if(node) {
            mCurrNode = node;
            return true;
        }
        return false;
    }
    
    bool ConfigParserXmlImpl::hasAttribute(const MistString& name) const {
        if(!mDocument)
            return false;
        
        pugi::xml_attribute attr = mCurrNode.attribute(string::WStringToString(name).c_str());
        if(attr) {
            return true;
        }
        return false;
    }
    
    std::vector<MistString> ConfigParserXmlImpl::getAttributes() const {
        if(!mDocument)
            return std::vector<MistString>();
        
        pugi::xml_attribute_iterator it = mCurrNode.attributes_begin();
        pugi::xml_attribute_iterator end = mCurrNode.attributes_end();
        
        std::vector<MistString> attributes;
        for(; it != end; ++it) {
            attributes.push_back(string::StringToWString(it->name()));
        }
        return attributes;
    }
    
    MistString ConfigParserXmlImpl::getString(const MistString& attr, const MistString& opt) const {
        if(!mDocument)
            return opt;
        
        if(attr.empty()) {
            MistString child_value = string::StringToWString(mCurrNode.child_value());
            return child_value;
        }
        pugi::xml_attribute attribute = mCurrNode.attribute(string::WStringToString(attr).c_str());
        if(attribute) {
            return string::StringToWString(attribute.value());
        } else {
            MistString c = string::StringToWString(mCurrNode.child_value(string::WStringToString(attr).c_str()));
            if(!c.empty())
                return c;
        }
        return opt;
    }
    
    bool ConfigParserXmlImpl::getBool(const MistString& attr, bool opt) const {
        if(!mDocument)
            return opt;
        
        if(attr.empty()) {
            return opt;
        }
        pugi::xml_attribute attribute = mCurrNode.attribute(string::WStringToString(attr).c_str());
        if(attribute) {
            return attribute.as_bool();
        }
        return opt;
    }
    
    int32 ConfigParserXmlImpl::getInt(const MistString& attr, int32 opt) const {
        if(!mDocument)
            return opt;
        
        if(attr.empty())
            return opt;
        pugi::xml_attribute attribute = mCurrNode.attribute(string::WStringToString(attr).c_str());
        if(attribute) {
            return attribute.as_int();
        }
        return opt;
    }
    
    float ConfigParserXmlImpl::getFloat(const MistString& attr, float opt) const {
        if(!mDocument)
            return opt;
        
        if(attr.empty())
            return opt;
        pugi::xml_attribute attribute = mCurrNode.attribute(string::WStringToString(attr).c_str());
        if(attribute) {
            return attribute.as_float();
        }
        return opt;
    }
    
    void ConfigParserXmlImpl::myWritter::write(const void* data, size_t size) {
        str += std::string((const char*)data, size);
    }
    
    void ConfigParserXmlImpl::myStreamWritter::write(const void* data, size_t size) {
        stream->write((const uint8*)data, size);
        stream->write((const uint8*)indent.data(), indent.size());
    }
    
    MistString ConfigParserXmlImpl::writeToString(const char* indent) const {
        if(!mDocument)
            return MistString();
        
        myWritter writter(indent);
        mDocument->save(writter, PUGIXML_TEXT("\t"), pugi::format_indent, pugi::encoding_wchar);
        return string::StringToWString(writter.str);
    }
    
    ConfigParserXmlImpl::myStreamWritter::myStreamWritter(const char* indent) {
        stream = MakeSharedPtr<MemoryStream>();
    }
    
    ConfigParserXmlImpl::myWritter::myWritter(const char* indent) {

    }
    
    StreamPtr ConfigParserXmlImpl::writeToStream(const char* indent) const {
        if(!mDocument)
            return MakeSharedPtr<MemoryStream>();
        
        myStreamWritter writter(indent);
        mDocument->save(writter, PUGIXML_TEXT("\t"), pugi::format_indent, pugi::encoding_wchar);
        return writter.stream;
    }
    
    bool ConfigParserXmlImpl::beginNode(const MistString& name) {
        if(!mDocument)
            return false;
        
        pugi::xml_node node = mCurrNode.append_child(string::WStringToString(name).c_str());
        if(node) {
            mCurrNode = node;
            return true;
        }
        return false;
    }
    
    void ConfigParserXmlImpl::endNode() {
        toParent();
    }
    
    void ConfigParserXmlImpl::setValue(const MistString& val) {
        if(!mDocument)
            return;
        
        mCurrNode.set_value(string::WStringToString(val).c_str());
    }
    
    void ConfigParserXmlImpl::setString(const MistString& attr, const MistString& val) {
        if(!mDocument)
            return;
        
        pugi::xml_attribute attribute = mCurrNode.attribute(string::WStringToString(attr).c_str());
        if(attribute)
            attribute.set_value(string::WStringToString(val).c_str());
        else {
            attribute = mCurrNode.append_attribute(string::WStringToString(attr).c_str());
            attribute.set_value(string::WStringToString(val).c_str());
        }
    }
    
    void ConfigParserXmlImpl::setBool(const MistString& attr, bool val) {
        if(!mDocument)
            return;
        
        pugi::xml_attribute attribute = mCurrNode.attribute(string::WStringToString(attr).c_str());
        if(attribute)
            attribute.set_value(val);
        else {
            attribute = mCurrNode.append_attribute(string::WStringToString(attr).c_str());
            attribute.set_value(val);
        }
    }
    
    void ConfigParserXmlImpl::setInt(const MistString& attr, int32 val) {
        if(!mDocument)
            return;
        
        pugi::xml_attribute attribute = mCurrNode.attribute(string::WStringToString(attr).c_str());
        if(attribute)
            attribute.set_value(val);
        else {
            attribute = mCurrNode.append_attribute(string::WStringToString(attr).c_str());
            attribute.set_value(val);
        }
    }
    
    void ConfigParserXmlImpl::setFloat(const MistString& attr, float val) {
        if(!mDocument)
            return;
        
        pugi::xml_attribute attribute = mCurrNode.attribute(string::WStringToString(attr).c_str());
        if(attribute)
            attribute.set_value(val);
        else {
            attribute = mCurrNode.append_attribute(string::WStringToString(attr).c_str());
            attribute.set_value(val);
        }
    }
    
    const MistString& ConfigParserXmlImpl::getName() const {
        return mName;
    }
    
    
} // namespace mist

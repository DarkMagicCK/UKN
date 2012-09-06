//
//  LocalizedStringTableLoader.cpp
//  Project Unknown
//
//  Created by Robert Bu on 7/22/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#include "ukn/LocalizedStringTableLoader.h"
#include "ukn/LocalizedStringTable.h"

#include "ukn/ConfigParser.h"
#include "ukn/Resource.h"

namespace ukn {
    
    bool LocalizedStringTableLoader::Load(const ukn_string& file, LocalizedStringTable& stringTable) {
        ukn::ConfigParserPtr parser = ukn::ConfigParser::MakeParser(ukn::ResourceLoader::Instance().loadResource(file));
        if(parser &&
           parser->toNode(L"strings")) {
            
            parser->toFirstChild();
            
            do {
                ukn_string lanName = parser->getCurrentNodeName();
                if(!lanName.empty()) {
                    stringTable.addLanguage(lanName);
                    
                    parser->toFirstChild();
                    
                    do {
                        ukn_string stringId = parser->getCurrentNodeName();
                        if(!stringId.empty()) {
                            
                            ukn_string value = parser->getString(L"value");
                            
                            stringTable.addString(lanName, stringId, value); 
                        }
                        
                    } while(parser->toNextChild());
                    
                    parser->toParent();
                }
                
            } while(parser->toNextChild());
            
            return true;
        }

        return false;
    }
    
    bool LocalizedStringTableLoader::Save(const ukn_string& file, const LocalizedStringTable& stringTable) {
        ukn::ConfigParserPtr parser = ukn::ConfigParser::MakeEmptyParser(ukn::CPT_XML);
        
        if(parser) {
            parser->beginNode(L"strings");
            
            for(LocalizedStringTable::LanguageMap::const_iterator it = stringTable.begin(),
                end = stringTable.end();
                it != end;
                ++it) {
                parser->beginNode(it->first);
                
                const StringMap& stringMap = it->second;
                
                for(StringMap::StringMapCont::const_iterator it = stringMap.begin(),
                    end = stringMap.end();
                    it != end;
                    ++it) {
                    parser->beginNode(it->first);
                    parser->setString(L"value", it->second);
                    parser->endNode(); // String->first;
                }
                
                parser->endNode();
            }
            
            parser->endNode();
            
            ukn_string content = parser->writeToString();
            
            FILE* pfile = fopen(String::WStringToString(file).c_str(),
                                "w+");
            if(!pfile)
                return false;
            
            fwrite(content.c_str(), 1, content.size(), pfile);
            fclose(pfile);
            
            return true;
        }
        return false;
    }
    
    
}
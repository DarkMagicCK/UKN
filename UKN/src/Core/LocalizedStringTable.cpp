//
//  LocalizedStringTable.cpp
//  StringEditor
//
//  Created by Ruiwei Bu on 7/19/12.
//  Copyright (c) 2012 Studio Symphonie. All rights reserved.
//

#include "ukn/LocalizedStringTable.h"
#include "ukn/ConfigParser.h"

namespace ukn {
    
    ukn_string StringMap::stringAtIndex(size_t index) const {
        if(index < mStringMap.size()) {
            StringMap::StringMapCont::const_iterator it = mStringMap.begin();
            std::advance(it, index);
            return it->second;
        }
        return L"";
    }
    
    ukn_string StringMap::stringAtId(const ukn_string& sid) const {
        StringMapCont::const_iterator it = mStringMap.find(sid);
        return it != mStringMap.end() ? it->second : L"";
    }
    
    void StringMap::setStringAtId(const ukn_string& sid, const ukn_string& string) {
        StringMapCont::iterator it = mStringMap.find(sid);
        if(it != mStringMap.end()) {
            it->second = string;
        }
    }
    
    void StringMap::addString(const ukn_string& sid, const ukn_string& string) {
        mStringMap.insert(std::make_pair(sid, string));
    }
    
    void StringMap::removeStringAtId(const ukn_string& sid) {
        StringMapCont::iterator it = mStringMap.find(sid);
        if(it != mStringMap.end()) {
            mStringMap.erase(it);
        }
    }
    
    size_t StringMap::size() const {
        return mStringMap.size();
    }
    
    void StringMap::setStringIdAtId(const ukn_string& sid, const ukn_string& after) {
        StringMapCont::iterator it = mStringMap.find(sid);
        if(it != mStringMap.end()) {
            ukn_string value = it->second;
            mStringMap.erase(it);
            mStringMap.insert(std::make_pair(after, value));
        }
    }
    
    const StringMap& LocalizedStringTable::languageStringMapAtId(const ukn_string& lanName) const {
        LanguageMap::const_iterator it = mLocalizedString.find(lanName);
        if(it != mLocalizedString.end()) {
            return it->second;
        } else {
            throw Exception("cannot find language name");
        }
    }
    
    StringMap& LocalizedStringTable::languageStringMapAtId(const ukn_string& lanName) {
        LanguageMap::iterator it = mLocalizedString.find(lanName);
        if(it != mLocalizedString.end()) {
            return it->second;
        } else {
            throw Exception("cannot find language name");
        }
    }
    
    void LocalizedStringTable::addLanguage(const ukn_string& lanName) {
        mLocalizedString.insert(std::make_pair(lanName, StringMap()));
    }
    
    void LocalizedStringTable::removeLanguage(const ukn_string& lanName) {
        LocalizedStringTable::iterator it = mLocalizedString.find(lanName);
        if(it != mLocalizedString.end()) {
            mLocalizedString.erase(it);
        }
    }
    
    void LocalizedStringTable::setCurrentLanguage(const ukn_string& language) {
        mCurrentLanguage = language;
    }
    
    const ukn_string& LocalizedStringTable::getCurrentLanguage() const {
        return mCurrentLanguage;
    }
    
    ukn_string LocalizedStringTable::stringById(const ukn_string& sid) const {
        if(mCurrentLanguage.empty())
            return L"";
        
        const StringMap& map = languageStringMapAtId(mCurrentLanguage);
        return map.stringAtId(sid);
    }
    
    void LocalizedStringTable::setStringById(const ukn_string& sid, const ukn_string& string) {
        if(mCurrentLanguage.empty())
            return;
        
        StringMap& map = languageStringMapAtId(mCurrentLanguage);
        map.setStringAtId(sid, string);
    }
    
    void LocalizedStringTable::addString(const ukn_string& sid, const ukn_string& string) {
        for(LanguageMap::iterator it = mLocalizedString.begin(),
            end = mLocalizedString.end();
            it != end;
            ++it) {
            it->second.addString(sid, string);
        }
        mIdVector.push_back(sid);
    }
    
    void LocalizedStringTable::removeStringById(const ukn_string& sid) {
        for(LanguageMap::iterator it = mLocalizedString.begin(),
            end = mLocalizedString.end();
            it != end;
            ++it) {
            it->second.removeStringAtId(sid);
        }
        
        for(IdVector::iterator it = mIdVector.begin(),
            end = mIdVector.end();
            it != end;
            ++it) {
            if(*it == sid) {
                mIdVector.erase(it);
                break;
            }
        }
        
    }
    
    ukn_string LocalizedStringTable::getString(const ukn_string& lan, const ukn_string& sid) {
        StringMap& map = languageStringMapAtId(lan);
        return map.stringAtId(sid);
    }
    
    void LocalizedStringTable::setString(const ukn_string& lan, const ukn_string& sid, const ukn_string& string) {
        StringMap& map = languageStringMapAtId(lan);
        map.setStringAtId(sid, string);
    }
    
    size_t LocalizedStringTable::sizeOfLanguages() const {
        return mLocalizedString.size();
    }
    
    size_t LocalizedStringTable::sizeOfStrings() const {
        if(mLocalizedString.empty())
            return 0;
        return mLocalizedString.begin()->second.size();
    }
    
    void LocalizedStringTable::setStringIdAtId(const ukn_string& before, const ukn_string& after) {
        for(LanguageMap::iterator it = mLocalizedString.begin(),
            end = mLocalizedString.end();
            it != end;
            ++it)  {
            it->second.setStringIdAtId(before, after);
        }
        updateStringId(before, after);
    }
    
    ukn_string LocalizedStringTable::getIdByIndex(size_t index) const {
        return mIdVector[index];
    }

    void LocalizedStringTable::clear() {
        mLocalizedString.clear();
    }
    
    bool LocalizedStringTable::checkLanguage(const ukn_string& lanName) const {
        return mLocalizedString.find(lanName) == mLocalizedString.end();
    }
    bool LocalizedStringTable::checkStringId(const ukn_string& stringId) const {
        return std::find(mIdVector.begin(),
                         mIdVector.end(),
                         stringId) == mIdVector.end();
    }
    
    void LocalizedStringTable::updateStringId(const ukn_string& before, const ukn_string& after) {
        for(IdVector::iterator it = mIdVector.begin(),
            end = mIdVector.end();
            it != end;
            ++it) {
            if(*it == before) {
                *it = after;
                break;
            }
        }
    }
    
    void LocalizedStringTable::addString(const ukn_string& lan, const ukn_string& sid, const ukn_string& string) {
        StringMap& map = languageStringMapAtId(lan);
        map.addString(sid, string);
        
        if(std::find(mIdVector.begin(),
                     mIdVector.end(),
                     sid) == mIdVector.end()) {
            mIdVector.push_back(sid);
        }
    }
}

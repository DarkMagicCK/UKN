//
//  LocalizedStringTable.cpp
//  StringEditor
//
//  Created by Ruiwei Bu on 7/19/12.
//  Copyright (c) 2012 Studio Symphonie. All rights reserved.
//

#include "ukn/LocalizedStringTable.h"

#include "mist/ConfigParser.h"

namespace ukn {
    
    UknString StringMap::stringAtIndex(size_t index) const {
        if(index < mStringMap.size()) {
            StringMap::StringMapCont::const_iterator it = mStringMap.begin();
            std::advance(it, index);
            return it->second;
        }
        return L"";
    }
    
    UknString StringMap::stringAtId(const UknString& sid) const {
        StringMapCont::const_iterator it = mStringMap.find(sid);
        return it != mStringMap.end() ? it->second : L"";
    }
    
    void StringMap::setStringAtId(const UknString& sid, const UknString& string) {
        StringMapCont::iterator it = mStringMap.find(sid);
        if(it != mStringMap.end()) {
            it->second = string;
        }
    }
    
    void StringMap::addString(const UknString& sid, const UknString& string) {
        mStringMap.insert(std::make_pair(sid, string));
    }
    
    void StringMap::removeStringAtId(const UknString& sid) {
        StringMapCont::iterator it = mStringMap.find(sid);
        if(it != mStringMap.end()) {
            mStringMap.erase(it);
        }
    }
    
    size_t StringMap::size() const {
        return mStringMap.size();
    }
    
    void StringMap::setStringIdAtId(const UknString& sid, const UknString& after) {
        StringMapCont::iterator it = mStringMap.find(sid);
        if(it != mStringMap.end()) {
            UknString value = it->second;
            mStringMap.erase(it);
            mStringMap.insert(std::make_pair(after, value));
        }
    }
    
    const StringMap& LocalizedStringTable::languageStringMapAtId(const UknString& lanName) const {
        LanguageMap::const_iterator it = mLocalizedString.find(lanName);
        if(it != mLocalizedString.end()) {
            return it->second;
        } else {
            throw Exception("cannot find language name");
        }
    }
    
    StringMap& LocalizedStringTable::languageStringMapAtId(const UknString& lanName) {
        LanguageMap::iterator it = mLocalizedString.find(lanName);
        if(it != mLocalizedString.end()) {
            return it->second;
        } else {
            throw Exception("cannot find language name");
        }
    }
    
    void LocalizedStringTable::addLanguage(const UknString& lanName) {
        mLocalizedString.insert(std::make_pair(lanName, StringMap()));
    }
    
    void LocalizedStringTable::removeLanguage(const UknString& lanName) {
        LocalizedStringTable::iterator it = mLocalizedString.find(lanName);
        if(it != mLocalizedString.end()) {
            mLocalizedString.erase(it);
        }
    }
    
    void LocalizedStringTable::setCurrentLanguage(const UknString& language) {
        mCurrentLanguage = language;
    }
    
    const UknString& LocalizedStringTable::getCurrentLanguage() const {
        return mCurrentLanguage;
    }
    
    UknString LocalizedStringTable::stringById(const UknString& sid) const {
        if(mCurrentLanguage.empty())
            return L"";
        
        const StringMap& map = languageStringMapAtId(mCurrentLanguage);
        return map.stringAtId(sid);
    }
    
    void LocalizedStringTable::setStringById(const UknString& sid, const UknString& string) {
        if(mCurrentLanguage.empty())
            return;
        
        StringMap& map = languageStringMapAtId(mCurrentLanguage);
        map.setStringAtId(sid, string);
    }
    
    void LocalizedStringTable::addString(const UknString& sid, const UknString& string) {
        for(LanguageMap::iterator it = mLocalizedString.begin(),
            end = mLocalizedString.end();
            it != end;
            ++it) {
            it->second.addString(sid, string);
        }
        mIdVector.push_back(sid);
    }
    
    void LocalizedStringTable::removeStringById(const UknString& sid) {
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
    
    UknString LocalizedStringTable::getString(const UknString& lan, const UknString& sid) {
        StringMap& map = languageStringMapAtId(lan);
        return map.stringAtId(sid);
    }
    
    void LocalizedStringTable::setString(const UknString& lan, const UknString& sid, const UknString& string) {
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
    
    void LocalizedStringTable::setStringIdAtId(const UknString& before, const UknString& after) {
        for(LanguageMap::iterator it = mLocalizedString.begin(),
            end = mLocalizedString.end();
            it != end;
            ++it)  {
            it->second.setStringIdAtId(before, after);
        }
        updateStringId(before, after);
    }
    
    UknString LocalizedStringTable::getIdByIndex(size_t index) const {
        return mIdVector[index];
    }

    void LocalizedStringTable::clear() {
        mLocalizedString.clear();
    }
    
    bool LocalizedStringTable::checkLanguage(const UknString& lanName) const {
        return mLocalizedString.find(lanName) == mLocalizedString.end();
    }
    bool LocalizedStringTable::checkStringId(const UknString& stringId) const {
        return std::find(mIdVector.begin(),
                         mIdVector.end(),
                         stringId) == mIdVector.end();
    }
    
    void LocalizedStringTable::updateStringId(const UknString& before, const UknString& after) {
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
    
    void LocalizedStringTable::addString(const UknString& lan, const UknString& sid, const UknString& string) {
        StringMap& map = languageStringMapAtId(lan);
        map.addString(sid, string);
        
        if(std::find(mIdVector.begin(),
                     mIdVector.end(),
                     sid) == mIdVector.end()) {
            mIdVector.push_back(sid);
        }
    }
}

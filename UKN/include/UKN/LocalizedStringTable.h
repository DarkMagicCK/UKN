//
//  LocalizedStringTable.h
//  StringEditor
//
//  Created by Ruiwei Bu on 7/19/12.
//  Copyright (c) 2012 Studio Symphonie. All rights reserved.
//

#ifndef StringEditor_LocalizedStringTable_h
#define StringEditor_LocalizedStringTable_h

#include <map>
#include <vector>
#include <string>
#include <exception>

#include "UKN/PreDeclare.h"

namespace ukn {

    class StringMap {
    public:
        typedef std::map<UknString, UknString> StringMapCont;
        
        UknString stringAtIndex(size_t index) const;
        UknString stringAtId(const UknString& sid) const;

        void setStringAtId(const UknString& sid, const UknString& string);
        void addString(const UknString& sid, const UknString& string);
        void removeStringAtId(const UknString& sid);
        void setStringIdAtId(const UknString& sid, const UknString& after);
        
        size_t size() const;

        typedef StringMapCont::iterator iterator;
        typedef StringMapCont::const_iterator const_iterator;
        
        iterator begin() { return mStringMap.begin(); }
        iterator end() { return mStringMap.end(); }
        
        const_iterator begin() const { return mStringMap.begin(); }
        const_iterator end() const { return mStringMap.end(); }
        
        
    private:
        StringMapCont mStringMap;
    };
    
    class LocalizedStringTable {
    public:
        typedef std::map<UknString /* lan */, StringMap /* StringMap */> LanguageMap;
        
        const StringMap& languageStringMapAtId(const UknString& lanName) const;
        StringMap& languageStringMapAtId(const UknString& lanName);
        
        void addLanguage(const UknString& lanName);

        void removeLanguage(const UknString& lanName);
        void setCurrentLanguage(const UknString& language);
        
        const UknString& getCurrentLanguage() const;
        
        UknString stringById(const UknString& sid) const;
        
        void setStringById(const UknString& sid, const UknString& string);
        void addString(const UknString& sid, const UknString& string);
        void addString(const UknString& lan, const UknString& sid, const UknString& string);

        void removeStringById(const UknString& sid);
        
        UknString getString(const UknString& lan, const UknString& sid);
        void setString(const UknString& lan, const UknString& sid, const UknString& string);
        void setStringIdAtId(const UknString& before, const UknString& after);
        
        UknString getIdByIndex(size_t index) const;
        
        size_t sizeOfLanguages() const;
        size_t sizeOfStrings() const;
        
        void clear();
    
        bool checkLanguage(const UknString& lanName) const;
        bool checkStringId(const UknString& stringId) const;
        
        typedef LanguageMap::iterator iterator;
        typedef LanguageMap::const_iterator const_iterator;
        
        iterator begin() { return mLocalizedString.begin(); }
        iterator end()   { return mLocalizedString.end(); }
        
        const_iterator begin() const { return mLocalizedString.begin(); }
        const_iterator end()   const { return mLocalizedString.end(); }
        
    private:
        friend class LocalizedStringTableLoader;
        void updateStringId(const UknString& before, const UknString& after);
        
        LanguageMap mLocalizedString;
        
        typedef std::vector<UknString> IdVector;
        IdVector mIdVector;
        
        UknString mCurrentLanguage;
    };

} // namespace ukn

#endif

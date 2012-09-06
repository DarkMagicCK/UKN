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

#include "UKN/Platform.h"

namespace ukn {

    class StringMap {
    public:
        typedef std::map<ukn_string, ukn_string> StringMapCont;
        
        ukn_string stringAtIndex(size_t index) const;
        ukn_string stringAtId(const ukn_string& sid) const;

        void setStringAtId(const ukn_string& sid, const ukn_string& string);
        void addString(const ukn_string& sid, const ukn_string& string);
        void removeStringAtId(const ukn_string& sid);
        void setStringIdAtId(const ukn_string& sid, const ukn_string& after);
        
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
        typedef std::map<ukn_string /* lan */, StringMap /* StringMap */> LanguageMap;
        
        const StringMap& languageStringMapAtId(const ukn_string& lanName) const;
        StringMap& languageStringMapAtId(const ukn_string& lanName);
        
        void addLanguage(const ukn_string& lanName);

        void removeLanguage(const ukn_string& lanName);
        void setCurrentLanguage(const ukn_string& language);
        
        const ukn_string& getCurrentLanguage() const;
        
        ukn_string stringById(const ukn_string& sid) const;
        
        void setStringById(const ukn_string& sid, const ukn_string& string);
        void addString(const ukn_string& sid, const ukn_string& string);
        void addString(const ukn_string& lan, const ukn_string& sid, const ukn_string& string);

        void removeStringById(const ukn_string& sid);
        
        ukn_string getString(const ukn_string& lan, const ukn_string& sid);
        void setString(const ukn_string& lan, const ukn_string& sid, const ukn_string& string);
        void setStringIdAtId(const ukn_string& before, const ukn_string& after);
        
        ukn_string getIdByIndex(size_t index) const;
        
        size_t sizeOfLanguages() const;
        size_t sizeOfStrings() const;
        
        void clear();
    
        bool checkLanguage(const ukn_string& lanName) const;
        bool checkStringId(const ukn_string& stringId) const;
        
        typedef LanguageMap::iterator iterator;
        typedef LanguageMap::const_iterator const_iterator;
        
        iterator begin() { return mLocalizedString.begin(); }
        iterator end()   { return mLocalizedString.end(); }
        
        const_iterator begin() const { return mLocalizedString.begin(); }
        const_iterator end()   const { return mLocalizedString.end(); }
        
    private:
        friend class LocalizedStringTableLoader;
        void updateStringId(const ukn_string& before, const ukn_string& after);
        
        LanguageMap mLocalizedString;
        
        typedef std::vector<ukn_string> IdVector;
        IdVector mIdVector;
        
        ukn_string mCurrentLanguage;
    };

} // namespace ukn

#endif

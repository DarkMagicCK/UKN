/*
 *  lexer.cpp
 *  lScript_lexer
 *
 *  Created by Robert Bu on 10年1月7日.
 *  Copyright 2010 RobertBu(Project L). All rights reserved.
 *
 */

#include "Lexer.h"

#define LEX_STATE_START					0
#define LEX_STATE_INT					1
#define LEX_STATE_FLOAT					2
#define LEX_STATE_IDENT					5

#define LEX_STATE_OP					6
#define LEX_STATE_DELIM					7

#define LEX_STATE_STRING				8
#define LEX_STATE_STRING_ESCAPE			9
#define LEX_STATE_STRING_CLOSE_QUOTE	10

#if !defined(__APPLE__)
#include <malloc.h>
#endif

#include "mist/Resource.h"
#include "mist/Stream.h"
#include "mist/Convert.h"

namespace mist {
    
    
    //determines if a character is numeric
    inline bool isCharNumericLex(wchar_t cChar) {
        if(cChar >= L'0' &&
           cChar <= L'9')
            return true;
        return false;
    }
    
    inline bool isCharNumericSpecial(wchar_t cChar) {
        if(
           // hex
           cChar == L'x'
           )
            return true;
        return false;
    }
    
    //determines if a character is whitespace
    inline bool isCharWhitespaceLex(wchar_t cChar) {
        if(cChar == L' ' ||
           cChar == L'\t' ||
           cChar == L'\n' ||
           cChar == L'\r')
            return true;
        return false;
    }
    
    //determines if a character is ASCII character
    inline bool isCharASCIIChar(wchar_t cChar) {
        // ascii head 0
        return ((cChar&0x80) == 0);
    }
    
    //determines if a charatcter could be part of a valid identifier
    inline bool isCharIdentLex(wchar_t cChar) {
        if((cChar >= L'0' && cChar <= L'9') ||
           (cChar >= L'A' && cChar <= L'Z') ||
           (cChar >= L'a' && cChar <= L'z') ||
           (cChar == L'_') ||
           (!isCharASCIIChar(cChar)))
            return true;
        return false;
    }
    
    //determines if a character is a delimiter
    inline bool isCharDelimiterLex(wchar_t cChar) {
        if(cChar == L',' ||
           cChar == L'(' ||
           cChar == L')' ||
           cChar == L'[' ||
           cChar == L']' ||
           cChar == L';' ||
           cChar == L'{' ||
           cChar == L'}' ||
           cChar == L'.' ||
           cChar == L':')
            return true;
        return false;
    }
    
    
    Lexer::Lexer() {
        reset();
    }
    
    Lexer::~Lexer() {
        
    }
    
    void Lexer::reset() {
        // reset lexer
        currState.iCurrLexemeIndex = currState.iCurrOp = currState.iCurrLine = 0;
        currState.tkTokenSubType = TokenSubTypeNone;
        currState.strCurrLexeme.clear();
        
        usErrorCode = LexNoError;
        strInvalidChar = 0;
        
        contSource.clear();
        contLexerState.clear();
    }
    
    bool Lexer::loadFile(const MistString& filename) {
        ResourcePtr resource = ResourceLoader::Instance().loadResource(filename);
        
        StreamPtr data = resource->readIntoMemory();
        return loadFileMem(0, 0);
    }
    
    bool Lexer::loadFileMem(void* ptr, int64 size) {
        if(ptr) {
            const char* pstrSource = (const char*)ptr;
            unsigned long c = 0;
            
            char cCurrChar;
            MistString sCurrLine;
            while(pstrSource[c] || (size != -1 && c < size)) {
                cCurrChar = pstrSource[c];
                
                // ASCII character
                if(isCharASCIIChar(cCurrChar)) {
                    if(cCurrChar == '\\')
                        cCurrChar = '/';
                    
                    //*nix LF, windows LF+CR
                    //ignore CR
                    if(cCurrChar == '\r') {
                        ++c;
                        continue;
                    }
                    else if(cCurrChar == '\n') {
                        contSource.push_back(sCurrLine);
                        sCurrLine.clear();
                    } else {
                        sCurrLine += cCurrChar;
                    }
                    ++c;
                } else {
                    // just add to source
                    sCurrLine += cCurrChar;
                    ++c;
                }
            }
            contSource.push_back(sCurrLine);
            
            currState.iCurrLexemeIndex = currState.iCurrOp = currState.iCurrLine = 0;
            currState.tkTokenSubType = TokenSubTypeNone;
            currState.strCurrLexeme.clear();
            
            usErrorCode = LexNoError;
            strInvalidChar = 0;
            
            return true;
        }
        usErrorCode = LexFileIOError;
        return false;
    }
    
    Lexer::Token Lexer::errorInvalidChar(wchar_t cCurrChar) {
        strInvalidChar = cCurrChar;
        usErrorCode = LexInvalidInput;
        return TokenInvalidInput;
    }
    
    wchar_t Lexer::getNextChar() {
        if(currState.iCurrLexemeIndex < contSource[currState.iCurrLine].size()) {
            return contSource[currState.iCurrLine][currState.iCurrLexemeIndex++];
        } else {
            //	printf("line %d/%d, char %d/%d, %s :", iCurrLine, contSource.size(), iCurrLexemeIndex, contSource[iCurrLine].size(), contSource[iCurrLine].c_str());
            currState.iCurrLexemeIndex = 0;
            ++currState.iCurrLine;
            return L'\n';
        }
    }
    
    void Lexer::rewindChar() {
        if(currState.iCurrLexemeIndex > 0) {
            --currState.iCurrLexemeIndex;
        }
    }
    
    wchar_t Lexer::getLookAheadChar() {
        if(currState.iCurrLexemeIndex < contSource[currState.iCurrLine].size()-1) {
            return contSource[currState.iCurrLine][currState.iCurrLexemeIndex+1];
        } else {
            return 0;
        }
    }
    
    Lexer::Token Lexer::toNextToken() {
        if(usErrorCode == LexNoError) return getNextToken();
        else if(usErrorCode == LexInvalidInput) {
            rewindChar();
            wchar_t cCurrChar = getNextChar();
            while(!(isCharWhitespaceLex(cCurrChar) ||
                    isCharDelimiterLex(cCurrChar) ||
                    isCharIdentLex(cCurrChar) ||
                    operators.checkChar(cCurrChar))) {
                cCurrChar = getNextChar();
            }
            usErrorCode = LexNoError;
            rewindChar();
            return getNextToken();
        }
        return TokenEndOfStream;
    }
    
    Lexer::Token Lexer::getNextToken() {
        if(usErrorCode != LexNoError) return TokenInvalidInput;
        
        if(currState.iCurrLine >= contSource.size())
            return TokenEndOfStream;
        
        currState.tkTokenSubType = TokenSubTypeNone;
        currState.strCurrLexeme.clear();
        
        if(currState.iCurrLine != -1)
            contLexerState.push_back(currState);
        
        int iCurrState = LEX_STATE_START;
        bool iLexemeDone = false;
        
        int iCurrOpStateIndex = -1;
        
        wchar_t cCurrChar;
        bool iAddCurrChar;
        
        while(true) {
            if(currState.iCurrLine >= contSource.size())
                return TokenEndOfStream;
            
            cCurrChar = getNextChar();
            if(cCurrChar == L'\0')
                break;
            //	if(cCurrChar == '\n')
            //		continue;
            
            iAddCurrChar = true;
            
            switch (iCurrState) {
                case LEX_STATE_START:
                    if(isCharWhitespaceLex(cCurrChar)) {
                        iAddCurrChar = false;
                    } else if(isCharNumericLex(cCurrChar)) {
                        // octal number begins with '0'
                        if(cCurrChar == L'0')
                            currState.tkTokenSubType = TokenSubtypeOctalInt;
                        
                        iCurrState = LEX_STATE_INT;
                    } else if(cCurrChar == L'.') {
                        if(isCharNumericLex( getLookAheadChar() ))
                            iCurrState = LEX_STATE_FLOAT;
                        else
                            iCurrState = LEX_STATE_DELIM;
                    } else if(isCharIdentLex(cCurrChar)) {
                        iCurrState = LEX_STATE_IDENT;
                    } else if(operators.checkChar(cCurrChar)) {
                        currState.iCurrOp = iCurrOpStateIndex;
                        
                        iCurrState = LEX_STATE_OP;
                    } else if(isCharDelimiterLex(cCurrChar)) {
                        iCurrState = LEX_STATE_DELIM;
                    } else if(cCurrChar == L'"') {
                        iAddCurrChar = false;
                        iCurrState = LEX_STATE_STRING;
                    }
                    else {
                        return errorInvalidChar(cCurrChar);
                    }
                    break;
                    
                case LEX_STATE_INT:
                    if(isCharNumericLex(cCurrChar))
                        iCurrState = LEX_STATE_INT;
                    else if(cCurrChar == L'.') {
                        // no special for float
                        if(currState.tkTokenSubType != TokenSubTypeNone) {
                            return errorInvalidChar(cCurrChar);
                        }
                        iCurrState = LEX_STATE_FLOAT;
                    }
                    else if(isCharNumericSpecial(cCurrChar)) {
                        // determine which kind of number
                        switch (cCurrChar) {
                            case L'x': currState.tkTokenSubType = TokenSubtypeHexInt; break;
                        }
                        iCurrState = LEX_STATE_INT;
                    }
                    else if(isCharWhitespaceLex(cCurrChar) || isCharDelimiterLex(cCurrChar) || operators.checkChar(cCurrChar)) {
                        iAddCurrChar = false;
                        iLexemeDone = true;
                    }
                    else {
                        return errorInvalidChar(cCurrChar);
                    }
                    break;
                    
                case LEX_STATE_FLOAT:
                    if(isCharNumericLex(cCurrChar))
                        iCurrState = LEX_STATE_FLOAT;
                    else if(isCharWhitespaceLex(cCurrChar) || isCharDelimiterLex(cCurrChar) || operators.checkChar(cCurrChar)) {
                        iAddCurrChar = false;
                        iLexemeDone = true;
                    } else if(cCurrChar == L'f') {
                        iAddCurrChar = false;
                    }
                    else {
                        return errorInvalidChar(cCurrChar);
                    }
                    break;
                    
                case LEX_STATE_IDENT:
                    if(isCharWhitespaceLex(cCurrChar) || isCharDelimiterLex(cCurrChar) || operators.checkChar(cCurrChar)) {
                        iAddCurrChar = false;
                        iLexemeDone = true;
                    } else if(isCharIdentLex(cCurrChar)) {
                        iCurrState = LEX_STATE_IDENT;
                    } else {
                        return errorInvalidChar(cCurrChar);
                    }
                    break;
                    
                case LEX_STATE_OP:
                    if((iCurrOpStateIndex = operators.check((currState.strCurrLexeme+cCurrChar))) != -1) {
                        currState.iCurrOp = iCurrOpStateIndex;
                    } else {
                        currState.iCurrOp = operators.check(currState.strCurrLexeme);
                        if(currState.iCurrOp == -1)
                            return errorInvalidChar(cCurrChar);
                        
                        iAddCurrChar = false;
                        iLexemeDone = true;
                    }
                    break;
                    
                case LEX_STATE_DELIM:
                    iAddCurrChar = false;
                    iLexemeDone = true;
                    break;
                    
                case LEX_STATE_STRING:
                    if(cCurrChar == L'"') {
                        iAddCurrChar = false;
                        iCurrState = LEX_STATE_STRING_CLOSE_QUOTE;
                    } else if(cCurrChar == L'\\') {
                        iAddCurrChar = false;
                        iCurrState = LEX_STATE_STRING_ESCAPE;
                    }
                    break;
                    
                case LEX_STATE_STRING_ESCAPE:
                    iCurrState = LEX_STATE_STRING;
                    break;
                    
                case LEX_STATE_STRING_CLOSE_QUOTE:
                    iAddCurrChar = false;
                    iLexemeDone = true;
                    break;
            }
            if(iAddCurrChar) {
                currState.strCurrLexeme += cCurrChar;
            }
            
            if(iLexemeDone)
                break;
        }
        
        if(currState.iCurrLexemeIndex > 0)
            --currState.iCurrLexemeIndex;
        
        Token tokenType;
        switch (iCurrState) {
            case LEX_STATE_INT:
                tokenType = TokenInt;
                break;
                
            case LEX_STATE_FLOAT:
                tokenType = TokenFloat;
                break;
                
            case LEX_STATE_IDENT:
                tokenType = TokenIdent;
                break;
                
            case LEX_STATE_DELIM:
                switch (currState.strCurrLexeme[0]) {
                    case L',': tokenType = TokenDelimComma; break;
                    case L'(': tokenType = TokenDelimOpenParen; break;
                    case L')': tokenType = TokenDelimCloseParen; break;
                    case L'[': tokenType = TokenDelimOpenBrace; break;
                    case L']': tokenType = TokenDelimCloseBrace; break;
                    case L'{': tokenType = TokenDelimOpenCurlyBrace; break;
                    case L'}': tokenType = TokenDelimCloseCurlyBrace; break;
                    case L';': tokenType = TokenDelimSemicolon; break;
                    case L'.': tokenType = TokenDelimPeriod; break;
                    case ':': tokenType = TokenDelimColon; break;
                }
                break;
                
            case LEX_STATE_OP:
                tokenType = TokenOperator;
                break;
                
            case LEX_STATE_STRING_CLOSE_QUOTE:
                tokenType = TokenString;
                break;
                
            default:
                tokenType = TokenEndOfStream;
                break;
        }
        
        return tokenType;
    }
    
    int Lexer::rewindTokenStream() {
        if(contLexerState.size() > 0) {
            currState = contLexerState.back();
            contLexerState.pop_back();
            return true;
        }
        return false;
    }
    
    int32 Lexer::asInt() const {
        return Convert::ToInt32(getCurrLexeme());
    }
    
    bool Lexer::asBool() const {
        return Convert::ToBoolean(getCurrLexeme());
    }
    
    float Lexer::asFloat() const {
        return Convert::ToDouble(getCurrLexeme());
    }
    
    const MistString& Lexer::getCurrLexeme() const {
        return currState.strCurrLexeme;
    }
    
    int Lexer::getCurrOp() const {
        return currState.iCurrOp;
    }
    
    wchar_t Lexer::getCurrInvalidChar() const {
        return strInvalidChar;
    }
    
    Lexer::Token Lexer::getCurrTokenSubtype() const {
        return currState.tkTokenSubType;
    }
    
    size_t Lexer::getCurrCharIndex() const {
        return currState.iCurrLexemeIndex;
    }
    
    size_t Lexer::getCurrLineIndex() const {
        return currState.iCurrLine;
    }
    
    size_t Lexer::getSourceLineCount() const {
        return contSource.size();
    }
    
    size_t Lexer::getSourceSize() const {
        return lSourceSize;
    }
    
    const MistString& Lexer::getCurrLine() const {
        return contSource[currState.iCurrLine];
    }
    
    void Lexer::addOperator(const MistString& op, int ident) {
        operators.add(op, ident);
    }

    
} // namespace sora
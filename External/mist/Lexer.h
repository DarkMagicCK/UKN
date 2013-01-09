/*
 *  lexer.h
 *  lScript_lexer
 *
 *  Created by Robert Bu on 10年1月7日.
 *  Copyright 2010 RobertBu(Project L). All rights reserved.
 *
 */
#ifndef _SORA_LEXER_H_
#define _SORA_LEXER_H_

#include <cstdio>
#include <cstring>

#include <string>
#include <vector>

#include "mist/Platform.h"
#include <map>


namespace mist {
    
    
    class MIST_API Lexer {
    public:
        typedef int32 Token;
        
        /* Token Types */
        typedef enum {
            TokenInvalidInput       = -1, 	// invalid input
            TokenEndOfStream        = 0,	// end of a file
            TokenInt                = 1,	// int number
            TokenFloat              = 2,	// float number
            TokenIdent              = 3,	// a ident
            TokenString             = 4,	// a string
            TokenOperator			= 15,	// operators, see lex_operators
            
            // delimeters
            TokenDelimComma				= 16,	// ,
            TokenDelimOpenParen			= 17,	// (
            TokenDelimCloseParen		= 18,	// )
            TokenDelimOpenBrace			= 19,	// [
            TokenDelimCloseBrace		= 20,	// ]
            TokenDelimOpenCurlyBrace	= 21,	// {
            TokenDelimCloseCurlyBrace	= 22,	// }
            TokenDelimSemicolon         = 23,	// ;
            TokenDelimPeriod			= 24,	// .
            TokenDelimColon				= 25,	// :
        } LexerTokenType;
        
        // token subtypes
        // int token special, hex & binary & octal
        typedef enum {
            TokenSubTypeNone			= 100,
            TokenSubtypeHexInt          = 101,
            TokenSubtypeOctalInt		= 102,
        } LexerTokenSubtype;
        
        /* Error Codes */
        typedef enum {
            LexNoError          = 1,
            LexFileIOError      = -1,
            LexInvalidInput     = -2
        } LexerErrorCode;
        
    public:
        Lexer();
        ~Lexer();
        /*
         reset the lexer to default state
         */
        void reset();
        
        /*
         load a file to lex
         @param, path of the file
         @retval, if succeed, return true
         */
        bool loadFile(const MistString&);
        /*
         load a file from memory
         @param ptr, the pointer to the memory block that contains the file data
         @param size, the size of the memory block, if size == -1, then ptr should be a 0 terminated string
         @retval, if succeed, return true
         */
        bool loadFileMem(void* ptr, int64 size = -1);
        
        /*
         get next token, return TOKEN_TYPE_INVALID_INPUT if there a invalid input met
         @retval, next token
         */
        Token getNextToken();
        /*
         get current token's subtype, see TOKEN_SUBTPYE_*
         if getNextToken() = TOKEN_TYPE_INT, then sub type can be 
         TOKEN_SUBTYPE_INT_OCTAL	- Octal Int 
         TOKEN_SUBTYPE_INT_HEX		- Hex Int
         TOKEN_SUBTYPE_NULL			- Demical Int
         @retval, current token's subtype
         */
        Token getCurrTokenSubtype() const;
        /*
         to next token, when LEXER_INVALID_INPUT error happened, use this function to skip to next token, 
         otherwise the function behaves the same as getNextToken()
         @retval, next token
         */
        Token toNextToken();
        
        /*
         get the lexeme of current token
         @retval, pointer to the lexeme
         */
        const MistString& getCurrLexeme() const;
        
        /*
         convert current lexeme into specified types
         */
        int32 asInt() const;
        bool  asBool() const;
        float asFloat() const;
        
        /*
         get current operator, only available when getNextToken() == TOKEN_TYPE_OP
         @retval, current operator
         */
        int32 getCurrOp() const;
        
        // information APIs
        /*
         get the line count of the source file
         @retval, line count of the source file
         */
        size_t getSourceLineCount() const;
        /*
         get the size of the source file
         @retval, the size of the source file
         */
        size_t getSourceSize() const;
        /*
         get current char index, use when LEX_INVALID_INPUT happens to get the invalid input char index
         @retval, current char index in the line
         */
        size_t getCurrCharIndex() const;
        /*
         get current line index, use when LEX_INVALID_INPUT happens to get the current line number of the source
         @retval, current line index in the source file
         */
        size_t getCurrLineIndex() const;
        /*
         get current line of the source file
         @retval, current line of the source file
         */
        const MistString& getCurrLine() const;
        /*
         get invalid char, use when LEX_INVALID_INPUT happens to get the invalid char, returns 0 if there's no LEX_INVALID_INPUT error happened
         @retval, invalid char
         */
        wchar_t getCurrInvalidChar() const;
        
        /*
         rewind the token stream to previous state
         @retval, succeed or not
         */
        int	rewindTokenStream();
        
        /*
         add a operator ident to parser
         @param op, the string of the operator, eg "++"
         @param id, the id of the operator, see getNextToken() and toNextToken();
         */
        void addOperator(const MistString& op, int id);
    	
    private:
        Lexer(const Lexer&);
        Lexer& operator=(const Lexer&);
        
        inline wchar_t	getNextChar();
        inline void rewindChar();
        inline wchar_t getLookAheadChar();
        Token errorInvalidChar(wchar_t cCurrChar);
		
        wchar_t strInvalidChar;
        unsigned long lSourceSize;
        
        typedef std::vector<MistString> SourceCont;
        SourceCont contSource;
        
    private:
        class LexemeState {
        public:
            int	iCurrOp;
            
            size_t	iCurrLexemeStart;
            size_t	iCurrLexemeIndex;
            size_t	iCurrLine;
            
            Token tkTokenSubType;
            
            MistString strCurrLexeme;
            
            LexemeState(): iCurrOp(-1), iCurrLine(-1), iCurrLexemeIndex(0), iCurrLexemeStart(0), tkTokenSubType(TokenSubTypeNone) {}
        };
        
        class LexerOperators {
        public:
            typedef std::map<MistString, int> OperatorMap;
            typedef std::map<MistString, int>::iterator iterator;
            OperatorMap mOperators;
            
            void add(const MistString& str, int id) {
                this->mOperators[str] = id;
            }
            int check(const MistString& str) {
                iterator it = this->mOperators.begin();
                if((it = this->mOperators.find(str)) != this->mOperators.end()) {
                    return it->second;
                }
                return -1;
            }
            int checkChar(wchar_t cChar) {
                iterator it = mOperators.begin();
                while(it != this->mOperators.end()) {
                    if(it->first[0] == cChar) return 1;
                    ++it;
                }
                return 0;
            }
        };
        
        LexemeState currState;
        typedef std::vector<LexemeState> LexemeStateCont;
        LexemeStateCont contLexerState;
        
        LexerOperators operators;
        short usErrorCode;
    };

}

#endif
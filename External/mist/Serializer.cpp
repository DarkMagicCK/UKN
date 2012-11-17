//
//  Serializer.cpp
//  Mist
//
//  Created by Robert Bu on 12/14/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "mist/Serializer.h"
#include "mist/Stream.h"

#ifndef MIST_WINDOWS
#include <wchar.h>
#endif

namespace mist {
    
    // bool
    template<>
    void SerializeHelper::FromString<bool>(const MistString& str, bool* val) {
        mist_assert(val);
        if((str.size() == 1 && str[0] == L'1') ||
           (str == L"true") ||
           (str == L"yes"))
            *val = true;
        else
            *val = false;
    }
    
    template<>
    MistString SerializeHelper::ToString<bool>(bool* val) {
        if(*val)
            return L"true";
        else
            return L"false";
    }
    
    template<>
    bool SerializeHelper::DefaultValue<bool>() {
        return false;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<bool>() {
        return STI_BOOL;
    }
    
    // int32
    template<>
    void SerializeHelper::FromString<int32>(const MistString& str, int32* val) {
        mist_assert(val);
        *val = (int32)wcstol(str.c_str(), 0, 10);
    }
    
    template<>
    MistString SerializeHelper::ToString<int32>(int32* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    int32 SerializeHelper::DefaultValue<int32>() {
        return 0;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<int32>() {
        return STI_INT;
    }
    
    // int16
    template<>
    void SerializeHelper::FromString<int16>(const MistString& str, int16* val) {
        mist_assert(val);
        *val = (int16)wcstol(str.c_str(), 0, 10);
    }
    
    template<>
    MistString SerializeHelper::ToString<int16>(int16* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    int16 SerializeHelper::DefaultValue<int16>() {
        return 0;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<int16>() {
        return STI_INT16;
    }
    
    // uint16
    template<>
    void SerializeHelper::FromString<uint16>(const MistString& str, uint16* val) {
        mist_assert(val);
        *val = (uint16)wcstol(str.c_str(), 0, 10);
    }
    
    template<>
    MistString SerializeHelper::ToString<uint16>(uint16* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    uint16 SerializeHelper::DefaultValue<uint16>() {
        return 0;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<uint16>() {
        return STI_INT;
    }
    
    // uint32
    template<>
    void SerializeHelper::FromString<uint32>(const MistString& str, uint32* val) {
        mist_assert(val);
        *val = (uint32)wcstol(str.c_str(), 0, 10);
    }
    
    template<>
    MistString SerializeHelper::ToString<uint32>(uint32* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    uint32 SerializeHelper::DefaultValue<uint32>() {
        return 0;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<uint32>() {
        return STI_UINT;
    }
    
    // float
    template<>
    void SerializeHelper::FromString<float>(const MistString& str, float* val) {
        mist_assert(val);
        *val = (float)wcstod(str.c_str(), 0);
    }
    
    template<>
    MistString SerializeHelper::ToString<float>(float* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    float SerializeHelper::DefaultValue<float>() {
        return 0.f;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<float>() {
        return STI_FLOAT;
    }
    
    // double
    template<>
    void SerializeHelper::FromString<double>(const MistString& str, double* val) {
        mist_assert(val);
        *val = (float)wcstod(str.c_str(), 0);
    }
    
    template<>
    MistString SerializeHelper::ToString<double>(double* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    double SerializeHelper::DefaultValue<double>() {
        return 0.0;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<double>() {
        return STI_DOUBLE;
    }
    
    // char
    template<>
    void SerializeHelper::FromString<char>(const MistString& str, char* val) {
        mist_assert(val);
        if(str.length() > 0 && str[0] < 256) {
            *val = (char)(str[0]);
        }
        *val = '?';
    }
    
    template<>
    MistString SerializeHelper::ToString<char>(char* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    char SerializeHelper::DefaultValue<char>() {
        return '0';
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<char>() {
        return STI_CHAR;
    }
    
    // wchar_t
    template<>
    void SerializeHelper::FromString<wchar_t>(const MistString& str, wchar_t* val) {
        mist_assert(val);
        if(str.length() > 0) {
            *val = (char)(str[0]);
        }
        *val = L'?';
    }
    
    template<>
    MistString SerializeHelper::ToString<wchar_t>(wchar_t* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    wchar_t SerializeHelper::DefaultValue<wchar_t>() {
        return L'0';
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<wchar_t>() {
        return STI_WCHAR;
    }
    
    // byte
    template<>
    void SerializeHelper::FromString<uint8>(const MistString& str, uint8* val) {
        mist_assert(val);
        *val = (uint8)wcstol(str.c_str(), 0, 10);
    }
    
    template<>
    MistString SerializeHelper::ToString<uint8>(uint8* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    uint8 SerializeHelper::DefaultValue<uint8>() {
        return 0;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<uint8>() {
        return STI_BYTE;
    }
    
    // long
    template<>
    void SerializeHelper::FromString<long>(const MistString& str, long* val) {
        mist_assert(val);
        *val = wcstol(str.c_str(), 0, 10);
    }
    
    template<>
    MistString SerializeHelper::ToString<long>(long* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    long SerializeHelper::DefaultValue<long>() {
        return 0;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<long>() {
        return STI_ULONG;
    }
    
    // unsigned long
    template<>
    void SerializeHelper::FromString<unsigned long>(const MistString& str, unsigned long* val) {
        mist_assert(val);
        *val = wcstol(str.c_str(), 0, 10);
    }
    
    template<>
    MistString SerializeHelper::ToString<unsigned long>(unsigned long* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    unsigned long SerializeHelper::DefaultValue<unsigned long>() {
        return 0;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<unsigned long>() {
        return STI_ULONG;
    }
    
    // int64
    template<>
    void SerializeHelper::FromString<int64>(const MistString& str, int64* val) {
        mist_assert(val);
        *val = wcstol(str.c_str(), 0, 10);
    }
    
    template<>
    MistString SerializeHelper::ToString<int64>(int64* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    int64 SerializeHelper::DefaultValue<int64>() {
        return 0;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<int64>() {
        return STI_INT64;
    }
    
    // uint64
    template<>
    void SerializeHelper::FromString<uint64>(const MistString& str, uint64* val) {
        mist_assert(val);
        *val = wcstol(str.c_str(), 0, 10);
    }
    
    template<>
    MistString SerializeHelper::ToString<uint64>(uint64* val) {
        return String::AnyToWString(*val);
    }
    
    template<>
    uint64 SerializeHelper::DefaultValue<uint64>() {
        return 0;
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<uint64>() {
        return STI_INT64;
    }
    
    // string
    template<>
    void SerializeHelper::FromString<std::string>(const MistString& str, std::string* val) {
        mist_assert(val);
        *val = String::WStringToString(str);
    }
    
    template<>
    MistString SerializeHelper::ToString<std::string>(std::string* val) {
        return String::StringToWString(*val);
    }
    
    template<>
    std::string SerializeHelper::DefaultValue<std::string>() {
        return std::string();
    }
    
    template<>
    SerializableTypeId SerializeHelper::GetTypeId<std::string>() {
        return STI_STRING;
    }
    
} // namespace mist

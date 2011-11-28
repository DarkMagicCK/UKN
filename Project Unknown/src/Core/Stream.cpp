//
//  Stream.cpp
//  Project Unknown
//
//  Created by Robert Bu on 11/23/11.
//  Copyright (c) 2011 heizi. All rights reserved.
//

#include "UKN/Stream.h"
#include "UKN/MemoryUtil.h"

namespace ukn {
    
    MemoryStream::MemoryStream(): 
    mLength(0), 
    mCurrPos(0), 
    mRealSize(0) {
        
    }
    
	MemoryStream::MemoryStream(const uint8* data, size_t length):
	mCurrPos(0), 
    mLength(length),
    mRealSize(length),
    mData(data, length) {
	}
	
	MemoryStream::~MemoryStream() {
    }
    
    void MemoryStream::clean() {
        mData.clear();
    }
    
    uint8 MemoryStream::operator[](size_t index) {
        ukn_assert(index < mLength);
        
        return mData[index];
    }
	
	void MemoryStream::set(uint8* data, size_t length) {
        mData.set(data, length);
        mCurrPos = 0;
		mLength = length;
        mRealSize = mLength;
	}
	
    void MemoryStream::map(uint8* data, size_t length) {        
        mData.map(data, length);
        mCurrPos = 0;
        mLength = length;
        mRealSize = mLength;
    }
    
	bool MemoryStream::alloc(size_t size) {
		mData.realloc(size, 0);
        
		mLength = size;
		seek(0);
		return true;
	}
	
	void MemoryStream::resize() {
		if(mLength != mRealSize && mRealSize != 0) {
			uint8* tmpData = (uint8*)ukn_malloc(mRealSize);
			memcpy(tmpData, (void*)(get()), mRealSize);
			mLength = mRealSize;
			
            mData.set(tmpData, mLength);
            
            ukn_free(tmpData);
		}
	}
	
	size_t MemoryStream::write(const uint8* data, size_t size) {
		mData.append(data, size);
		return size;
	}
	
	size_t MemoryStream::read(uint8* pv, size_t size) {
		if(!isValid()) return 0;
		if(mCurrPos == mLength) return 0;
		
		assert(pv != NULL);
		
		if(mCurrPos+size > mLength) {
			size = mLength - mCurrPos;
		}
		memcpy(pv, (void*)(get()+mCurrPos), size);
		mCurrPos += size;
		return size;
	}
	
	size_t MemoryStream::read(size_t offset, uint8* pv, size_t size) {
		if(!isValid()) return false;
		
		size_t tempPos = mCurrPos;
		if(!seek(offset))
			seek(0);
		size_t result = read(pv, size);
		seek(tempPos);
		return result;
	}
	
	const uint8* MemoryStream::get() const { 
        return mData.begin();
    }
    
	const uint8* MemoryStream::get(size_t offset) const { 
		if(offset > mLength) 
            offset = 0;
		return (mData.begin()+offset);
	}
	
	bool MemoryStream::seek(size_t pos) {
		if(pos <= mLength) {
			mCurrPos = pos;
			return true;
		}
		return false;
	}
	
	size_t MemoryStream::getSize() const { 
        return mLength;
    }
    
	size_t MemoryStream::getRealsize() const { 
        return mRealSize;
    }
    
	size_t MemoryStream::getPos() const { 
        return mCurrPos; 
    }
    
	bool MemoryStream::isValid() const { 
        return !mData.empty();
    }
    
    StreamPtr FileStreamBasic::readIntoMemory() {
        if(isValid()) {
            uint8* dataBuffer = (uint8*)ukn_malloc(getSize());
            ukn_assert(dataBuffer != 0);
            
            this->seek(0);
            size_t readSize = this->read(dataBuffer, getSize());
            if(readSize != getSize()) {
                // log error
            }
            
            StreamPtr memBuffer = MakeSharedPtr<MemoryStream>();
            // give memory management to MemoryStream
            // faster
            static_cast<MemoryStream*>(memBuffer.get())->set(dataBuffer, readSize);
            
            return memBuffer;
        } else 
            // return null memorystream
            return MakeSharedPtr<MemoryStream>();
    }
    
#ifdef OS_WIN32
    
    FileStreamWin32::FileStreamWin32() {
        file = NULL;
    }
    
    FileStreamWin32::~FileStreamWin32() {
        close();
    }
    
    bool FileStreamWin32::open(const std::string& filename, bool canwrite, bool append, bool nocache) {
        if(file)
            return false;
        
        file = CreateFileA(filename.c_str(), 
                           (canwrite ? GENERIC_WRITE : 0) | GENERIC_READ, FILE_SHARE_READ, NULL, canwrite ? OPEN_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | (nocache ? FILE_FLAG_WRITE_THROUGH : 0), 
                           NULL);
        if(file == NULL || file == INVALID_HANDLE_VALUE) {
            return false;
        }
        if(append) {
            SetFilePointer(file, 0, NULL, FILE_END);
        }
        return true;
    }
    
    bool FileStreamWin32::open(const std::wstring& filename, bool canwrite, bool append, bool nocache) {
        if(file)
            return false;
        
        file = CreateFileW(filename.c_str(), 
                           (canwrite ? GENERIC_WRITE : 0) | GENERIC_READ, FILE_SHARE_READ, NULL, canwrite ? OPEN_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | (nocache ? FILE_FLAG_WRITE_THROUGH : 0), 
                           NULL);
        if(file == NULL || file == INVALID_HANDLE_VALUE) {
            return false;
        }
        if(append) {
            SetFilePointer(file, 0, NULL, FILE_END);
        }
		return true;
    }
    
    void FileStreamWin32::close() {
        if(file != NULL) {
            CloseHandle(file);
            file = NULL;
        }
    }
    
    void FileStreamWin32::truncate() {
        SetEndOfFile(file);
    }
    
    size_t FileStreamWin32::getSize() const {
        DWORD low, high;
        low = GetFileSize(file, &high);
        return (uint64)low + (((uint64)high)<<32);
    }
    
    size_t FileStreamWin32::getPos() const {
        DWORD low;
        LONG high = 0;
        low = SetFilePointer(file, 0, &high, FILE_CURRENT);
        return (uint64)low + ((high)<<32);
    }
    
    void FileStreamWin32::seek(size_t pos, SeekType type) {
        LONG high = (LONG)(pos >> 32);
        SetFilePointer(file, pos & 0xFFFFFFFF, (high > 0 ? &high : NULL), type);
    }
    
    bool FileStreamWin32::isValid() const {
        return file != 0;
    }
    
    bool FileStreamWin32::seek(size_t pos) {
        LONG high = (LONG)(pos >> 32);
        return SetFilePointer(file, pos & 0xFFFFFFFF, (high > 0 ? &high : NULL), type) != INVALID_HANDLE_VALUE;
    }
    
    size_t FileStreamWin32::read(uint8* data, size_t len) {
        DWORD size;
        if(ReadFile(file, data, len, &size, NULL))
            return size;
        return 0;
    }
    
    size_t FileStreamWin32::write(const uint8* data, size_t len) {
        DWORD size;
        if(WriteFile(file, data, len, &size, NULL))
            return size;
        return 0;
    }
    
#else 
    
    FileStreamPosix::FileStreamPosix() {
        nocache = false;
        file = NULL;
    }
    
    FileStreamPosix::~FileStreamPosix() {
        close();
    }
    
    bool FileStreamPosix::open(const ukn_string& filename, bool canwrite, bool append, bool nocache) {
        if(file != NULL)
            return false;
        
        this->nocache = nocache;
        file = fopen(filename.c_str(), canwrite ? (append ? "a+b" : "r+b") : "rb");
        if(file == NULL) {
            if(canwrite)
                file = fopen(filename.c_str(), "w+b");
            if(file == NULL)
                return false;
        }
        return true;
    }
    
    void FileStreamPosix::close() {
        if(file != NULL) {
            fclose(file);
            file = NULL;
        }
    }
    
    void FileStreamPosix::truncate() {
        
    }
    
    size_t FileStreamPosix::getSize() const {
        size_t pos = ftell(file);
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        fseek(file, pos, SEEK_SET);
        return size;
    }
    
    size_t FileStreamPosix::getPos() const {
        return ftell(file);
    }
    
    bool FileStreamPosix::isValid() const {
        return file != 0;
    }
    
    bool FileStreamPosix::seek(size_t pos) {
        return fseek(file, pos, SEEK_SET) == 0 ? true : false;
    }
    
    void FileStreamPosix::seek(size_t pos, SeekType type) {
        fseek(file, pos, type);
    }
    
    size_t FileStreamPosix::read(uint8* data, size_t len) {
        return fread(data, 1, len, file);
    }
    
    size_t FileStreamPosix::write(const uint8* data, size_t len) {
        if(nocache) 
            return fwrite(data, 1, len, file);
        else {
            size_t size = fwrite(data, 1, len, file);
            fflush(file);
            return size;
        }
    }
    
    
#endif // OS_WIN32
    
} // namespace ukn

//
//  ImageWriter.cpp
//  Project Unknown
//
//  Created by Robert Bu on 2/19/13.
//  Copyright (c) 2013 heizi. All rights reserved.
//

#include "mist/Logger.h"
#include "UKN/ImageWriter.h"

#include "libPNG/png.h"

#include <iostream>

namespace ukn {
    
    using namespace mist;
    
    class PNGError {};
    
    static void PNGWarningCallback(png_structp png_ptr,
                                   png_const_charp msg) {
        mist::log_warning(std::string("libPNG Warning: ") + msg);
    }
    
    static void PNGErrorCallback(png_structp png_ptr,
                                 png_const_charp msg) {
        mist::log_error(std::string("libPNG Warning: ") + msg);
        throw PNGError();
    }
    
    static void PNGWriteCallback(png_structp png_ptr, png_bytep data, png_size_t size) {
        Array<uint8>* arr = (Array<uint8>*)png_get_io_ptr(png_ptr);
        arr->insert(arr->end(), data, data + size);
    }
    
    Array<uint8> ImageWriter::WritePNG(uint32* bits, uint32 width, uint32 height, uint32 pitch, bool setbgr, bool needAlpha) {
        png_structp png_ptr = 0;
        png_infop info_ptr = 0;
        
        Array<uint8> output;
        try {
            
            //      create png structs
            png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, PNGErrorCallback, PNGWarningCallback);
            if (png_ptr == 0) return output;
            
            info_ptr = png_create_info_struct(png_ptr);
            if (info_ptr == 0) return output;
            
            //      init i/o
            png_set_write_fn(png_ptr, &output, PNGWriteCallback, 0);
            
            //      setup params
            if ( needAlpha ) {
                png_set_IHDR( png_ptr, info_ptr, width, height,
                             8,
                             PNG_COLOR_TYPE_RGBA,
                             PNG_INTERLACE_NONE,
                             PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
            }
            else {
                png_set_IHDR( png_ptr, info_ptr, width, height,
                             8,
                             PNG_COLOR_TYPE_RGB,
                             PNG_INTERLACE_NONE,
                             PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
            }
            
            png_write_info(png_ptr, info_ptr);
            
            if ( !needAlpha )
                png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);   //      strip alpha
            
            if(setbgr)
                png_set_bgr(png_ptr);   //      switch to little-endian notation
            
            //      writing
            unsigned char* pSrc = (unsigned char*)bits;
            for ( int i = 0; i < height; i++, pSrc += pitch*sizeof(uint32) ) {
                png_write_row( png_ptr, pSrc );
            }
            
            png_write_end(png_ptr, info_ptr);
        }
        catch(PNGError) {
            png_destroy_write_struct(&png_ptr, (info_ptr == 0) ? NULL : &info_ptr);
            return output;
        }
        
        //      cleanup
        png_destroy_write_struct( &png_ptr, (info_ptr == 0) ? NULL : &info_ptr);
        return output;
    }
    
}

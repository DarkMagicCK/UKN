//
//  TimeUpdater.h
//  Project Unknown
//
//  Created by Ruiwei Bu on 1/29/12.
//  Copyright (c) 2012 heizi. All rights reserved.
//

#ifndef Project_Unknown_TimeUpdater_h
#define Project_Unknown_TimeUpdater_h

#include "UKN/MathUtil.h"
#include "UKN/TimeUtil.h"

namespace df {
    
    template<typename T>
    struct TimeUpdater {
        TimeUpdater(T& value):
        mLength(0),
        mCurrent(0),
        mValue(value) {
        }
        
        TimeUpdater(T start, T end, float length, T& val):
        mLength(length),
        mCurrent(0),
        mValue(val),
        mStart(start),
        mEnd(end) {
            mValue = mStart;
        }
        
        void restart() {
            mCurrent = 0.f;
            mValue = mStart;
        }
        
        void restart(T start, T end, float length) {
            mStart = start;
            mEnd = end;
            mLength = length;
            mCurrent = 0;
        }
        
        void update(float dt) {
            if(mLength != 0.f) {
                mCurrent += dt;
                
                if(mCurrent >= mLength) {
                    mCurrent = mLength = 0;
                }
                
                mValue = lerp(mStart, mEnd, percentage());
            }
        }
        
        bool available() const {
            return mLength != 0;
        }
        
        float percentage() const {
            if(mLength != 0)
                return mCurrent / mLength;
            return 1.f;
        }
        
    private:
        float mLength;
        float mCurrent;
        
        T& mValue;
        T mStart;
        T mEnd;
    };
    
}


#endif

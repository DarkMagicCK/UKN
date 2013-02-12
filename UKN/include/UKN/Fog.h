#ifndef PROJECT_UNKNOWN_FOG_H_
#define PROJECT_UNKNOWN_FOG_H_

#include "UKN/PreDeclare.h"

namespace ukn {

    class Fog {
    public:
        Fog();
        ~Fog();

        enum FogType {
            Linear,
            Exponential,
            Exponential2,
        };

    private:
        ukn::Color mColor;
        float mStart, mEnd;
    };

    typedef SharedPtr<Fog> FogPtr;
}

#endif
// --------------------------------------------------------------------------------
// Name        : RectPlacement.h
// Description : A class that allocates subrectangles into power-of-2 rectangles
//               (C) Copyright 2000-2002 by Javier Arevalo
//               This code is free to use and modify for all purposes
// --------------------------------------------------------------------------------
// modified to support sprite margins and VC8
// (C) Copyright 2008 by Haaf
// --------------------------------------------------------------------------------

// modified by robert bu for mist

#ifndef RECT_PLACEMENT_HEADER
#define RECT_PLACEMENT_HEADER

#include "mist/Platform.h"
#include "mist/MathUtil.h"

#include <vector>

namespace mist {
    
    class MIST_API RectPlacement {
    public:
        typedef std::vector<Vector2>  CPosArray;
        typedef std::vector<Rectangle>  CRectArray;
        
        RectPlacement();
        ~RectPlacement();
        
        void init(int w = 1, int h = 1);
        void end();
        bool isOk() const;
        int  getW() const;
        int  getH() const;
        long getArea() const;
        long getTotalArea() const;
        
        bool addAtEmptySpotAutoGrow (Rectangle& rect, int maxW, int maxH);
        bool addAtEmptySpot (Rectangle &r);

        void setMargin(int32 margin);
        int32 getMargin() const;
        
    private:
        Rectangle   m_size;
        CRectArray  m_vRects;
        CPosArray   m_vPositions;
        long        m_area;
        int32       m_margin;
        
        
        bool isFree                 (const Rectangle &r) const;
        void addPosition            (const Vector2 &p);
        void addRect                (const Rectangle &r);
    };

}

#endif // RECT_PLACEMENT_HEADER
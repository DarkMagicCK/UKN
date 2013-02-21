// ----------------------------------------------------------------------------------------
// Name        : RectPlacement.cpp
// Description : A class that fits subrectangles into a power-of-2 rectangle
//               (C) Copyright 2000-2002 by Javier Arevalo
//               This code is free to use and modify for all purposes
// ----------------------------------------------------------------------------------------
// modified to support sprite margins and VC8
// (C) Copyright 2008 by Haaf
// --------------------------------------------------------------------------------

/*
 You have a bunch of rectangular pieces. You need to arrange them in a
 rectangular surface so that they don't overlap, keeping the total area of the
 rectangle as small as possible. This is fairly common when arranging characters
 in a bitmapped font, lightmaps for a 3D engine, and I guess other situations as
 well.
 
 The idea of this algorithm is that, as we add rectangles, we can pre-select
 "interesting" places where we can try to add the next rectangles. For optimal
 results, the rectangles should be added in order. I initially tried using area
 as a sorting criteria, but it didn't work well with very tall or very flat
 rectangles. I then tried using the longest dimension as a selector, and it
 worked much better. So much for intuition...
 
 These "interesting" places are just to the right and just below the currently
 added rectangle. The first rectangle, obviously, goes at the top left, the next
 one would go either to the right or below this one, and so on. It is a weird way
 to do it, but it seems to work very nicely.
 
 The way we search here is fairly brute-force, the fact being that for most off-
 line purposes the performance seems more than adequate. I have generated a
 japanese font with around 8500 characters and all the time was spent generating
 the bitmaps.
 
 Also, for all we care, we could grow the parent rectangle in a different way
 than power of two. It just happens that power of 2 is very convenient for
 graphics hardware textures.
 
 I'd be interested in hearing of other approaches to this problem. Make sure
 to post them on http://www.flipcode.com
 */

#include "mist/RectPlacement.h"

namespace mist {
    
    RectPlacement::RectPlacement():
    m_margin(1) {
        
    }
    
    RectPlacement::~RectPlacement() {
        
    }
    
    bool RectPlacement::isOk() const {
        return m_size.width() > 0;
    }
    
    int  RectPlacement::getW() const {
        return m_size.width();
    }
    
    int  RectPlacement::getH() const {
        return m_size.height();
    }
    
    long RectPlacement::getArea () const {
        return m_area;
    }
    
    long RectPlacement::getTotalArea () const {
        return m_size.width() * m_size.height();
    }
    
    void RectPlacement::setMargin(int32 margin) {
        m_margin = margin;
    }
    
    int32 RectPlacement::getMargin() const {
        return m_margin;
    }
    
    void RectPlacement::init(int w, int h) {
        end();
        
        m_size = Rectangle(0, 0, w, h, true);
        m_vPositions.push_back(Vector2(m_margin, m_margin));
        m_area = 0;
    }
    
    void RectPlacement::end() {
        m_vPositions.clear();
        m_vRects.clear();
        m_size.x1 = m_size.x2 = 0;
    }
    
    bool RectPlacement::isFree (const Rectangle &r) const {
        if (!m_size.contains(r, m_margin))
            return false;
        for (CRectArray::const_iterator it = m_vRects.begin();
             it != m_vRects.end();
             ++it)
            if (it->intersect(r))
                return false;
        return true;
    }
    
    void RectPlacement::addPosition(const Vector2 &p)
    {
        // Try to insert anchor as close as possible to the top left corner
        // So it will be tried first
        bool bFound = false;
        CPosArray::iterator it;
        for (it = m_vPositions.begin();
             !bFound && it != m_vPositions.end();
             ++it) {
            if (p.x() + p.y() < it->x() + it->y())
                bFound = true;
        }
        if (bFound)
            m_vPositions.insert(it, p);
        else
            m_vPositions.push_back(p);
    }
    
    void RectPlacement::addRect(const Rectangle &r) {
        m_vRects.push_back(r);
        m_area += r.width() * r.height();
        
        // Add two new anchor points
        addPosition(Vector2(r.x(), r.y() + r.height() + m_margin));
        addPosition(Vector2(r.x() + r.width() + m_margin, r.y()));
    }
   
    bool RectPlacement::addAtEmptySpot(Rectangle &r) {
        // Find a valid spot among available anchors.
        
        bool bFound = false;
        CPosArray::iterator it;
        for (it = m_vPositions.begin();
             !bFound && it != m_vPositions.end();
             ++it) {
            Rectangle rect(it->x(), it->y(), r.width(), r.height());
            
            if (isFree(rect)) {
                r = rect;
                bFound = true;
                break; // Don't let the loop increase the iterator.
            }
        }
        if (bFound) {
            // Remove the used anchor point
            m_vPositions.erase(it);
            
            // Sometimes, anchors end up displaced from the optimal position
            // due to irregular sizes of the subrects.
            // So, try to adjut it up & left as much as possible.
            
            int x,y;
            
            for (x = 1; x <= r.x(); x++)
                if (!isFree(Rectangle(r.x() - x, r.y(), r.width(), r.height())))
                    break;
            for (y = 1; y <= r.y(); y++)
                if (!isFree(Rectangle(r.x(), r.y() - y, r.width(), r.height())))
                    break;
            if (y > x)
                r.y1 -= y-1;
            else
                r.x1 -= x-1;
            addRect(r);
        }
        return bFound;
    }
    
    bool RectPlacement::addAtEmptySpotAutoGrow(Rectangle &rect, int maxW, int maxH)
    {
        if (rect.width() <= 0)
            return true;
        
        int orgW = m_size.width();
        int orgH = m_size.height();
        
        // Try to add it in the existing space
        while (!addAtEmptySpot(rect)) {
            int pw = m_size.width();
            int ph = m_size.height();
            
            // Sanity check - if area is complete.
            if (pw >= maxW && ph >= maxH)
            {
                m_size.x2 = m_size.x1 + orgW;
                m_size.y2 = m_size.y1 + orgH;
                return false;
            }
            
            // Try growing the smallest dim
            if (pw < maxW && (pw < ph || ((pw == ph) && (rect.width() >= rect.height()))))
                m_size.x2 = m_size.x1 + pw*2;
            else
                m_size.y2 = m_size.y1 + ph*2;
            if (addAtEmptySpot(rect))
                break;
            
            // Try growing the other dim instead
            if (pw != m_size.width()) {
                m_size.x2 = m_size.x1 + pw;
                if (ph < maxW)
                    m_size.y2 = m_size.y1 + ph*2;
            }
            else {
                m_size.y2 = m_size.y1 + ph;
                if (pw < maxW)
                    m_size.x2 = m_size.x1 + pw*2;
            }
            
            if (pw != m_size.width() || ph != m_size.height())
                if (addAtEmptySpot(rect))
                    break;
            
            // Grow both if possible, and reloop.
            m_size.x2 = m_size.x1 + pw;
            m_size.y2 = m_size.y1 + ph;
            if (pw < maxW)
                m_size.x2 = m_size.x1 + pw*2;
            if (ph < maxH)
                m_size.y2 = m_size.y1 + ph*2;
        }
        return true;
    }

}
    
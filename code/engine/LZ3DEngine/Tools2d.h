#pragma once
#include <Windows.h>

#define getXLParam(lp)   ((int)(short)LOWORD(lp))
#define getYLParam(lp)   ((int)(short)HIWORD(lp))

class CPoint : public POINT
{
public:
    CPoint()
    {
        set(0,0);
    }

    CPoint(const CPoint & pt)
    {
        set(pt.x, pt.y);
    }
    CPoint(const POINT * pt) 
    {
        set(pt->x, pt->y);
    }
    CPoint(int x_ , int y_ )
    {
        set(x_, y_);
    }

    CPoint(LPARAM lParam)
    {
        formatLParam(lParam);
    }

    void set(int x_ , int y_ )
    {
        x = x_ ;
        y = y_ ;
    }

    CPoint & operator=(const CPoint & pt)
    {
        x = pt.x;
        y = pt.y;
        return (*this);
    }
    CPoint & operator=(const POINT & pt)
    {
        x = pt.x;
        y = pt.y;
        return (*this);
    }
    bool operator==(const CPoint & pt)
    {
        return (x==pt.x && y==pt.y);
    }
    bool operator==(const POINT & pt)
    {
        return (x==pt.x && y==pt.y);
    }
    CPoint operator+=(const CPoint & pt)
    {
        x += pt.x;
        y += pt.y;
        return (*this);
    }
    CPoint operator-=(const CPoint & pt)
    {
        x -= pt.x;
        y -= pt.y;
        return (*this);
    }
    CPoint operator-(const CPoint & pt)
    {
        return CPoint(x-pt.x, y-pt.y);
    }
    CPoint operator+(const CPoint & pt)
    {
        return CPoint(x+pt.x, y+pt.y);
    }

    void formatLParam(LPARAM lparam)
    {
        x = getXLParam(lparam);
        y = getYLParam(lparam);
    }
    LPARAM toLParam(void)
    {
        LPARAM l = MAKELPARAM(x, y);
        return l;
    }
};

class CSize : public SIZE
{
public:
    CSize(void)
    {
        cx = cy = 0;
    }
    CSize(int w,int h)
    {
        cx = w;
        cy = h;
    }
    CSize(const CSize & sz)
    {
        cx = sz.cx;
        cy = sz.cy;
    }
    CSize(CSize * psz)
    {
        cx = psz->cx;
        cy = psz->cy;
    }

    void set(int cx_, int cy_)
    {
        cx = cx_;
        cy = cy_;
    }

    CSize & operator=(const CSize & sz)
    {
        set(sz.cx, sz.cy);
        return (*this);
    }

    CSize & operator=(const SIZE & sz)
    {
        set(sz.cx, sz.cy);
        return (*this);
    }

    bool operator==(const CSize & sz)
    {
        return (cx==sz.cx && cy==sz.cy);
    }

    bool operator==(const SIZE & sz)
    {
        return (cx==sz.cx && cy==sz.cy);
    }
};

class CRect : public RECT
{
public:
    CRect(void)
    {
        left = top = right = bottom = 0;
    };
    CRect(int left_,int top_,int right_,int bottom_ )
    {
        set(left_, top_, right_, bottom_);
    }
    CRect(const RECT & rc)
    {
        *this = rc;
    }
    CRect(RECT * rc)
    {
        *this = *rc;
    }

    CRect(CPoint pt1, CPoint pt2)
    {
        set(pt1.x, pt1.y, pt2.x, pt2.y);
    }

    CRect(CPoint pt1, CSize size)
    {
        set(pt1.x, pt1.y, pt1.x+size.cx, pt1.y+size.cy);
    }

    void set(int left_,int top_,int right_,int bottom_ )
    {
        left = left_ ;
        right = right_ ;
        top = top_ ;
        bottom = bottom_ ;
    }
    CRect & operator=(const RECT & rc)
    {
        left = rc.left;
        top = rc.top;
        right = rc.right;
        bottom = rc.bottom;
        return (*this);
    }

    CRect & operator=(const CRect & rc)
    {
        left = rc.left;
        top = rc.top;
        right = rc.right;
        bottom = rc.bottom;
        return (*this);
    }

    bool isIn(const CPoint & pt)
    {
        return isIn(pt.x, pt.y);
    }

    bool isIn(const POINT *pt)
    {
        return isIn(pt->x,pt->y);
    }

    bool isIn(int x,int y)
    {
        if (x>=left && x<=right && y>=top && y<=bottom)
        {
            return true;
        }
        else return false;
    }
    bool isIntersect(RECT * rc)
    {
        if (right<rc->left  || left>rc->right 
            || bottom<rc->top || top>rc->bottom)
        {
            return false;
        }
        else return true;
    }

    int width(void){ return right-left; }
    int height(void){ return bottom-top; }

    RECT toRect(void)
    {
        RECT rc;
        rc.left = left;
        rc.right = right;
        rc.top = top;
        rc.bottom = bottom;
        return rc;
    }

    /** 偏移。*/
    void offset(int x, int y)
    {
        left += x;
        right += x;
        top += y;
        bottom += y;
    }

    /** 扩展或收缩*/
    void expand(int x, int y)
    {
        left -= x;
        right += x;
        top -= y;
        bottom += y;
    }
};

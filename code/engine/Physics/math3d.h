#pragma once

#include "PhysicsConfig.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <assert.h>

namespace Physics
{

const float MAX_FLOAT = 1e30f;
const float MIN_FLOAT = -MAX_FLOAT;
const float MIN_DIST = 0.001f;
const float MIN_DIST_SQ = MIN_DIST * MIN_DIST;

typedef D3DXMATRIX Matrix4x4;

enum SIDE
{
    SIDE_ON     = 0,
    SIDE_FRONT  = 1,
    SIDE_BACK   = 2,
    SIDE_BOTH   = 3,
    SIDE_FORCE_DWORD = 0x7fffffff
};

enum AXIS_INDEX
{
    AXIS_X,
    AXIS_Y,
    AXIS_Z
};



inline float min2(float a, float b)
{
    return a < b ? a : b;
}

inline float max2(float a, float b)
{
    return a > b ? a : b;
}

inline bool amostZero(float d, float epsilon=0.0004f)
{
    return d>-epsilon && d<epsilon;
}


//////////////////////////////////////////////////////////////////////////
//2维向量
//////////////////////////////////////////////////////////////////////////
class Vector2 : public D3DXVECTOR2
{
public:
    Vector2() : D3DXVECTOR2(0.0f, 0.0f) {}

    Vector2(float x_, float y_) : D3DXVECTOR2(x_, y_) {}

    void init(float x_, float y_)
    {
        x = x_;
        y = y_;
    }

    Vector2 operator+(const Vector2 & r)
    {
        return Vector2(x + r.x, y + r.y);
    }

    Vector2 operator-(const Vector2 & r)
    {
        return Vector2(x - r.x, y - r.y);
    }
};

//////////////////////////////////////////////////////////////////////////
// 3D vector
//////////////////////////////////////////////////////////////////////////
class Vector3 : public D3DXVECTOR3
{
public:
    Vector3()
    {}

    Vector3(float fx, float fy, float fz)
        :D3DXVECTOR3(fx, fy, fz)
    {}

    Vector3(const D3DXVECTOR3 & v)
        : D3DXVECTOR3(v.x, v.y, v.z)
    {}

    Vector3(const Vector3 & v)
        : D3DXVECTOR3(v.x, v.y, v.z)
    {}

    inline bool isZero() const
    {
        return x==0.0f && y==0.0f && z==0.0f;
    }

    inline void zero()
    { 
        x = 0.0f; y=0.0f; z=0.0f;
    }

    //求负
    inline void negative()
    {
        x = -x; y = -y; z=-z;
    }

    inline void set(float fx, float fy, float fz) 
    { 
        x = fx; y = fy; z = fz;
    }

    float lengthSq() const
    {
        return D3DXVec3LengthSq(this);
    }

    float length() const
    {
        return D3DXVec3Length(this);
    }

    float distToSq(const Vector3 & p) const
    {
        return D3DXVec3LengthSq(&(*this - p));
    }

    float distTo(const Vector3 & p) const
    {
        return D3DXVec3Length(&(*this - p));
    }

    void normalize()
    {
        D3DXVec3Normalize(this, this);
    }

    void normalize(Vector3 & out) const
    {
        D3DXVec3Normalize(&out, this);
    }
    
    //点乘
    float dot(const Vector3& v) const
    {
        return (*this) * v;
    }

    //差乘
    Vector3 cross(const Vector3& v) const
    {
        Vector3 temp;
        D3DXVec3Cross(&temp, this, &v);
        return temp;
    }

    //差乘
    void cross(Vector3 & out, const Vector3& v) const
    {
        D3DXVec3Cross(&out, this, &v);
    }

    void toD3DXVec3(D3DXVECTOR3 & left) const
    {
        left.x = x;
        left.y = y;
        left.z = z;
    }

    void applyMatrix(const Matrix4x4 & mat)
    {
        D3DXVec3TransformCoord(this, this, &mat);
    }

    void applyNormalMatrix(const Matrix4x4 & mat)
    {
        D3DXVec3TransformNormal(this, this, &mat);
    }

    void applyMatrix(Vector3 & out, const Matrix4x4 & mat) const
    {
        D3DXVec3TransformCoord(&out, this, &mat);
    }

    void applyNormalMatrix(Vector3 & out, const Matrix4x4 & mat) const
    {
        D3DXVec3TransformNormal(&out, this, &mat);
    }

    bool tooClose(const Vector3 & v) const
    {
        return distToSq(v) <= MIN_DIST_SQ;
    }

    void projectTo2D(Vector2 & p) const;

    void projectTo2D(int & u, int & v) const;

	int maxAxis() const;

    float minValue() const
    {
        return min2(x, min2(y, z));
    }

    //////////////////////////////////////////////////////////////////////////
    ///运算符重载
    //////////////////////////////////////////////////////////////////////////
    
    Vector3 operator - (const Vector3& v) const
    {
        return Vector3(x - v.x, y - v.y, z - v.z) ;
    }

    Vector3 operator-() const
    {
        return Vector3(-x, -y, -z);
    }

    Vector3 operator + (const Vector3& v) const
    {
        return Vector3(x + v.x, y + v.y, z + v.z) ;
    }

    float operator * (const Vector3& v) const
    {
        return x*v.x + y*v.y + z*v.z;
    }

    Vector3 operator * (float f) const
    {
        return Vector3(x * f, y * f, z * f) ;
    }

    Vector3 operator / (float f) const
    {
        return Vector3(x / f, y / f, z / f) ;
    }

    const Vector3 & operator=(const Vector3 & r)
    {
        set(r.x, r.y, r.z);
        return *this;
    }

    const Vector3 & operator=(const D3DXVECTOR3 & r)
    {
        set(r.x, r.y, r.z);
        return *this;
    }

    const Vector3 & operator+=(const Vector3 & r)
    {
        x += r.x; y += r.y; z += r.z;
        return *this;
    }

    const Vector3 & operator-=(const Vector3 & r)
    {
        x -= r.x; y -= r.y; z -= r.z;
        return *this;
    }

    const Vector3 & operator*=(float f)
    {
        x *= f; y *= f; z *= f;
        return *this;
    }

    const Vector3 & operator /= (float f)
    {
        x /= f; y /= f; z /= f;
        return *this;
    }

    float & operator[](int index)
    {
        if (index == AXIS_X) return x;
        if (index == AXIS_Y) return y;
        if (index == AXIS_Z) return z;

        assert( 0 && "Vector3 index out of range");
        return x;
    }

    float operator[](int index) const
    {
        if (index == AXIS_X) return x;
        if (index == AXIS_Y) return y;
        if (index == AXIS_Z) return z;

        assert( 0 && "Vector3 index out of range");
        return x;
    }

    friend std::ostream & operator<<(std::ostream & out, const Vector3 & v);
    friend std::istream & operator>>(std::istream & in, Vector3 & v);
};

namespace MathConst
{
    const Vector3 vec3Max = Vector3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
    const Vector3 vec3Min = Vector3(MIN_FLOAT, MIN_FLOAT, MIN_FLOAT);
    const Vector3 vec3Zero = Vector3(0.0f, 0.0f, 0.0f);
    const Vector3 vec3x = Vector3(1.0f, 0.0f, 0.0f);
    const Vector3 vec3y = Vector3(0.0f, 1.0f, 0.0f);
    const Vector3 vec3z = Vector3(0.0f, 0.0f, 1.0f);
}

//////////////////////////////////////////////////////////////////////////
//直线
//////////////////////////////////////////////////////////////////////////
class Line
{
public:
    Line(){}

    Line(const Vector3 & s, const Vector3 & e)
        : start(s), end(e)
    {}

    float length() const
    {
        return start.distTo(end);
    }

    Vector3 direction() const
    {
        Vector3 dir = end - start;
        dir.normalize();
        return dir;
    }

    //线性插值
    Vector3 interpolation(float ratio) const
    {
        return end*ratio + start*(1.0f-ratio);
    }
    
    //线性插值
    Vector3 interpolation(float rs, float re) const
    {
        float down = rs + re;
        if(down == 0.0f)
        {
            return start;
        }
        down = 1.0f/down;
        //return interpolation(rs*down);
        return (end*rs + start*re)*down;
    }

    //点在直线上的投影
    Vector3 project(const Vector3 & point);

    bool intersect(Vector3 & cross, const Line & line);

public:
    Vector3 start;
    Vector3 end;
};

//////////////////////////////////////////////////////////////////////////
//三角形
//////////////////////////////////////////////////////////////////////////
class Triangle
{
public:
    Triangle(){}
    Triangle(const Vector3 & A, const Vector3 & B, const Vector3 & C)
        :a(A), b(B), c(C)
    {}

    Vector3 normal() const
    {
        Vector3 ab = b - a;
        Vector3 bc = c - b;
        Vector3 n = ab.cross(bc);
        n.normalize();
        return n;
    }

    Vector3 & operator[](int i)
    {
        if(i == 0) return a;
        if(i == 1) return b;
        if(i == 2) return c;
        throw std::out_of_range("Triangle index out of range");
    }
    const Vector3 & operator[](int i) const
    {
        if(i == 0) return a;
        if(i == 1) return b;
        if(i == 2) return c;
        throw std::out_of_range("Triangle index out of range");
    }

    bool isPointIn(const Vector3 & p) const;

    bool isLineIntersect(const Line & line, Vector3 * cross=NULL) const;

    bool isSphereIntersect(const Vector3 & center, float radius, Vector3 * cross=NULL) const;
    
    void applyMatrix(const Matrix4x4 & mat)
    {
        a.applyMatrix(mat);
        b.applyMatrix(mat);
        c.applyMatrix(mat);
    }

    bool intersect(const Triangle & triangle, const Vector3 & offset) const;

    bool intersectRay(const Vector3 & start, const Vector3 & dir, float *pDist=NULL) const;

public :
    Vector3 a, b, c;
};

//////////////////////////////////////////////////////////////////////////
//平面
//////////////////////////////////////////////////////////////////////////
class Plane
{
public:
    Plane()  : distance(0.0f)  {}

    Plane(const Vector3 & n, const Vector3 & p)
        : normal(n)
    {
        distance = normal.dot(p);
    }

    Plane(const Vector3 & n, float d) 
        : normal(n), distance(d) 
    {}

    Plane(const Triangle & tri) 
        : normal(tri.normal())
    { 
        distance = normal.dot(tri.a);
    }

    void set(const Vector3 & n, const Vector3 & p)   
    {
        normal = n;
        distance = normal.dot(p);
    }

    float distToPoint(const Vector3 & p) const
    {
        return normal.dot(p) - distance;
    }

    bool isIntersect(const Line & line, Vector3 * cross=NULL) const;

    bool isIntersect(const Vector3 & start, const Vector3 & end, float * pPos=NULL) const;

    bool isSphereIntersect(const Vector3 & center, float radius, Vector3 * cross=NULL);

    int witchSide(const Vector3 & pt) const
    {
        float dt = distToPoint(pt);
        if (amostZero(dt)) return SIDE_ON;
        if (dt > 0) return SIDE_FRONT;
        return SIDE_BACK;
    }

    int witchSide(const Triangle & tri)
    {
        int side = 0;
        side |= witchSide(tri.a);
        side |= witchSide(tri.b);
        side |= witchSide(tri.c);
        return side;
    }

    bool operator==(const Plane & p)
    {
        return normal == p.normal && distance == p.distance;
    }

public:
    Vector3 normal;
    float distance;
};

//////////////////////////////////////////////////////////////////////////
//多边形
//////////////////////////////////////////////////////////////////////////
typedef std::vector<Vector3> PolygonBase;

class Polygon : public PolygonBase
{
public:

    Polygon(): PolygonBase() {}

    Polygon(int size) : PolygonBase(size) {}

    Polygon(const Polygon& p) : PolygonBase(p) {}

    Polygon(const Triangle & tri)
    {
        *this = tri;
    }

    const Polygon & operator=(const Triangle & tri)
    {
        addTriangle(tri);
        return *this;
    }

    void genPanel(Plane & panel) const
    {
        assert(size() >= 3);

        const_iterator it = begin();
        const Vector3 &a = *it++;
        const Vector3 &b = *it++;
        const Vector3 &c = *it;
        Vector3 ab = b - a;
        Vector3 bc = c - b;
        Vector3 n = ab.cross(bc);
        n.normalize();
        panel.set(n, a);
    }

    void addTriangle(const Triangle & tri)
    {
        addPoint(tri.a);
        addPoint(tri.b);
        addPoint(tri.c);
    }

    void split(const Plane & panel, Polygon & frontPoly, Polygon & backPoly) const;

    void splitFront(const Plane & panel, Polygon & frontPoly) const;

    bool choiceFront(const Plane & panel);

    void addPoint(const Vector3 & p);

    void adjudge();

    float minDistToPanel(const Plane & panel);
};


class AABB
{
public:

    Vector3 min;
    Vector3 max;

    void zero()
    {
        min = MathConst::vec3Max;
        max = MathConst::vec3Min;
    }

    //正规化。aabb经过变换后，min可能要大于max。
    void normalization();

    void addPoint(const Vector3 & pt)
    {
        min.x = min2(min.x, pt.x);
        min.y = min2(min.y, pt.y);
        min.z = min2(min.z, pt.z);

        max.x = max2(max.x, pt.x);
        max.y = max2(max.y, pt.y);
        max.z = max2(max.z, pt.z);
    }

    void addTriangle(const Triangle & tri)
    {
        addPoint(tri.a);
        addPoint(tri.b);
        addPoint(tri.c);
    }

    void getCenter(Vector3 & center) const
    {
        center = (max + min) * 0.5f;
    }

    void getDiameter(Vector3 & diameter) const
    {
        diameter = max - min;
    }

    void sub(const AABB & aabb)
    {
        min.x = max2(min.x, aabb.min.x);
        min.y = max2(min.y, aabb.min.y);
        min.z = max2(min.z, aabb.min.z);

        max.x = min2(max.x, aabb.max.x);
        max.y = min2(max.y, aabb.max.y);
        max.z = min2(max.z, aabb.max.z);
    }

    void add(const AABB & aabb)
    {

    }

    bool intersectsRay(const Vector3 & origin, const Vector3 & dir ) const;

    bool intersect(const AABB & aabb) const;

    bool clipLine(Vector3 & start, Vector3 & end ) const;


    void applyMatrix(const Matrix4x4 & mat);
    void applyMatrix(AABB & out, const Matrix4x4 & mat) const;
};



typedef std::vector<int> TriangleIndices;
typedef std::vector<Triangle> TriangleSet;


class FRect
{
public:
    float left, top, right, bottom;

    FRect() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f)
    {
    }

    FRect(float l, float t, float r, float b)
        : left(l), top(t), right(r), bottom(b)
    {
    }

    float width() const { return right - left; }
    float height() const { return bottom - top; }

    bool isIn(float x_, float y_) const
    {
        if(x_ < left || x_ > right) return false;
        if(y_ < top || y_ > bottom) return false;
        return true;
    }

    void expand(const FRect & rc)
    {
        left = min2(left, rc.left);
        top = min2(top, rc.top);
        right = max2(right, rc.right);
        bottom = max2(bottom, rc.bottom);
    }

    void expand(float x, float y)
    {
        left = min2(left, x);
        top = min2(top, y);
        right = max2(right, x);
        bottom = max2(bottom, y);
    }

    void delta(float dx, float dy)
    {
        left -= dx;
        right += dx;
        top -= dy;
        bottom += dy;
    }

    ///正规化。aabb经过变换后，min可能要大于max。
    void normalization();

    ///两个区域是否相交
    bool isIntersect(const FRect & rc) const;

    ///三角形xz所在区域，是否与当前区域相交
    bool isTriangleXZIn(const Triangle & tri) const;
};





}//namespace Physics
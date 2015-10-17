
#include "stdafx.h"

#include "math3d.h"


namespace Physics
{


    inline bool inside(const Vector2 & a, const Vector2 & b)
    {
        return (a.x*b.y - a.y*b.x) >= 0;
    }

    //分割定理求两个2d三角形相交
    bool triangle2DIntersect(Vector2 triA[3], Vector2 triB[3])
    {
        bool signA = inside( triA[1] - triA[0], triA[2] - triA[0] );
        bool signB = inside( triB[1] - triB[0], triB[2] - triB[0] );
        Vector2 segmentA, segmentB;

        for (int i1 = 0, i2 = 2; i1 <= 2; i2 = i1, i1++)
        {
            segmentA = triA[i1] - triA[i2];
            //线段A可以将两个三角形分割开,则不相交
            if (inside( segmentA, triB[0] - triA[i2] ) != signA &&
                inside( segmentA, triB[1] - triA[i2] ) != signA &&
                inside( segmentA, triB[2] - triA[i2] ) != signA) 
            {
                return false;
            }

            segmentB = triB[i1] - triB[i2];
            if (inside( segmentB, triA[0] - triB[i2] ) != signB &&
                inside( segmentB, triA[1] - triB[i2] ) != signB &&
                inside( segmentB, triA[2] - triB[i2] ) != signB)
            {
                return false;
            }
        }
        return true;
    }

    std::ostream & operator<<(std::ostream & out, const Vector3 & v)
    {
        out<<"(" <<v.x <<", " <<v.y <<", " <<v.z <<")";
        return out;
    }

    std::istream & operator>>(std::istream & in, Vector3 & v)
    {
        char ch;
        do
        {
            in >> ch;
        }while(ch != '(');

        in>>v.x>>ch >>v.y>>ch >>v.z>>ch;

        return in;
    }


    //////////////////////////////////////////////////////////////////////////

    void Vector3::projectTo2D(Vector2 & p) const
    {
        float maxD = fabs(x);
        p.init(y, z);

        float t = fabs(y);
        if (maxD < t)
        {
            maxD = t;
            p.init(x, z);
        }

        t = fabs(z);
        if (maxD < t)
        {
            p.init(x, y);
        }
    }

    void Vector3::projectTo2D(int & u, int & v) const
    {
        float maxD = fabs(x);
        u = AXIS_Y;
        v = AXIS_Z;

        float t = fabs(y);
        if (maxD < t)
        {
            maxD = t;
            u = AXIS_X;
            v = AXIS_Z;
        }

        t = fabs(z);
        if (maxD < t)
        {
            u = AXIS_X;
            v = AXIS_Y;
        }

    }

	int Vector3::maxAxis() const
	{
		float maxD = fabs(x);
		int axis = AXIS_X;
		
		float t = fabs(y);
		if (maxD < t)
		{
			maxD = t;
			axis = AXIS_Y;
		}

		t = fabs(z);
		if (maxD < t)
		{
			axis = AXIS_Z;
		}

		return axis;
	}


    //////////////////////////////////////////////////////////////////////////

    Vector3 Line::project(const Vector3 & point)
    {
        Vector3 n = direction(); //直线方向
        Vector3 sp = point - start; //start -> point
        float d = sp*n; //边sp在直线上的投影长度
        Vector3 q = start + n*d; //投影点
        return q;
    }

    bool Line::intersect(Vector3 & cross, const Line & line)
    {
        Vector3 n1 = direction();
        Vector3 n2 = line.direction();
        Vector3 n1xn2 = n1.cross(n2);
        if (n1xn2.isZero()) //直线平行
        {
            return false;
        }
        float dn1xn2 = 1.0f/n1xn2.lengthSq();
        Vector3 p1p2 = line.start - start;

        float d1 = p1p2.cross(n2) * n1xn2 * dn1xn2;

        float d2 = p1p2.cross(n1) * n1xn2 * dn1xn2;

        Vector3 q1 = start + n1*d1;
        Vector3 q2 = line.start + n2*d2;
        if (q1.distToSq(q2) != 0.0f)
        {
            return false;
        }
        cross = q1;
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    // Determine whether point P in triangle ABC
    bool pointinTriangle(const Vector3 & A, const Vector3 & B, const Vector3 & C, const Vector3 & P)
    {
        Vector3 v0 = C - A ;
        Vector3 v1 = B - A ;
        Vector3 v2 = P - A ;

        float dot00 = v0.dot(v0) ;
        float dot01 = v0.dot(v1) ;
        float dot02 = v0.dot(v2) ;
        float dot11 = v1.dot(v1) ;
        float dot12 = v1.dot(v2) ;

        float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01) ;

        float u = (dot11 * dot02 - dot01 * dot12) * inverDeno ;
        if (u < 0 || u > 1) // if u out of range, return directly
        {
            return false ;
        }

        float v = (dot00 * dot12 - dot01 * dot02) * inverDeno ;
        if (v < 0 || v > 1) // if v out of range, return directly
        {
            return false ;
        }

        return u + v <= 1 ;
    }

    bool Triangle::isPointIn(const Vector3 & p) const
    {
        return pointinTriangle(a, b, c, p);
    }

    bool Triangle::isLineIntersect(const Line & line, Vector3 * cross) const
    {
        Plane panel(*this);
        Vector3 point;
        if(!panel.isIntersect(line, &point))
        {
            return false;
        }
        if(isPointIn(point))
        {
            *cross = point;
            return true;
        }
        return false;
    }

    bool Triangle::isSphereIntersect(const Vector3 & center, float radius, Vector3 * cross/*=NULL*/) const
    {
        Plane panel(*this);
        Vector3 pos;
        if (!panel.isSphereIntersect(center, radius, &pos))
        {
            return false;
        }
        if (!isPointIn(pos))
        {
            return false;
        }
        if (cross)
        {
            *cross = pos;
        }
        return true;
    }

    //与三角形划过体碰撞
    bool Triangle::intersect(const Triangle & triangle,
        const Vector3 & offset) const
    {
        //math3d::addDebugDrawTriangle(*this, 0x3fffffff);

        Plane panel(*this);
        //求位移在平面法线上的投影总长度
        float dt = panel.normal * offset;
        if (amostZero(dt))
        {
            return false;
        }

        //求裁剪比例
        float indt = 1.0f/dt;
        float at = panel.distToPoint(triangle.a) * indt;
        float bt = panel.distToPoint(triangle.b) * indt;
        float ct = panel.distToPoint(triangle.c) * indt;

        //不在范围
        if(at<0.0f && bt<0.0f && ct<0.0f)
        {
            return false;
        }
        if(at>1.0f && bt>1.0f && ct>1.0f)
        {
            return false;
        }

        //裁剪三角体得到的三角形
        Triangle clipT(
            triangle.a + offset*at,
            triangle.b + offset*bt,
            triangle.c + offset*ct );

        //接下来判断当前三角形与裁剪得到的三角形是否相交。
        //将三角形投影到2维空间，判断两个2维三角形是否相交。

        int indexU, indexV;
        panel.normal.projectTo2D(indexU, indexV);

        //当前三角形的投影
        Vector2 triA[3] = {
            Vector2(a[indexU], a[indexV]),
            Vector2(b[indexU], b[indexV]),
            Vector2(c[indexU], c[indexV]),
        };

        //裁剪三角形的投影
        Vector2 triB[3] = {
            Vector2(clipT.a[indexU], clipT.a[indexV]),
            Vector2(clipT.b[indexU], clipT.b[indexV]),
            Vector2(clipT.c[indexU], clipT.c[indexV]),
        };

        //math3d::addDebugDrawTriangle(*this, 0x3fffffff);

        if(!triangle2DIntersect(triA, triB))
        {
            return false;
        }

        return true;
    }

    bool Triangle::intersectRay(const Vector3 & start, const Vector3 & dir, float *pDist/*=NULL*/) const
    {
        Plane plane(*this);
        float delta = plane.normal.dot(dir);//夹角的余弦值
        float dist = plane.distToPoint(start);

        //如果起点在正面，方向与法线夹角小于90度时，不相交。
        //如果在反面，同理。
        if (dist * delta > 0)
        {
            return false;
        }

        float length = fabs(dist / delta);
        Vector3 end = start + dir * length;//计算终点

        //判断终点是否落在了三角形中。

        if(isPointIn(end))
        {
            if(pDist) *pDist = length;
            return true;
        }

        return false;
    }


    //////////////////////////////////////////////////////////////////////////

    bool Plane::isIntersect(const Line & line, Vector3 * cross) const
    {
        float distS = distToPoint(line.start);
        float distE = distToPoint(line.end);
        if(distS * distE > 0)
        {
            return false;
        }

        if(cross)
        {
            *cross = line.interpolation(fabs(distS), fabs(distE));
        }
        return true;
    }

    bool Plane::isIntersect(const Vector3 & start, const Vector3 & end, float * pPos) const
    {
        float distS = distToPoint(start);
        float distE = distToPoint(end);
        if(distS * distE > 0)
        {
            return false;
        }

        if(pPos)
        {
            *pPos = fabs(distS)/(fabs(distS) + fabs(distE));
        }
        return true;
    }

    bool Plane::isSphereIntersect(const Vector3 & center, float radius, Vector3 * cross/*=NULL*/)
    {
        float ds = distToPoint(center);
        if (fabs(ds) > radius)
        {
            return false;
        }
        if (cross)
        {
            *cross = center + normal * ds;
        }
        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    void Polygon::addPoint(const Vector3 & p)
    {
        if (empty() || !p.tooClose(back()))
        {
            push_back(p);
            return;
        }
    }

    void Polygon::split(const Plane & panel, Polygon & frontPoly, Polygon & backPoly) const
    {
        if(size() < 3)
        {
            return;
        }

        const Vector3*  pLastPt = &back();
        float lastDist = panel.distToPoint(*pLastPt);
        bool wasFront = lastDist > 0.0f;

        for (const_iterator it=begin(); it!=end(); ++it)
        {
            float dist = panel.distToPoint(*it);
            bool isFront = dist > 0.0f;

            if (wasFront != isFront)
            {
                //插值求割点
                float ratio = fabs(lastDist / (dist - lastDist));
                Vector3 cross = (*pLastPt)*(1-ratio) + (*it)*ratio;
                frontPoly.addPoint(cross);
                backPoly.addPoint(cross);
            }

            if (isFront)
            {
                frontPoly.addPoint(*it);
            }
            else
            {
                backPoly.addPoint(*it);
            }

            pLastPt = &(*it);
            lastDist = dist;
            wasFront = isFront;
        }

        frontPoly.adjudge();
        backPoly.adjudge();
    }

    void Polygon::splitFront(const Plane & panel, Polygon & frontPoly) const
    {
        if(size() < 3)
        {
            return;
        }

        const Vector3*  pLastPt = &back();
        float lastDist = panel.distToPoint(*pLastPt);
        bool wasFront = lastDist >= 0;

        for (const_iterator it=begin(); it!=end(); ++it)
        {
            float dist = panel.distToPoint(*it);
            bool isFront = dist >= 0;

            if (wasFront != isFront)
            {
                //插值求割点
                float ratio = fabs(lastDist / (dist - lastDist));
                Vector3 cross = (*pLastPt)*(1-ratio) + (*it)*ratio;
                frontPoly.addPoint(cross);
            }

            if (isFront)
            {
                frontPoly.addPoint(*it);
            }

            pLastPt = &(*it);
            lastDist = dist;
            wasFront = isFront;
        }
        frontPoly.adjudge();
    }

    bool Polygon::choiceFront(const Plane & panel)
    {
        Polygon frontPoly;
        splitFront(panel, frontPoly);
        swap(frontPoly);
        return !empty();
    }

    void Polygon::adjudge()
    {
        if (size() < 3)
        {
            clear();
            return;
        }
        if (front().tooClose(back()))
        {
            pop_back();
        }
        if (size() < 3)
        {
            clear();
        }
    }

    float Polygon::minDistToPanel(const Plane & panel)
    {
        float minDist = MAX_FLOAT;
        if(empty())
        {
            return minDist;
        }
        for(iterator it = begin(); it!=end(); ++it)
        {
            float dist = panel.distToPoint(*it);
            if (minDist > dist)
            {
                minDist = dist;
            }
        }
        return minDist;
    }

    //////////////////////////////////////////////////////////////////////////

    bool AABB::intersectsRay(const Vector3 & origin, const Vector3 & dir ) const
    {
        Vector3 ptOnPlane;
        Vector3 base(0.0f, 0.0f, 0.0f);

        int couter = 0;

        if(origin[0] <= min[0])
            base[0] = min[0];
        else if(origin[0] >= max[0])
            base[0] = max[0];
        else
        {  
            couter++;
            float offset_min_x = origin[0] - min[0];
            float offset_max_x = max[0] - origin[0];

            if(offset_min_x<offset_max_x)
                base[0] = min[0];
            else
                base[0] = max[0];
        }

        if(origin[1]<=min[1])
            base[1] = min[1];
        else if(origin[1]>=max[1])
            base[1] = max[1];
        else
        { 
            couter++;
            float offset_min_y = origin[1] - min[1];
            float offset_max_y = max[1] - origin[1];

            if(offset_min_y<offset_max_y)
                base[1] = min[1];
            else
                base[1] = max[1];
        }

        if(origin[2]<=min[2])
            base[2] = min[2];
        else if(origin[2]>=max[2])
            base[2] = max[2];
        else
        { 
            couter++;
            float offset_min_z = origin[2] - min[2];
            float offset_max_z = max[2] - origin[2];

            if(offset_min_z<offset_max_z)
                base[2] = min[2];
            else
                base[2] = max[2];
        }

        if (couter ==3)
            return true;

        for (int axis = 0; axis <= 2; axis++)
        {
            if (dir[axis] != 0.f)
            {
                float t = (base[axis] - origin[axis]) / dir[axis];

                if (t > 0.f)
                {
                    ptOnPlane = origin + t * dir;

                    int nextAxis = (axis + 1) % 3;
                    int prevAxis = (axis + 2) % 3;

                    if (min[nextAxis] < ptOnPlane[nextAxis] && ptOnPlane[nextAxis] < max[nextAxis] &&
                        min[prevAxis] < ptOnPlane[prevAxis] && ptOnPlane[prevAxis] < max[prevAxis])
                    {
                        return true;
                    }
                }
            }
        }


        return false;
    }

    bool AABB::clipLine(Vector3 & start, Vector3 & end ) const
    {
        float sMin = 0.0f;
        float eMax = 1.0f;

        Vector3 delta = end - start;

        for (int i=0; i<3; ++i)
        {
            if (delta[i] != 0)
            {
                float dMin = (min[i] - start[i])/delta[i];
                float dMax = (max[i] - start[i])/delta[i];

                if (delta[i] > 0)//射线方向从min指向max
                {
                    sMin = max2(sMin, dMin);
                    eMax = min2(eMax, dMax);
                }
                else//射线方向从max指向min，dMax应该和dMin交换
                {
                    sMin = max2(sMin, dMax);
                    eMax = min2(eMax, dMin);
                }
            }
            else if(start[i] < min[i] || start[i] > max[i]) //起点不在包围盒范围。
                return false;
        }

        end = start + eMax * delta;
        start = start + sMin * delta;

        return true;
    }

    bool AABB::intersect(const AABB & aabb) const
    {
        for (int i=0; i<3; ++i)
        {
            if (max[i] < aabb.min[i] || min[i] > aabb.max[i])
            {
                return false;
            }
        }
        return true;
    }

    void AABB::normalization()
    {
        for(int i=0; i<3; ++i)
        {
            if(min[i] > max[i]) std::swap(min[i], max[i]);
        }
    }

    void AABB::applyMatrix(const Matrix4x4 & mat)
    {
        min.applyMatrix(mat);
        max.applyMatrix(mat);
        normalization();
    }

    void AABB::applyMatrix(AABB & out, const Matrix4x4 & mat) const
    {
        min.applyMatrix(out.min, mat);
        max.applyMatrix(out.max, mat);
        out.normalization();
    }

    //////////////////////////////////////////////////////////////////////////
    
    bool FRect::isIntersect(const FRect & rc) const
    {
        if(left > rc.right) return false;
        if(right < rc.left) return false;
        if(top > rc.bottom) return false;
        if(bottom < rc.top) return false;
        return true;
    }


    bool FRect::isTriangleXZIn(const Triangle & tri) const
    {
        float minx = min2(tri.a.x, min2(tri.b.x, tri.c.x));
        if(minx > right) return false;

        float maxx = max2(tri.a.x, max2(tri.b.x, tri.c.x));
        if(maxx < left) return false;

        float minz = min2(tri.a.z, min2(tri.b.z, tri.c.z));
        if(minz > bottom) return false;

        float maxz = max2(tri.a.z, max2(tri.b.z, tri.c.z));
        if(maxz < top) return false;

        return true;
    }

    ///正规化。aabb经过变换后，min可能要大于max。
    void FRect::normalization()
    {
        if(left > right) std::swap(left, right);
        if(top > bottom) std::swap(top, bottom);
    }
    
}//namespace Physics
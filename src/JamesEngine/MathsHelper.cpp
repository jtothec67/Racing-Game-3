#include "MathsHelper.h"

#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

namespace Maths
{
    glm::vec3 ClosestPointOnTriangle(const glm::vec3& p,
        const glm::vec3& a,
        const glm::vec3& b,
        const glm::vec3& c)
    {
        // Compute vectors from a to the other points and p.
        glm::vec3 ab = b - a;
        glm::vec3 ac = c - a;
        glm::vec3 ap = p - a;

        // Compute dot products
        float d1 = glm::dot(ab, ap);
        float d2 = glm::dot(ac, ap);
        // Check if p is in vertex region outside A.
        if (d1 <= 0.0f && d2 <= 0.0f)
            return a;

        // Check if p is in vertex region outside B.
        glm::vec3 bp = p - b;
        float d3 = glm::dot(ab, bp);
        float d4 = glm::dot(ac, bp);
        if (d3 >= 0.0f && d4 <= d3)
            return b;

        // Check if p is in edge region of AB, and if so, return projection onto AB.
        float vc = d1 * d4 - d3 * d2;
        if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
        {
            float v = d1 / (d1 - d3);
            return a + v * ab;
        }

        // Check if p is in vertex region outside C.
        glm::vec3 cp = p - c;
        float d5 = glm::dot(ab, cp);
        float d6 = glm::dot(ac, cp);
        if (d6 >= 0.0f && d5 <= d6)
            return c;

        // Check if p is in edge region of AC, and if so, return projection onto AC.
        float vb = d5 * d2 - d1 * d6;
        if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
        {
            float w = d2 / (d2 - d6);
            return a + w * ac;
        }

        // Check if p is in edge region of BC, and if so, return projection onto BC.
        float va = d3 * d6 - d5 * d4;
        if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
        {
            float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + w * (c - b);
        }

        // Otherwise, p is inside face region.
        float denom = 1.0f / (va + vb + vc);
        float v = vb * denom;
        float w = vc * denom;
        return a + ab * v + ac * w;
    }

    bool TriBoxOverlap(const glm::vec3 triVerts[3], const glm::vec3& boxHalfSize)
    {
        // The triangle vertices (in box local space: box center at origin)
        const glm::vec3& v0 = triVerts[0];
        const glm::vec3& v1 = triVerts[1];
        const glm::vec3& v2 = triVerts[2];

        // Compute the edge vectors of the triangle
        glm::vec3 e0 = v1 - v0;
        glm::vec3 e1 = v2 - v1;
        glm::vec3 e2 = v0 - v2;

        float fex = std::fabs(e0.x);
        float fey = std::fabs(e0.y);
        float fez = std::fabs(e0.z);
        {
            // Test axis L = cross(e0, (1,0,0)) --> (0, -e0.z, e0.y)
            float p0 = e0.z * v0.y - e0.y * v0.z;
            float p2 = e0.z * v2.y - e0.y * v2.z;
            float minVal = std::min(p0, p2);
            float maxVal = std::max(p0, p2);
            float rad = fey * boxHalfSize.z + fez * boxHalfSize.y;
            if (minVal > rad || maxVal < -rad)
                return false;
        }
        {
            // Test axis L = cross(e0, (0,1,0)) --> (e0.z, 0, -e0.x)
            float p0 = -e0.z * v0.x + e0.x * v0.z;
            float p2 = -e0.z * v2.x + e0.x * v2.z;
            float minVal = std::min(p0, p2);
            float maxVal = std::max(p0, p2);
            float rad = fex * boxHalfSize.z + fez * boxHalfSize.x;
            if (minVal > rad || maxVal < -rad)
                return false;
        }
        {
            // Test axis L = cross(e0, (0,0,1)) --> (-e0.y, e0.x, 0)
            float p0 = e0.y * v0.x - e0.x * v0.y;
            float p2 = e0.y * v2.x - e0.x * v2.y;
            float minVal = std::min(p0, p2);
            float maxVal = std::max(p0, p2);
            float rad = fex * boxHalfSize.y + fey * boxHalfSize.x;
            if (minVal > rad || maxVal < -rad)
                return false;
        }

        // Repeat tests for edge e1
        fex = std::fabs(e1.x); fey = std::fabs(e1.y); fez = std::fabs(e1.z);
        {
            float p0 = e1.z * v0.y - e1.y * v0.z;
            float p1 = e1.z * v1.y - e1.y * v1.z;
            float minVal = std::min(p0, p1);
            float maxVal = std::max(p0, p1);
            float rad = fey * boxHalfSize.z + fez * boxHalfSize.y;
            if (minVal > rad || maxVal < -rad)
                return false;
        }
        {
            float p0 = -e1.z * v0.x + e1.x * v0.z;
            float p1 = -e1.z * v1.x + e1.x * v1.z;
            float minVal = std::min(p0, p1);
            float maxVal = std::max(p0, p1);
            float rad = fex * boxHalfSize.z + fez * boxHalfSize.x;
            if (minVal > rad || maxVal < -rad)
                return false;
        }
        {
            float p0 = e1.y * v0.x - e1.x * v0.y;
            float p1 = e1.y * v1.x - e1.x * v1.y;
            float minVal = std::min(p0, p1);
            float maxVal = std::max(p0, p1);
            float rad = fex * boxHalfSize.y + fey * boxHalfSize.x;
            if (minVal > rad || maxVal < -rad)
                return false;
        }

        // Repeat tests for edge e2
        fex = std::fabs(e2.x); fey = std::fabs(e2.y); fez = std::fabs(e2.z);
        {
            float p0 = e2.z * v0.y - e2.y * v0.z;
            float p1 = e2.z * v1.y - e2.y * v1.z;
            float minVal = std::min(p0, p1);
            float maxVal = std::max(p0, p1);
            float rad = fey * boxHalfSize.z + fez * boxHalfSize.y;
            if (minVal > rad || maxVal < -rad)
                return false;
        }
        {
            float p0 = -e2.z * v0.x + e2.x * v0.z;
            float p1 = -e2.z * v1.x + e2.x * v1.z;
            float minVal = std::min(p0, p1);
            float maxVal = std::max(p0, p1);
            float rad = fex * boxHalfSize.z + fez * boxHalfSize.x;
            if (minVal > rad || maxVal < -rad)
                return false;
        }
        {
            float p0 = e2.y * v0.x - e2.x * v0.y;
            float p1 = e2.y * v1.x - e2.x * v1.y;
            float minVal = std::min(p0, p1);
            float maxVal = std::max(p0, p1);
            float rad = fex * boxHalfSize.y + fey * boxHalfSize.x;
            if (minVal > rad || maxVal < -rad)
                return false;
        }

        // Test overlap in the x, y, and z directions.
        {
            float minVal = std::min({ v0.x, v1.x, v2.x });
            float maxVal = std::max({ v0.x, v1.x, v2.x });
            if (minVal > boxHalfSize.x || maxVal < -boxHalfSize.x)
                return false;
        }
        {
            float minVal = std::min({ v0.y, v1.y, v2.y });
            float maxVal = std::max({ v0.y, v1.y, v2.y });
            if (minVal > boxHalfSize.y || maxVal < -boxHalfSize.y)
                return false;
        }
        {
            float minVal = std::min({ v0.z, v1.z, v2.z });
            float maxVal = std::max({ v0.z, v1.z, v2.z });
            if (minVal > boxHalfSize.z || maxVal < -boxHalfSize.z)
                return false;
        }

        // Finally, test if the plane of the triangle intersects the box.
        glm::vec3 normal = glm::cross(e0, e1);
        float d = -glm::dot(normal, v0);
        float r = boxHalfSize.x * std::fabs(normal.x) +
            boxHalfSize.y * std::fabs(normal.y) +
            boxHalfSize.z * std::fabs(normal.z);
        if (-r > d || d > r)
            return false;

        return true;
    }

    // Computes the distance from point P to the segment AB.
    float DistancePointSegment(const glm::vec3& P, const glm::vec3& A, const glm::vec3& B)
    {
        glm::vec3 AB = B - A;
        float ab2 = glm::dot(AB, AB);
        // If A and B are the same point, return the distance from P to A.
        if (ab2 < std::numeric_limits<float>::epsilon())
            return glm::length(P - A);
        float t = glm::dot(P - A, AB) / ab2;
        t = glm::clamp(t, 0.0f, 1.0f);
        glm::vec3 closest = A + t * AB;
        return glm::length(P - closest);
    }

    // Computes the distance from point P to the triangle defined by (A, B, C).
    // If the projection of P lies inside the triangle, the distance is the perpendicular distance.
    // Otherwise, it is the minimum distance to one of the triangle's edges.
    float DistancePointTriangle(const glm::vec3& P, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C)
    {
        glm::vec3 AB = B - A;
        glm::vec3 AC = C - A;
        glm::vec3 AP = P - A;

        // Compute barycentric coordinates.
        float d00 = glm::dot(AB, AB);
        float d01 = glm::dot(AB, AC);
        float d11 = glm::dot(AC, AC);
        float d20 = glm::dot(AP, AB);
        float d21 = glm::dot(AP, AC);
        float denom = d00 * d11 - d01 * d01;

        // If the triangle is degenerate, fall back to point-segment distance.
        if (std::abs(denom) < std::numeric_limits<float>::epsilon())
            return std::min(DistancePointSegment(P, A, B),
                std::min(DistancePointSegment(P, A, C),
                    DistancePointSegment(P, B, C)));

        float v = (d11 * d20 - d01 * d21) / denom;
        float w = (d00 * d21 - d01 * d20) / denom;

        if (v >= 0.0f && w >= 0.0f && (v + w) <= 1.0f) {
            // P is inside the triangle: return the absolute (perpendicular) distance to the plane.
            glm::vec3 normal = glm::normalize(glm::cross(AB, AC));
            return std::fabs(glm::dot(P - A, normal));
        }
        else {
            // Otherwise, return the minimum distance to one of the triangle's edges.
            float dAB = DistancePointSegment(P, A, B);
            float dAC = DistancePointSegment(P, A, C);
            float dBC = DistancePointSegment(P, B, C);
            return std::min(dAB, std::min(dAC, dBC));
        }
    }

    // Computes the distance between two segments: one from P0 to P1 and the other from Q0 to Q1.
    float DistanceSegmentSegment(const glm::vec3& P0, const glm::vec3& P1,
        const glm::vec3& Q0, const glm::vec3& Q1)
    {
        glm::vec3 u = P1 - P0;
        glm::vec3 v = Q1 - Q0;
        glm::vec3 w = P0 - Q0;
        float a = glm::dot(u, u);         // squared length of u
        float b = glm::dot(u, v);
        float c = glm::dot(v, v);         // squared length of v
        float d = glm::dot(u, w);
        float e = glm::dot(v, w);
        float D = a * c - b * b;
        float sc, sN, sD = D;             // sc = sN / sD
        float tc, tN, tD = D;             // tc = tN / tD

        const float EPSILON = 1e-6f;
        if (D < EPSILON) {
            // The segments are almost parallel.
            sN = 0.0f;
            sD = 1.0f;
            tN = e;
            tD = c;
        }
        else {
            sN = (b * e - c * d);
            tN = (a * e - b * d);
            if (sN < 0.0f) {
                sN = 0.0f;
                tN = e;
                tD = c;
            }
            else if (sN > sD) {
                sN = sD;
                tN = e + b;
                tD = c;
            }
        }
        if (tN < 0.0f) {
            tN = 0.0f;
            if (-d < 0.0f)
                sN = 0.0f;
            else if (-d > a)
                sN = sD;
            else {
                sN = -d;
                sD = a;
            }
        }
        else if (tN > tD) {
            tN = tD;
            if ((-d + b) < 0.0f)
                sN = 0.0f;
            else if ((-d + b) > a)
                sN = sD;
            else {
                sN = (-d + b);
                sD = a;
            }
        }
        sc = (std::abs(sN) < EPSILON ? 0.0f : sN / sD);
        tc = (std::abs(tN) < EPSILON ? 0.0f : tN / tD);

        glm::vec3 dP = w + (sc * u) - (tc * v);
        return glm::length(dP);
    }

    // Computes the minimal distance between a segment (segA, segB) and a triangle (triA, triB, triC).
    // This function first checks if the segment crosses the triangle’s plane inside the triangle,
    // in which case the distance is zero. Otherwise, it computes candidate distances from the segment’s
    // endpoints, from the triangle’s vertices to the segment, and between the segment and each triangle edge.
    float DistanceSegmentTriangle(const glm::vec3& segA, const glm::vec3& segB,
        const glm::vec3& triA, const glm::vec3& triB, const glm::vec3& triC)
    {
        // Compute the triangle's plane normal.
        glm::vec3 edge1 = triB - triA;
        glm::vec3 edge2 = triC - triA;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        // Compute signed distances of the segment endpoints to the triangle's plane.
        float distA = glm::dot(segA - triA, normal);
        float distB = glm::dot(segB - triA, normal);

        // Check if the segment crosses the plane.
        if (distA * distB < 0.0f) {
            // Compute the intersection point with the plane.
            float t = distA / (distA - distB);
            glm::vec3 intersectPoint = segA + t * (segB - segA);

            // Compute barycentric coordinates for the intersection point.
            glm::vec3 v0 = triB - triA;
            glm::vec3 v1 = triC - triA;
            glm::vec3 v2 = intersectPoint - triA;
            float dot00 = glm::dot(v0, v0);
            float dot01 = glm::dot(v0, v1);
            float dot11 = glm::dot(v1, v1);
            float dot02 = glm::dot(v0, v2);
            float dot12 = glm::dot(v1, v2);
            float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
            float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
            float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

            // If the intersection point lies inside the triangle, the minimal distance is zero.
            if (u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f)
                return 0.0f;
        }

        // Otherwise, compute several candidate distances:
        float d1 = DistancePointTriangle(segA, triA, triB, triC);
        float d2 = DistancePointTriangle(segB, triA, triB, triC);
        float d3 = DistancePointSegment(triA, segA, segB);
        float d4 = DistancePointSegment(triB, segA, segB);
        float d5 = DistancePointSegment(triC, segA, segB);
        float d6 = DistanceSegmentSegment(segA, segB, triA, triB);
        float d7 = DistanceSegmentSegment(segA, segB, triB, triC);
        float d8 = DistanceSegmentSegment(segA, segB, triC, triA);

        float minDist = d1;
        minDist = std::min(minDist, d2);
        minDist = std::min(minDist, d3);
        minDist = std::min(minDist, d4);
        minDist = std::min(minDist, d5);
        minDist = std::min(minDist, d6);
        minDist = std::min(minDist, d7);
        minDist = std::min(minDist, d8);

        return minDist;
    }

    bool PointInTriangle(const glm::vec3& P, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) {
        glm::vec3 v0 = C - A;
        glm::vec3 v1 = B - A;
        glm::vec3 v2 = P - A;

        float dot00 = glm::dot(v0, v0);
        float dot01 = glm::dot(v0, v1);
        float dot02 = glm::dot(v0, v2);
        float dot11 = glm::dot(v1, v1);
        float dot12 = glm::dot(v1, v2);

        float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        return (u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f);
    }

    bool GetLineSegmentTriangleIntersection(const glm::vec3& P, const glm::vec3& Q,
        const glm::vec3& T0, const glm::vec3& T1, const glm::vec3& T2,
        glm::vec3& intersection) {
        // Compute the plane for triangle T.
        glm::vec3 normal = glm::normalize(glm::cross(T1 - T0, T2 - T0));
        float dP = glm::dot(P - T0, normal);
        float dQ = glm::dot(Q - T0, normal);

        // If both endpoints are on the same side of the plane, no intersection.
        if (dP * dQ > 0.0f)
            return false;

        // Compute the interpolation factor.
        float t = dP / (dP - dQ);
        if (t < 0.0f || t > 1.0f)
            return false;

        intersection = P + t * (Q - P);

        // Check if the intersection point is inside the triangle.
        return PointInTriangle(intersection, T0, T1, T2);
    }

    void IntersectionCheck(const glm::vec3& P, const glm::vec3& Q,
        const glm::vec3& T0, const glm::vec3& T1, const glm::vec3& T2,
        std::vector<glm::vec3>& intersectionPoints) {
        // If endpoints are inside the triangle, add them.
        if (PointInTriangle(P, T0, T1, T2))
            intersectionPoints.push_back(P);
        if (PointInTriangle(Q, T0, T1, T2))
            intersectionPoints.push_back(Q);

        // Check for a proper intersection along the edge.
        glm::vec3 pt;
        if (GetLineSegmentTriangleIntersection(P, Q, T0, T1, T2, pt))
            intersectionPoints.push_back(pt);
    }


    glm::vec3 CalculateCollisionPoint(const glm::vec3& A0, const glm::vec3& A1, const glm::vec3& A2,
        const glm::vec3& B0, const glm::vec3& B1, const glm::vec3& B2) {
        std::vector<glm::vec3> intersections;

        // Check each edge of triangle A against triangle B.
        IntersectionCheck(A0, A1, B0, B1, B2, intersections);
        IntersectionCheck(A1, A2, B0, B1, B2, intersections);
        IntersectionCheck(A2, A0, B0, B1, B2, intersections);

        // Check each edge of triangle B against triangle A.
        IntersectionCheck(B0, B1, A0, A1, A2, intersections);
        IntersectionCheck(B1, B2, A0, A1, A2, intersections);
        IntersectionCheck(B2, B0, A0, A1, A2, intersections);

        // If no intersection points are found, fall back to the original centroid approach.
        if (intersections.empty()) {
            glm::vec3 centroidA = (A0 + A1 + A2) / 3.0f;
            glm::vec3 centroidB = (B0 + B1 + B2) / 3.0f;
            return (centroidA + centroidB) * 0.5f;
        }
        else {
            // Average the intersection points to compute the collision point.
            glm::vec3 sum(0.0f);
            for (const auto& pt : intersections)
                sum += pt;
            return sum / static_cast<float>(intersections.size());
        }
    }


    float CalculatePenetrationDepth(const glm::vec3& A0, const glm::vec3& A1, const glm::vec3& A2,
        const glm::vec3& B0, const glm::vec3& B1, const glm::vec3& B2)
    {
        // Compute the face normals.
        glm::vec3 normalA = glm::normalize(glm::cross(A1 - A0, A2 - A0));
        glm::vec3 normalB = glm::normalize(glm::cross(B1 - B0, B2 - B0));

        float penetrationA = FLT_MAX;
        {
            // Compute plane constant for triangle A.
            float dA = glm::dot(A0, normalA);
            // Compute signed distances of triangle B vertices to A's plane.
            float dB0 = glm::dot(B0, normalA) - dA;
            float dB1 = glm::dot(B1, normalA) - dA;
            float dB2 = glm::dot(B2, normalA) - dA;
            // If any vertex is behind A's plane, it is penetrating.
            if (dB0 < 0 || dB1 < 0 || dB2 < 0)
            {
                float p0 = dB0 < 0 ? -dB0 : FLT_MAX;
                float p1 = dB1 < 0 ? -dB1 : FLT_MAX;
                float p2 = dB2 < 0 ? -dB2 : FLT_MAX;
                penetrationA = std::min({ p0, p1, p2 });
            }
        }

        float penetrationB = FLT_MAX;
        {
            // Compute plane constant for triangle B.
            float dB = glm::dot(B0, normalB);
            // Compute signed distances of triangle A vertices to B's plane.
            float dA0 = glm::dot(A0, normalB) - dB;
            float dA1 = glm::dot(A1, normalB) - dB;
            float dA2 = glm::dot(A2, normalB) - dB;
            if (dA0 < 0 || dA1 < 0 || dA2 < 0)
            {
                float p0 = dA0 < 0 ? -dA0 : FLT_MAX;
                float p1 = dA1 < 0 ? -dA1 : FLT_MAX;
                float p2 = dA2 < 0 ? -dA2 : FLT_MAX;
                penetrationB = std::min({ p0, p1, p2 });
            }
        }

        // Choose the smaller penetration depth (if neither triangle shows penetration, return 0)
        float penetration = std::min(penetrationA, penetrationB);
        if (penetration == FLT_MAX)
            penetration = 0.0f;

        return penetration;
    }

    bool RayTriangleIntersect(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0,
        const glm::vec3& v1, const glm::vec3& v2, float& t, float& u, float& v)
    {
        const float EPSILON = 1e-6f;
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 h = glm::cross(dir, edge2);
        float a = glm::dot(edge1, h);

        // If a is close to 0, the ray is parallel to the triangle.
        if (a > -EPSILON && a < EPSILON)
            return false;

        float f = 1.0f / a;
        glm::vec3 s = orig - v0;
        u = f * glm::dot(s, h);
        if (u < 0.0f || u > 1.0f)
            return false;

        glm::vec3 q = glm::cross(s, edge1);
        v = f * glm::dot(dir, q);
        if (v < 0.0f || u + v > 1.0f)
            return false;

        t = f * glm::dot(edge2, q);

        // t > EPSILON ensures the intersection is along the ray (not behind the origin).
        return (t > EPSILON);
    }

	//  ----------- TRIANGLE OVERLAP TEST FROM https://gamedev.stackexchange.com/questions/88060/triangle-triangle-intersection-code -----------

    /* some 3D macros */

#define CROSS(dest,v1,v2)                       \
dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) dest[0]=v1[0]-v2[0]; \
dest[1]=v1[1]-v2[1]; \
dest[2]=v1[2]-v2[2]; 

#define SCALAR(dest,alpha,v) dest[0] = alpha * v[0]; \
dest[1] = alpha * v[1]; \
dest[2] = alpha * v[2];

#define CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2) {\
SUB(v1,p2,q1)\
SUB(v2,p1,q1)\
CROSS(N1,v1,v2)\
SUB(v1,q2,q1)\
if (DOT(v1,N1) > 0.0f) return 0;\
SUB(v1,p2,p1)\
SUB(v2,r1,p1)\
CROSS(N1,v1,v2)\
SUB(v1,r2,p1) \
if (DOT(v1,N1) > 0.0f) return 0;\
else return 1; }



/* Permutation in a canonical form of T2's vertices */

#define TRI_TRI_3D(p1,q1,r1,p2,q2,r2,dp2,dq2,dr2) { \
if (dp2 > 0.0f) { \
if (dq2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2) \
else if (dr2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2)\
else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2) }\
else if (dp2 < 0.0f) { \
if (dq2 < 0.0f) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2)\
else if (dr2 < 0.0f) CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2)\
else CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2)\
} else { \
if (dq2 < 0.0f) { \
if (dr2 >= 0.0f)  CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2)\
else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2)\
} \
else if (dq2 > 0.0f) { \
if (dr2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2)\
else  CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2)\
} \
else  { \
if (dr2 > 0.0f) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2)\
else if (dr2 < 0.0f) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2)\
else return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2,N1,N2);\
}}}



    /*
     *
     *  Three-dimensional Triangle-Triangle Overlap Test
     *
     */


    int tri_tri_overlap_test_3d(real p1[3], real q1[3], real r1[3],

        real p2[3], real q2[3], real r2[3])
    {
        real dp1, dq1, dr1, dp2, dq2, dr2;
        real v1[3], v2[3];
        real N1[3], N2[3];

        /* Compute distance signs  of p1, q1 and r1 to the plane of
         triangle(p2,q2,r2) */


        SUB(v1, p2, r2)
            SUB(v2, q2, r2)
            CROSS(N2, v1, v2)

            SUB(v1, p1, r2)
            dp1 = DOT(v1, N2);
        SUB(v1, q1, r2)
            dq1 = DOT(v1, N2);
        SUB(v1, r1, r2)
            dr1 = DOT(v1, N2);

        if (((dp1 * dq1) > 0.0f) && ((dp1 * dr1) > 0.0f))  return 0;

        /* Compute distance signs  of p2, q2 and r2 to the plane of
         triangle(p1,q1,r1) */


        SUB(v1, q1, p1)
            SUB(v2, r1, p1)
            CROSS(N1, v1, v2)

            SUB(v1, p2, r1)
            dp2 = DOT(v1, N1);
        SUB(v1, q2, r1)
            dq2 = DOT(v1, N1);
        SUB(v1, r2, r1)
            dr2 = DOT(v1, N1);

        if (((dp2 * dq2) > 0.0f) && ((dp2 * dr2) > 0.0f)) return 0;

        /* Permutation in a canonical form of T1's vertices */




        if (dp1 > 0.0f) {
            if (dq1 > 0.0f) TRI_TRI_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2)
            else if (dr1 > 0.0f) TRI_TRI_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2)
            else TRI_TRI_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2)
        }
        else if (dp1 < 0.0f) {
            if (dq1 < 0.0f) TRI_TRI_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2)
            else if (dr1 < 0.0f) TRI_TRI_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2)
            else TRI_TRI_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2)
        }
        else {
            if (dq1 < 0.0f) {
                if (dr1 >= 0.0f) TRI_TRI_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2)
                else TRI_TRI_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2)
            }
            else if (dq1 > 0.0f) {
                if (dr1 > 0.0f) TRI_TRI_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2)
                else TRI_TRI_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2)
            }
            else {
                if (dr1 > 0.0f) TRI_TRI_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2)
                else if (dr1 < 0.0f) TRI_TRI_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2)
                else return coplanar_tri_tri3d(p1, q1, r1, p2, q2, r2, N1, N2);
            }
        }
    };



    int coplanar_tri_tri3d(real p1[3], real q1[3], real r1[3],
        real p2[3], real q2[3], real r2[3],
        real normal_1[3], real normal_2[3]) {

        real P1[2], Q1[2], R1[2];
        real P2[2], Q2[2], R2[2];

        real n_x, n_y, n_z;

        n_x = ((normal_1[0] < 0) ? -normal_1[0] : normal_1[0]);
        n_y = ((normal_1[1] < 0) ? -normal_1[1] : normal_1[1]);
        n_z = ((normal_1[2] < 0) ? -normal_1[2] : normal_1[2]);


        /* Projection of the triangles in 3D onto 2D such that the area of
         the projection is maximized. */


        if ((n_x > n_z) && (n_x >= n_y)) {
            // Project onto plane YZ

            P1[0] = q1[2]; P1[1] = q1[1];
            Q1[0] = p1[2]; Q1[1] = p1[1];
            R1[0] = r1[2]; R1[1] = r1[1];

            P2[0] = q2[2]; P2[1] = q2[1];
            Q2[0] = p2[2]; Q2[1] = p2[1];
            R2[0] = r2[2]; R2[1] = r2[1];

        }
        else if ((n_y > n_z) && (n_y >= n_x)) {
            // Project onto plane XZ

            P1[0] = q1[0]; P1[1] = q1[2];
            Q1[0] = p1[0]; Q1[1] = p1[2];
            R1[0] = r1[0]; R1[1] = r1[2];

            P2[0] = q2[0]; P2[1] = q2[2];
            Q2[0] = p2[0]; Q2[1] = p2[2];
            R2[0] = r2[0]; R2[1] = r2[2];

        }
        else {
            // Project onto plane XY

            P1[0] = p1[0]; P1[1] = p1[1];
            Q1[0] = q1[0]; Q1[1] = q1[1];
            R1[0] = r1[0]; R1[1] = r1[1];

            P2[0] = p2[0]; P2[1] = p2[1];
            Q2[0] = q2[0]; Q2[1] = q2[1];
            R2[0] = r2[0]; R2[1] = r2[1];
        }

        return tri_tri_overlap_test_2d(P1, Q1, R1, P2, Q2, R2);

    };



    /*
        Three-dimensional Triangle-Triangle Intersection
     */

    /*
        This macro is called when the triangles surely intersect
        It constructs the segment of intersection of the two triangles
        if they are not coplanar.
     */

#define CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2) { \
SUB(v1,q1,p1) \
SUB(v2,r2,p1) \
CROSS(N,v1,v2) \
SUB(v,p2,p1) \
if (DOT(v,N) > 0.0f) {\
SUB(v1,r1,p1) \
CROSS(N,v1,v2) \
if (DOT(v,N) <= 0.0f) { \
SUB(v2,q2,p1) \
CROSS(N,v1,v2) \
if (DOT(v,N) > 0.0f) { \
SUB(v1,p1,p2) \
SUB(v2,p1,r1) \
alpha = DOT(v1,N2) / DOT(v2,N2); \
SCALAR(v1,alpha,v2) \
SUB(source,p1,v1) \
SUB(v1,p2,p1) \
SUB(v2,p2,r2) \
alpha = DOT(v1,N1) / DOT(v2,N1); \
SCALAR(v1,alpha,v2) \
SUB(target,p2,v1) \
return 1; \
} else { \
SUB(v1,p2,p1) \
SUB(v2,p2,q2) \
alpha = DOT(v1,N1) / DOT(v2,N1); \
SCALAR(v1,alpha,v2) \
SUB(source,p2,v1) \
SUB(v1,p2,p1) \
SUB(v2,p2,r2) \
alpha = DOT(v1,N1) / DOT(v2,N1); \
SCALAR(v1,alpha,v2) \
SUB(target,p2,v1) \
return 1; \
} \
} else { \
return 0; \
} \
} else { \
SUB(v2,q2,p1) \
CROSS(N,v1,v2) \
if (DOT(v,N) < 0.0f) { \
return 0; \
} else { \
SUB(v1,r1,p1) \
CROSS(N,v1,v2) \
if (DOT(v,N) >= 0.0f) { \
SUB(v1,p1,p2) \
SUB(v2,p1,r1) \
alpha = DOT(v1,N2) / DOT(v2,N2); \
SCALAR(v1,alpha,v2) \
SUB(source,p1,v1) \
SUB(v1,p1,p2) \
SUB(v2,p1,q1) \
alpha = DOT(v1,N2) / DOT(v2,N2); \
SCALAR(v1,alpha,v2) \
SUB(target,p1,v1) \
return 1; \
} else { \
SUB(v1,p2,p1) \
SUB(v2,p2,q2) \
alpha = DOT(v1,N1) / DOT(v2,N1); \
SCALAR(v1,alpha,v2) \
SUB(source,p2,v1) \
SUB(v1,p1,p2) \
SUB(v2,p1,q1) \
alpha = DOT(v1,N2) / DOT(v2,N2); \
SCALAR(v1,alpha,v2) \
SUB(target,p1,v1) \
return 1; \
}}}} 


    /*
     *
     *  Two dimensional Triangle-Triangle Overlap Test
     *
     */


     /* some 2D macros */

#define ORIENT_2D(a, b, c)  ((a[0]-c[0])*(b[1]-c[1])-(a[1]-c[1])*(b[0]-c[0]))

#define INTERSECTION_TEST_VERTEX(P1, Q1, R1, P2, Q2, R2) {\
  if (ORIENT_2D(R2,P2,Q1) >= 0.0f)\
    if (ORIENT_2D(R2,Q2,Q1) <= 0.0f)\
      if (ORIENT_2D(P1,P2,Q1) > 0.0f) {\
        if (ORIENT_2D(P1,Q2,Q1) <= 0.0f) return 1; \
        else return 0;} else {\
        if (ORIENT_2D(P1,P2,R1) >= 0.0f)\
          if (ORIENT_2D(Q1,R1,P2) >= 0.0f) return 1; \
          else return 0;\
        else return 0;}\
    else \
      if (ORIENT_2D(P1,Q2,Q1) <= 0.0f)\
        if (ORIENT_2D(R2,Q2,R1) <= 0.0f)\
          if (ORIENT_2D(Q1,R1,Q2) >= 0.0f) return 1; \
          else return 0;\
        else return 0;\
      else return 0;\
  else\
    if (ORIENT_2D(R2,P2,R1) >= 0.0f) \
      if (ORIENT_2D(Q1,R1,R2) >= 0.0f)\
        if (ORIENT_2D(P1,P2,R1) >= 0.0f) return 1;\
        else return 0;\
      else \
        if (ORIENT_2D(Q1,R1,Q2) >= 0.0f) {\
          if (ORIENT_2D(R2,R1,Q2) >= 0.0f) return 1; \
          else return 0; }\
        else return 0; \
    else  return 0; \
 };


#define INTERSECTION_TEST_EDGE(P1, Q1, R1, P2, Q2, R2) { \
if (ORIENT_2D(R2,P2,Q1) >= 0.0f) {\
if (ORIENT_2D(P1,P2,Q1) >= 0.0f) { \
if (ORIENT_2D(P1,Q1,R2) >= 0.0f) return 1; \
else return 0;} else { \
if (ORIENT_2D(Q1,R1,P2) >= 0.0f){ \
if (ORIENT_2D(R1,P1,P2) >= 0.0f) return 1; else return 0;} \
else return 0; } \
} else {\
if (ORIENT_2D(R2,P2,R1) >= 0.0f) {\
if (ORIENT_2D(P1,P2,R1) >= 0.0f) {\
if (ORIENT_2D(P1,R1,R2) >= 0.0f) return 1;  \
else {\
if (ORIENT_2D(Q1,R1,R2) >= 0.0f) return 1; else return 0;}}\
else  return 0; }\
else return 0; }}



    int ccw_tri_tri_intersection_2d(real p1[2], real q1[2], real r1[2],
        real p2[2], real q2[2], real r2[2]) {
        if (ORIENT_2D(p2, q2, p1) >= 0.0f) {
            if (ORIENT_2D(q2, r2, p1) >= 0.0f) {
                if (ORIENT_2D(r2, p2, p1) >= 0.0f) return 1;
                else INTERSECTION_TEST_EDGE(p1, q1, r1, p2, q2, r2)
            }
            else {
                if (ORIENT_2D(r2, p2, p1) >= 0.0f)
                    INTERSECTION_TEST_EDGE(p1, q1, r1, r2, p2, q2)
                else INTERSECTION_TEST_VERTEX(p1, q1, r1, p2, q2, r2)
            }
        }
        else {
            if (ORIENT_2D(q2, r2, p1) >= 0.0f) {
                if (ORIENT_2D(r2, p2, p1) >= 0.0f)
                    INTERSECTION_TEST_EDGE(p1, q1, r1, q2, r2, p2)
                else  INTERSECTION_TEST_VERTEX(p1, q1, r1, q2, r2, p2)
            }
            else INTERSECTION_TEST_VERTEX(p1, q1, r1, r2, p2, q2)
        }
    };


    int tri_tri_overlap_test_2d(real p1[2], real q1[2], real r1[2],
        real p2[2], real q2[2], real r2[2]) {
        if (ORIENT_2D(p1, q1, r1) < 0.0f)
            if (ORIENT_2D(p2, q2, r2) < 0.0f)
                return ccw_tri_tri_intersection_2d(p1, r1, q1, p2, r2, q2);
            else
                return ccw_tri_tri_intersection_2d(p1, r1, q1, p2, q2, r2);
        else
            if (ORIENT_2D(p2, q2, r2) < 0.0f)
                return ccw_tri_tri_intersection_2d(p1, q1, r1, p2, r2, q2);
            else
                return ccw_tri_tri_intersection_2d(p1, q1, r1, p2, q2, r2);

    };
}
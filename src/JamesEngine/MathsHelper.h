#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace Maths
{
	// Given a point 'p' and a triangle defined by vertices a, b, and c, this function returns
	// the closest point on the triangle to p. (Based on algorithms described in "Real-Time
	// Collision Detection" by Christer Ericson.)
    glm::vec3 ClosestPointOnTriangle(const glm::vec3& p,
        const glm::vec3& a,
        const glm::vec3& b,
        const glm::vec3& c);

    // Given a triangle (with vertices already transformed into a coordinate system
    // in which the box is axis-aligned and centered at the origin) and the box half-sizes,
    // returns true if the triangle and box overlap.
    //
    // This implementation is based on the SAT method described in:
    // "A Fast Triangle-Box Overlap Test" by Tomas Akenine-Möller.
    bool TriBoxOverlap(const glm::vec3 triVerts[3], const glm::vec3& boxHalfSize);

    float DistanceSegmentTriangle(const glm::vec3& segA, const glm::vec3& segB,
        const glm::vec3& triA, const glm::vec3& triB, const glm::vec3& triC);

    float DistancePointSegment(const glm::vec3& P, const glm::vec3& A, const glm::vec3& B);

    float DistancePointTriangle(const glm::vec3& P, const glm::vec3& A, const glm::vec3& B, const glm::vec3& C);
    
    float DistanceSegmentSegment(const glm::vec3& P0, const glm::vec3& P1,
        const glm::vec3& Q0, const glm::vec3& Q1);

    glm::vec3 CalculateCollisionPoint(const glm::vec3& A0, const glm::vec3& A1, const glm::vec3& A2,
        const glm::vec3& B0, const glm::vec3& B1, const glm::vec3& B2);

    float CalculatePenetrationDepth(const glm::vec3& A0, const glm::vec3& A1, const glm::vec3& A2,
        const glm::vec3& B0, const glm::vec3& B1, const glm::vec3& B2);

	bool RayTriangleIntersect(const glm::vec3& orig, const glm::vec3& dir, const glm::vec3& v0,
        const glm::vec3& v1, const glm::vec3& v2, float& t, float& u, float& v);


	//  ----------- TRIANGLE OVERLAP TEST FROM https://gamedev.stackexchange.com/questions/88060/triangle-triangle-intersection-code -----------
    //  -- WHICH IS A MODIFIED VERSION OF https://github.com/benardp/contours/blob/master/freestyle/view_map/triangle_triangle_intersection.c --

    /*
 *  Triangle-Triangle Overlap Test Routines
 *  July, 2002
 *  Updated December 2003
 *
 *  This file contains C implementation of algorithms for
 *  performing two and three-dimensional triangle-triangle intersection test
 *  The algorithms and underlying theory are described in
 *
 * "Fast and Robust Triangle-Triangle Overlap Test
 *  Using Orientation Predicates"  P. Guigue - O. Devillers
 *
 *  Journal of Graphics Tools, 8(1), 2003
 *
 *  Several geometric predicates are defined.  Their parameters are all
 *  points.  Each point is an array of two or three real precision
 *  floating point numbers. The geometric predicates implemented in
 *  this file are:
 *
 *    int tri_tri_overlap_test_3d(p1,q1,r1,p2,q2,r2)
 *    int tri_tri_overlap_test_2d(p1,q1,r1,p2,q2,r2)
 *
 *    int tri_tri_intersection_test_3d(p1,q1,r1,p2,q2,r2,
 *                                     coplanar,source,target)
 *
 *       is a version that computes the segment of intersection when
 *       the triangles overlap (and are not coplanar)
 *
 *    each function returns 1 if the triangles (including their
 *    boundary) intersect, otherwise 0
 *
 *
 *  Other information are available from the Web page
 *  http:<i>//www.acm.org/jgt/papers/GuigueDevillers03/
 *
 */

 // modified by Aaron to better detect coplanarity

    typedef float real;

#define ZERO_TEST(x)  (x == 0)
    //#define ZERO_TEST(x)  ((x) > -0.001 && (x) < .001)

#include "stdio.h"

/* function prototype */

    // Returns 0 for no collision, 1 for collision
    int tri_tri_overlap_test_3d(real p1[3], real q1[3], real r1[3],
        real p2[3], real q2[3], real r2[3]);


    int coplanar_tri_tri3d(real  p1[3], real  q1[3], real  r1[3],
        real  p2[3], real  q2[3], real  r2[3],
        real  N1[3], real  N2[3]);


    int tri_tri_overlap_test_2d(real p1[2], real q1[2], real r1[2],
        real p2[2], real q2[2], real r2[2]);

    /* coplanar returns whether the triangles are coplanar
     *  source and target are the endpoints of the segment of
     *  intersection if it exists)
     */

}
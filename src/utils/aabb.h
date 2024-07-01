#pragma once
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>

#include "plane.h"

// Adapted from https://github.com/fstrugar/CDLOD/blob/master/source/BasicCDLOD/MiniMath.h
class AABB {
    enum IntersectionType {
        INSIDE,
        INTERSECT,
        OUTSIDE
    };

    glm::vec3 min;
    glm::vec3 max;

    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}

    glm::vec3 get_center() { return (min + max) * 0.5f; }
    glm::vec3 get_extents() { return max - get_center(); }
    glm::vec3 get_size() { return max - min; }
    glm::vec3* get_corners() {
        glm::vec3 corners[8];
        corners[0] = glm::vec3(min.x, min.y, min.z);
        corners[1] = glm::vec3(min.x, max.y, min.z);
        corners[2] = glm::vec3(max.x, min.y, min.z);
        corners[3] = glm::vec3(max.x, max.y, min.z);
        corners[4] = glm::vec3(min.x, min.y, max.z);
        corners[5] = glm::vec3(min.x, max.y, max.z);
        corners[6] = glm::vec3(max.x, min.y, max.z);
        corners[7] = glm::vec3(max.x, max.y, max.z);
        return corners;
    }

    glm::vec3 get_positive(const glm::vec3& normal) {
        glm::vec3 pos = glm::vec3(0.0f);
        pos.x = normal.x > 0 ? max.x : min.x;
        pos.y = normal.y > 0 ? max.y : min.y;
        pos.z = normal.z > 0 ? max.z : min.z;
        return pos;
    }    
    
    glm::vec3 get_negative(const glm::vec3& normal) {
        glm::vec3 neg = glm::vec3(0.0f);
        neg.x = normal.x < 0 ? max.x : min.x;
        neg.y = normal.y < 0 ? max.y : min.y;
        neg.z = normal.z < 0 ? max.z : min.z;
        return neg;
    }

    bool intersects(const AABB& other) {
        return !((other.max.x < this->min.x) || (other.min.x > this->max.x)
            || (other.max.y < this->min.y) || (other.min.y > this->max.y)
            || (other.max.z < this->min.z) || (other.min.z > this->max.z));
    }

    float min_squared_distance(const glm::vec3 & point) {
        float dist = 0.0f, k = 0.0f;
        k = (point.x < min.x) ? point.x - min.x : point.x - max.x;
        dist += k * k;
        k = (point.y < min.y) ? point.y - min.y : point.y - max.y;
        dist += k * k;
        k = (point.z < min.x) ? point.z - min.z : point.z - max.z;
        dist += k * k;
        return dist;
    }

    float max_squared_distance(const glm::vec3& point) {
        float dist = 0.0f, k = 0.0f;

        k = std::max(fabsf(point.x - min.x), fabsf(point.x - max.x));
        dist += k * k;
        k = std::max(fabsf(point.y - min.y), fabsf(point.y - max.y));
        dist += k * k;
        k = std::max(fabsf(point.z - min.z), fabsf(point.z - max.z));
        dist += k * k;
        return dist;
    }

    bool test_contains_point(const glm::vec3& point) {
        return (min.x <= point.x && max.x >= point.x) &&
               (min.y <= point.x && max.y >= point.y) &&
               (min.z <= point.z && max.z >= point.z);
    }

    IntersectionType test_sphere(const glm::vec3& center, float radius) {
        if(min_squared_distance(center) <= radius * radius)
            return INTERSECT;
        if (max_squared_distance(center) <= radius * radius)
            return INSIDE;
        return OUTSIDE;
    }



    IntersectionType test_frustum(const std::vector<Plane*>& frustum_planes) {
        glm::vec3 center = get_center();
        glm::vec3 size = get_size();
        glm::vec3* corners = get_corners();
        float size_l = glm::length(size);

        // check bounding sphere against all planes
        for (auto plane : frustum_planes) {
            float dist_center = plane->distance(center);
            if (dist_center < -size_l / 2.0f)
                return OUTSIDE;
        }

        int inside_count = 0;
        size_l /= 6.0f; // for some reason this makes the test more precise
        for (auto plane : frustum_planes) {
            int points_in = 9;
            int front = 1;
            for (int i = 0; i < 9; ++i) {
                glm::vec3 point = (i == 8) ? center : corners[i];
                float dist = plane->distance(point);
                if (dist < -size_l) {
                    front = 0;
                    points_in--;
                }
            }
            // if all points were outside the plane, its not inside the frustum
            if (points_in == 0)
                return OUTSIDE;
            // if all points were in front of the plane
            inside_count += front;
        }
        return (inside_count == 6) ? INSIDE : INTERSECT;
    }

    bool test_intersects_ray(const glm::vec3& ray_origin, const glm::vec3& ray_direction, float& distance) {
        float tmin = -FLT_MAX;        // set to -FLT_MAX to get first hit on line
        float tmax = FLT_MAX;		  // set to max distance ray can travel

        float _ray_origin[] = { ray_origin.x, ray_origin.y, ray_origin.z };
        float _ray_direction[] = { ray_direction.x, ray_direction.y, ray_direction.z };
        float _min[] = { min.x, min.y, min.z };
        float _max[] = { max.x, max.y, max.z };

        const float EPSILON = 1e-5f;

        for (int i = 0; i < 3; i++) {
            if (fabsf(_ray_direction[i]) < EPSILON) {
                // Parallel to the plane
                if (_ray_origin[i] < _min[i] || _ray_origin[i] > _max[i])
                    return false;
            } else {
                float ood = 1.0f / _ray_direction[i];
                float t1 = (_min[i] - _ray_origin[i]) * ood;
                float t2 = (_max[i] - _ray_origin[i]) * ood;

                if (t1 > t2) std::swap(t1, t2);
                if (t1 > tmin) tmin = t1;
                if (t2 < tmax) tmax = t2;

                if (tmin > tmax)                
                    return false;
            }
        }
        distance = tmin;
        return true;
    }

    void expand(const glm::vec3& point) {
        max.x = std::max(point.x, max.x);
        max.y = std::max(point.y, max.y);
        max.z = std::max(point.y, max.y);
        min.x = std::min(point.x, min.x);
        min.y = std::min(point.y, min.y);
        min.z = std::min(point.y, min.y);
    }

    void expand(float percentage) {
        glm::vec3 offset = get_size() * percentage;
        min -= offset;
        max += offset;
    }

    #define _IT_CROSS(dest,v1,v2) \
       dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
       dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
       dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
    #define _IT_DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
    #define _IT_SUB(dest,v1,v2) \
       dest[0]=v1[0]-v2[0]; \
       dest[1]=v1[1]-v2[1]; \
       dest[2]=v1[2]-v2[2]; 

    inline bool intersects_tri(const glm::vec3& _orig, const glm::vec3& _dir, const glm::vec3& _vert0, const glm::vec3& _vert1,
        const glm::vec3& _vert2, float& u, float& v, float& dist) {
        const float c_epsilon = 1e-6f;

        float* orig = (float*)&_orig;
        float* dir = (float*)&_dir;
        float* vert0 = (float*)&_vert0;
        float* vert1 = (float*)&_vert1;
        float* vert2 = (float*)&_vert2;

        float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
        float det, inv_det;

        // find vectors for two edges sharing vert0
        _IT_SUB(edge1, vert1, vert0);
        _IT_SUB(edge2, vert2, vert0);

        // begin calculating determinant - also used to calculate U parameter
        _IT_CROSS(pvec, dir, edge2);

        // if determinant is near zero, ray lies in plane of triangle 
        det = _IT_DOT(edge1, pvec);

        // calculate distance from vert0 to ray origin 
        _IT_SUB(tvec, orig, vert0);
        inv_det = 1.0f / det;

        if (det > c_epsilon)
        {
            // calculate U parameter and test bounds 
            u = _IT_DOT(tvec, pvec);
            if (u < 0.0 || u > det)
                return false;

            // prepare to test V parameter 
            _IT_CROSS(qvec, tvec, edge1);

            // calculate V parameter and test bounds 
            v = _IT_DOT(dir, qvec);
            if (v < 0.0 || u + v > det)
                return false;

        }
        else if (det < -c_epsilon)
        {
            // calculate U parameter and test bounds 
            u = _IT_DOT(tvec, pvec);
            if (u > 0.0 || u < det)
                return false;

            // prepare to test V parameter 
            _IT_CROSS(qvec, tvec, edge1);

            // calculate V parameter and test bounds 
            v = _IT_DOT(dir, qvec);
            if (v > 0.0 || u + v < det)
                return false;
        }
        else return false;  // ray is parallel to the plane of the triangle 

        // calculate t, ray intersects triangle 
        dist = _IT_DOT(edge2, qvec) * inv_det;
        u *= inv_det;
        v *= inv_det;

        return dist >= 0;
    }

    static AABB enclose(const AABB& a, const AABB& b)
    {
        glm::vec3 r_min, r_max;

        r_min.x = std::min(a.min.x, b.min.x);
        r_min.y = std::min(a.min.y, b.min.y);
        r_min.z = std::min(a.min.z, b.min.z);

        r_max.x = std::max(a.max.x, b.max.x);
        r_max.y = std::max(a.max.y, b.max.y);
        r_max.z = std::max(a.max.z, b.max.z);

        return AABB(r_min, r_max);
    }

    bool operator == (const AABB& b)
    {
        return min == b.min && max == b.max;
    }

    float bounding_sphere_radius()
    {
        return glm::length(get_size()) * 0.5f;
    }
};
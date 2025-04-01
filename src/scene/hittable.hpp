#ifndef SCENE_HITTABLE_HPP
#define SCENE_HITTABLE_HPP

#include "../rtweekend.hpp"
// #include "material.hpp"

namespace jmrtiow::scene
{
    class material;

    struct hit_record
    {
        math::point3 p;
        math::vec3 normal;
        shared_ptr<material> mat_ptr;
        double t;
        bool front_face;

        inline void set_face_normal(const math::ray& r, const math::vec3& outward_normal)
        {
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
    };

    class hittable
    {
    public:
        virtual bool hit(const math::ray& r, math::interval ray_t, hit_record& rec) const = 0;
    };
}

#endif // SCENE_HITTABLE_HPP

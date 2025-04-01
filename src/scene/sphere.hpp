#ifndef SCENE_SPHERE_HPP
#define SCENE_SPHERE_HPP

#include "hittable.hpp"
#include "../math/vec3.hpp"

namespace jmrtiow::scene
{
    class sphere : public hittable
    {
    public:
        sphere() {}
        sphere(math::point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

        virtual bool hit(
            const math::ray& r, math::interval ray_t, hit_record& rec) const override;

    public:
        math::point3 center;
        double radius;
        shared_ptr<material> mat_ptr;
    };

    bool sphere::hit(const math::ray& r, math::interval ray_t, hit_record& rec) const
    {
        math::vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;
        if (discriminant < 0)
            return false;
        auto sqrtd = sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (-half_b - sqrtd) / a;
        if (!ray_t.surrounds(root))
        {
            root = (-half_b + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        math::vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat_ptr = mat_ptr;

        return true;
    }

    double hit_sphere(const math::point3& center, double radius, const math::ray& r)
    {
        math::vec3 oc = r.origin() - center;
        auto a = r.direction().length_squared();
        auto half_b = math::dot(oc, r.direction());
        auto c = oc.length_squared() - radius * radius;
        auto discriminiant = half_b * half_b - 4 * a * c;
        if (discriminiant < 0)
        {
            return -1.0;
        }
        else
        {
            return (-half_b - sqrt(discriminiant)) / (2.0 * a);
        }
    }
}

#endif // SCENE_SPHERE_HPP

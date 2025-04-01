#ifndef GRAPHICS_AABB_HPP
#define GRAPHICS_AABB_HPP

#include "../rtweekend.hpp"

namespace jmrtiow::math
{
    class aabb
    {
    public:
        interval x;
        interval y;
        interval z;

        aabb() {} // The default AABB is empty, since intervals are empty by default.

        aabb(const interval& x, const interval& y, const interval& z)
            : x(x), y(y), z(z)
        {
        }

        aabb(const point3& a, const point3& b)
        {
            // Treat the two points a and b as extreme for the bounding box, so we
            // don't require a particular min/max coordinate order.

            x = (a.x <= b.x) ? interval(a.x, b.x) : interval(b.x, a.x);
            y = (a.y <= b.y) ? interval(a.y, b.y) : interval(b.y, a.y);
            z = (a.z <= b.z) ? interval(a.z, b.z) : interval(b.z, a.z);
        }

        const interval& axis_interval(int n) const
        {
            if (n == 1) return y;
            if (n == 2) return z;
            return x;
        }

        bool hit(const ray& r, interval ray_t) const
        {
            const point3& ray_orig = r.origin();
            const vec3& ray_dir = r.direction();

            for (int axis = 0; axis < 3; axis++)
            {
                const interval& ax = axis_interval(axis);
                const double adinv = 1.0 / ray_dir[axis];

                double t0 = (ax.min - ray_orig[axis]) * adinv;
                double t1 = (ax.max - ray_orig[axis]) * adinv;

                if (t0 < t1)
                {
                    if (t0 > ray_t.min) ray_t.min = t0;
                    if (t1 < ray_t.max) ray_t.max = t1;
                }
                else
                {
                    if (t1 > ray_t.min) ray_t.min = t1;
                    if (t0 < ray_t.max) ray_t.max = t0;
                }

                if (ray_t.max <= ray_t.min)
                    return false;
            }
            return true;
        }
    };
}

#endif //GRAPHICS_AABB_HPP

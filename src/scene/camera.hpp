#ifndef SCENE_CAMERA_HPP
#define SCENE_CAMERA_HPP

#include "../rtweekend.hpp"

namespace jmrtiow::scene
{
    class camera
    {
    public:
        camera(
            math::point3 lookfrom,
            math::point3 lookat,
            math::vec3 vup,
            double vfov, // vertical field-of-view in degrees
            double aspect_ratio,
            double aperture,
            double focus_dist)
        {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta / 2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;

            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            origin = lookfrom;
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

            lens_radius = aperture / 2;
        }

        math::ray get_ray(double s, double t) const
        {
            math::vec3 rd = lens_radius * math::random_in_unit_disk();
            math::vec3 offset = u * rd.x + v * rd.y;

            return math::ray(
                origin + offset,
                lower_left_corner + s * horizontal + t * vertical - origin - offset);
        }

    private:
        math::point3 origin;
        math::point3 lower_left_corner;
        math::vec3 horizontal;
        math::vec3 vertical;
        math::vec3 u, v, w;
        double lens_radius;
    };
}

#endif // SCENE_CAMERA_HPP

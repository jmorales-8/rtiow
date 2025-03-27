#ifndef SCENE_MATERIAL_HPP
#define SCENE_MATERIAL_HPP

#include "../rtweekend.hpp"

namespace jmrtiow::scene
{
    struct hit_record;

    class material
    {
    public:
        virtual bool scatter(
            const math::ray& r_in, const hit_record& rec, math::color3& attenuation, math::ray& scattered) const = 0;
    };

    class lambertian : public material
    {
    public:
        lambertian(const math::color3& a) : albedo(a) {}

        virtual bool scatter(
            const math::ray& r_in, const hit_record& rec, math::color3& attenuation, math::ray& scattered) const override
        {
            auto scatter_direction = rec.normal + math::random_unit_vector();

            // Catch degenerate scatter direction
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;

            scattered = math::ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    public:
        math::color3 albedo;
    };

    class metal : public material
    {
    public:
        metal(const math::color3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            const math::ray& r_in, const hit_record& rec, math::color3& attenuation, math::ray& scattered) const override
        {
            math::vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            scattered = math::ray(rec.p, reflected + fuzz * math::random_in_unit_sphere());
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        math::color3 albedo;
        double fuzz;
    };

    class dielectric : public material
    {
    public:
        dielectric(double index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(
            const math::ray& r_in, const hit_record& rec, math::color3& attenuation, math::ray& scattered) const override
        {
            attenuation = math::color3(1.0, 1.0, 1.0);
            double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

            math::vec3 unit_direction = unit_vector(r_in.direction());
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            math::vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = math::ray(rec.p, direction);
            return true;
        }

    public:
        double ir; // Index of Refraction

    private:
        static double reflectance(double cosine, double ref_idx)
        {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * pow((1 - cosine), 5);
        }
    };
}

#endif // SCENE_MATERIAL_HPP

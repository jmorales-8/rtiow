#ifndef SCENE_HITTABLE_LIST_HPP
#define SCENE_HITTABLE_LIST_HPP

#include "hittable.hpp"
#include "material.hpp"
#include "sphere.hpp"

#include <memory>
#include <vector>

namespace jmrtiow::scene
{
    using std::make_shared;
    using std::shared_ptr;

    class hittable_list : public hittable
    {
    public:
        hittable_list() {}
        hittable_list(std::shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(std::shared_ptr<hittable> object) { objects.push_back(object); }

        virtual bool hit(
            const math::ray& r, math::interval ray_t, hit_record& rec) const override;

    public:
        std::vector<std::shared_ptr<hittable>> objects;
    };

    bool hittable_list::hit(const math::ray& r, math::interval ray_t, hit_record& rec) const
    {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects)
        {
            if (object->hit(r, math::interval(ray_t.min, closest_so_far), temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    scene::hittable_list random_scene()
    {
        scene::hittable_list world;

        auto ground_material = make_shared<scene::lambertian>(math::color3(0.5, 0.5, 0.5));
        world.add(make_shared<scene::sphere>(math::point3(0, -1000, 0), 1000, ground_material));

        for (int a = -11; a < 11; a++)
        {
            for (int b = -11; b < 11; b++)
            {
                auto choose_mat = random_double();
                math::point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

                if ((center - math::point3(4, 0.2, 0)).length() > 0.9)
                {
                    shared_ptr<scene::material> sphere_material;

                    if (choose_mat < 0.8)
                    {
                        // diffuse
                        auto albedo = math::color3::random() * math::color3::random();
                        sphere_material = make_shared<scene::lambertian>(albedo);
                        world.add(make_shared<scene::sphere>(center, 0.2, sphere_material));
                    }
                    else if (choose_mat < 0.95)
                    {
                        // scene::metal
                        auto albedo = math::color3::random(0.5, 1);
                        auto fuzz = random_double(0, 0.5);
                        sphere_material = make_shared<scene::metal>(albedo, fuzz);
                        world.add(make_shared<scene::sphere>(center, 0.2, sphere_material));
                    }
                    else
                    {
                        // glass
                        sphere_material = make_shared<scene::dielectric>(1.5);
                        world.add(make_shared<scene::sphere>(center, 0.2, sphere_material));
                    }
                }
            }
        }

        auto material1 = make_shared<scene::dielectric>(1.5);
        world.add(make_shared<scene::sphere>(math::point3(0, 1, 0), 1.0, material1));

        auto material2 = make_shared<scene::lambertian>(math::color3(0.4, 0.2, 0.1));
        world.add(make_shared<scene::sphere>(math::point3(-4, 1, 0), 1.0, material2));

        auto material3 = make_shared<scene::metal>(math::color3(0.7, 0.6, 0.5), 0.0);
        world.add(make_shared<scene::sphere>(math::point3(4, 1, 0), 1.0, material3));

        return world;
    }

    math::color3 ray_color(const math::ray& r, const scene::hittable& world, int depth)
    {
        scene::hit_record rec;

        // If we've exceeded the math::ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return math::color3(0, 0, 0);

        if (world.hit(r, math::interval(0.0001, infinity), rec))
        {

#if 0   // Alternate diffuse form
            math::point3 target = rec.p + random_in_hemisphere(rec.normal);
#elif 0 // True scene::lambertian reflection
            math::point3 target = rec.p + rec.normal + random_unit_vector();
#elif 0 // Random math::ray reflection
            math::point3 target = rec.p + rec.normal + random_in_unit_sphere();
#endif
            math::ray scattered;
            math::color3 attenuation;
            if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, world, depth - 1);
            return math::color3(0, 0, 0);
        }

        math::vec3 unit_direction = unit_vector(r.direction());
        auto t = 0.5 * (unit_direction.y + 1.0);
        return (1.0 - t) * math::color3(1.0, 1.0, 1.0) + t * math::color3(0.5, 0.7, 1.0);
    }

    hittable_list demo_scene()
    {
        hittable_list world = hittable_list();
        auto material_ground = make_shared<scene::lambertian>(math::color3(0.8, 0.8, 0.0));
        auto material_center = make_shared<scene::lambertian>(math::color3(0.1, 0.2, 0.5));
        auto material_left = make_shared<scene::dielectric>(1.5);
        auto material_right = make_shared<scene::metal>(math::color3(0.8, 0.6, 0.2), 0.0);

        world.add(make_shared<scene::sphere>(math::point3(0.0, -100.5, -1.0), 100.0, material_ground));
        world.add(make_shared<scene::sphere>(math::point3(0.0, 0.0, -1.0), 0.5, material_center));
        world.add(make_shared<scene::sphere>(math::point3(-1.0, 0.0, -1.0), 0.5, material_left));
        world.add(make_shared<scene::sphere>(math::point3(-1.0, 0.0, -1.0), -0.45, material_left)); // bubble effect on the left scene::sphere.
        world.add(make_shared<scene::sphere>(math::point3(1.0, 0.0, -1.0), 0.5, material_right));
        return world;
    }

    hittable_list demo_scene2()
    {
        hittable_list world = hittable_list();

        auto R = cos(pi / 4);
        auto material_left = make_shared<scene::lambertian>(math::color3(0, 0, 1));
        auto material_right = make_shared<scene::lambertian>(math::color3(1, 0, 0));

        world.add(make_shared<scene::sphere>(math::point3(-R, 0, -1), R, material_left));
        world.add(make_shared<scene::sphere>(math::point3(R, 0, -1), R, material_right));

        return world;
    }
}

#endif // SCENE_HITTABLE_LIST_HPP

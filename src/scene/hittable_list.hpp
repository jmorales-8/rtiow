#ifndef SCENE_HITTABLE_LIST_HPP
#define SCENE_HITTABLE_LIST_HPP

#include "hittable.hpp"

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
            const math::ray &r, double t_min, double t_max, hit_record &rec) const override;

    public:
        std::vector<std::shared_ptr<hittable>> objects;
    };

    bool hittable_list::hit(const math::ray &r, double t_min, double t_max, hit_record &rec) const
    {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = t_max;

        for (const auto &object : objects)
        {
            if (object->hit(r, t_min, closest_so_far, temp_rec))
            {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
}

#endif // SCENE_HITTABLE_LIST_HPP

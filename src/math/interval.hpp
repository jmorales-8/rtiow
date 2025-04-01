#ifndef MATH_INTERVAL_HPP
#define MATH_INTERVAL_HPP

namespace jmrtiow::math
{
    class interval
    {
    public:
        double min;
        double max;

        interval() : min(-infinity), max(infinity) {}

        interval(double min, double max) : min(min), max(max) {}

        double size() const
        {
            return max - min;
        }

        bool contains(double x) const
        {
            return min <= x && x <= max;
        }

        bool surrounds(double x) const
        {
            return min < x && x < max;
        }

        interval expand(double delta) const
        {
            double padding = delta / 2;
            return interval(min - padding, max + padding);
        }

        static const interval empty;
        static const interval universe;
    };

    const interval interval::empty = interval(+infinity, -infinity);
    const interval interval::universe = interval(-infinity, +infinity);
}

#endif //MATH_INTERVAL_HPP

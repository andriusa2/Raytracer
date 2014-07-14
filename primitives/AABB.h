#ifndef AABB_H
#define AABB_H

#include "../utils/Vector3D.h"

struct AABB {
    Vector3D min_v, max_v;
    bool Contains(Vector3D & a)
    {
        for (int i = 0; i < 3; i++)
            if (a[i] < min_v[i] || a[i] > max_v[i])
                return false;
        return true;
    }
    AABB() :min_v(), max_v() {};
    AABB(Vector3D left_, Vector3D right_) : min_v(left_), max_v(right_) {};
    float surfaceArea()
    {
        float len[3] = {};
        for (int i = 0; i < 3; i++)
            len[i] = max_v.get(i) - min_v.get(i);

        float retval = 0;
        for (int i = 0; i < 2; i++)
            for (int j = i + 1; j < 3; j++)
                retval += len[i] * len[j];
        return 2 * retval;
    }
    bool valid()
    {
        for (int i = 0; i < 3; i++)
            if (min_v.get(i) > max_v.get(i))
                return false;
        return true;
    }
};

#endif
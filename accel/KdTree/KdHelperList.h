#ifndef KDHELPERLIST_H
#define KDHELPERLIST_H
#include "../../primitives/Triangle.h"
struct KdHelperList {
    KdHelperList * next[3];
    float data[3];
    bool left;
    KdHelperList * otherSide;
    Triangle * tri;
    bool read;
    KdHelperList() : next(), data(), left(false), otherSide(0), tri(0), read(false) {};
    KdHelperList * insert(KdHelperList * node, int axis) {
        KdHelperList * retval = this;
        KdHelperList * tmp;
        for (; retval->next[axis] && retval->next[axis]->data[axis] < node->data[axis] + EPS;
            retval = retval->next[axis]);
        tmp = retval;
        retval = retval->next[axis];
        tmp->next[axis] = node;
        return retval;
    }
};

#endif
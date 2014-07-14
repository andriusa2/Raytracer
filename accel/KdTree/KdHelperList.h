#ifndef KDHELPERLIST_H
#define KDHELPERLIST_H
#include "../../primitives/Triangle.h"
struct KdHelperList {
    KdHelperList * next[3];
    KdHelperList * prev[3];
    float data[3];
    bool left;
    KdHelperList * otherSide;
    Triangle * tri;
    bool read;
    bool goes_left;
    KdHelperList() : next(), prev(), data(), left(false), otherSide(0), tri(0), read(false), goes_left(false) {};
    /*KdHelperList * insert(KdHelperList * node, int axis) {
        KdHelperList * retval = this;
        KdHelperList * tmp;
        for (; retval->next[axis] && retval->next[axis]->data[axis] < node->data[axis] + EPS;
            retval = retval->next[axis]);
        tmp = retval;
        retval = retval->next[axis];
        if (tmp->next[axis])
            tmp->next[axis]->prev[axis] = node;
        node->next[axis] = tmp->next[axis];
        tmp->next[axis] = node;
        node->prev[axis] = tmp;
        
        return retval;
    }*/
};

#endif
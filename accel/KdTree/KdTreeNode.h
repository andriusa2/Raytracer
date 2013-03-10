#ifndef KDTREENODE_H
#define KDTREENODE_H

#include "../ObjList.h"

class KdTreeNode{
public:
    enum{
        AXISBIT = (1 << 1) + (1 << 0),
        LEAFBIT = (1 << 2),
        POINTERBIT = ~(AXISBIT + LEAFBIT)
    };
    KdTreeNode();
    bool isLeaf();
    void setLeaf(bool val);
    KdTreeNode * getLeft();
    KdTreeNode * getRight();
    // the right one is left+1, so no need for setting it explicitly
    void setLeft(KdTreeNode * left);
    float getSplitPos();
    void setSplitPos(float spos);
    unsigned int getAxis();
    void setAxis(unsigned int axis);
    ObjList * getObjList();
    void setObjList(ObjList * head );
private:
    float split;
    /* as ObjList and KdTreeNodes are at least 8 bit aligned
     * then the pointer to the next one looks like
     * XX...X000, so we have 3 bits of data
     * let's use 00~10 for axis
     * 100 then denotes a leaf
     */
    unsigned int data;
};

#endif
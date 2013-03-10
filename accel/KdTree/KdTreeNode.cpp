#include "./KdTreeNode.h"

KdTreeNode::KdTreeNode():split(0.0f), data(0)
{
}

bool KdTreeNode::isLeaf() {
    return (data & LEAFBIT) == LEAFBIT;
}

void KdTreeNode::setLeaf(bool val) {
    if (val) 
        data = data | LEAFBIT;
    else
        data = data & (~LEAFBIT);
}

float KdTreeNode::getSplitPos() {
    return split;
}

void KdTreeNode::setSplitPos(float spos) {
    split = spos;
}

unsigned int KdTreeNode::getAxis() {
    return data & AXISBIT;
}

void KdTreeNode::setAxis(unsigned int axis) {
    if (axis < 3)
        data = data & (~AXISBIT) + axis;
}

ObjList * KdTreeNode::getObjList() {
    return (ObjList *)(data & POINTERBIT);
}

void KdTreeNode::setObjList(ObjList * head) {
    setLeaf(true);
    data = (unsigned int)head + data & (~POINTERBIT);
}

void KdTreeNode::setLeft(KdTreeNode * left) {
    setLeaf(false);
    data = (unsigned int)left + data & (~POINTERBIT);
}

KdTreeNode * KdTreeNode::getLeft() {
    return (KdTreeNode *)(data & POINTERBIT);
}

KdTreeNode * KdTreeNode::getRight() {
    return (KdTreeNode *)(data & POINTERBIT) + 1;
}
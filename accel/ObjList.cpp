#include "./ObjList.h"

ObjList::ObjList(Triangle * triangle) : tri(triangle), next(0)
{
}

ObjList::ObjList() : tri(0), next(0)
{
}

void ObjList::setNext(ObjList * obj) {
    next = obj;
}

ObjList * ObjList::getNext() {
    return next;
}

void ObjList::setTriangle(Triangle * triangle) {
    tri = triangle;
}

Triangle * ObjList::getTriangle() {
    return tri;
}


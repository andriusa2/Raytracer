#ifndef OBJLIST_H
#define OBJLIST_H

#include "../primitives/Triangle.h"

class ObjList{
public:
    ObjList (Triangle* triangle);
    ObjList ();
    void setNext(ObjList * obj);
    void setTriangle(Triangle * triangle);
    Triangle * getTriangle();
    ObjList * getNext();
private:
    Triangle * tri;
    ObjList * next;
};

#endif;
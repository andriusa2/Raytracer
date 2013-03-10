#ifndef ALIGNEDMMANAGER_H
#define ALIGNEDMMANAGER_H

#include <list>
#include "../accel/ObjList.h"
#include "../accel/KdTree/KdTreeNode.h"
#define NEW_ALIGNED(type,sz,align) (type*)((unsigned int)(new type[sz+1])&(~(align-1)))
class AlignedMManager {
public:
    AlignedMManager(int preload = 100000);
    KdTreeNode * getKdNode();
    ObjList * getObjListNode();
    // make sure that all references are dead before using this...
    void reset();
private:
    // lst[id][node]
    list<KdTreeNode *> kdnodes;
    list<ObjList *> objnodes;
    list<KdTreeNode*>::iterator currKd;
    list<ObjList *>::iterator currObj;
    int kdID, objID;
    int PRELOAD;
};
extern AlignedMManager mManager;

#endif;
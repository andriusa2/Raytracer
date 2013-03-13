#ifndef ALIGNEDMMANAGER_H
#define ALIGNEDMMANAGER_H

#include <list>
#include "../accel/ObjList.h"
#include "../accel/KdTree/KdTreeNode.h"
#include "../accel/KdTree/KdHelperList.h"
#define NEW_ALIGNED(type,sz,align) (type*)((unsigned int)(new type[sz+1])&(~(align-1)))
template<class T>
struct countRefs{
    int deloc;
    int maxsz;
    T* pointer;
    countRefs(int sz);
};

template<class T>
countRefs<T>::countRefs(int sz) :deloc(0), maxsz(0) {
    pointer = NEW_ALIGNED(T,sz,16);
}

class AlignedMManager {
public:
    AlignedMManager(int preload = 100000);
    KdTreeNode * getKdNode();
    ObjList * getObjListNode();
    ObjList * getObjListNodes(int amount);
    KdHelperList * getKdHelperNode();
    KdHelperList * getKdHelperNodes(int amount);
    //void releaseKdHelperNodes(list<KdHelperList *> & nodes);
    void releaseKdHelperNodes();
private:
    // lst[id][node]
    list<int> releasedKdHelpers;
    list<KdTreeNode *> kdnodes;
    list<ObjList *> objnodes;
    list< countRefs<KdHelperList> * > kdhelpnodes;
    KdHelperList* currkdh;
    KdTreeNode* currKd;
    ObjList * currObj;
    int kdID, objID, kdhID;
    int PRELOAD;
};
extern AlignedMManager mManager;

#endif;
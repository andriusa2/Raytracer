#include "./AlignedMManager.h"

AlignedMManager::AlignedMManager(int preload) :PRELOAD(preload) {
    kdnodes.push_back(NEW_ALIGNED(KdTreeNode, PRELOAD, 16));
    objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
    currKd = kdnodes.begin();
    currObj = objnodes.begin();
    kdID = 0;
    objID = 0;
}

KdTreeNode * AlignedMManager::getKdNode() {
    if(kdID < PRELOAD) {
        kdID += 2;
        return (*currKd)+(kdID-2);
    }
    list<KdTreeNode*>::iterator it = currKd++;
    if (currKd == kdnodes.end()) {
        kdnodes.push_back(NEW_ALIGNED(KdTreeNode, PRELOAD, 16));
        currKd = it++;
    }
    kdID = 0;
    return getKdNode();
}

ObjList * AlignedMManager::getObjListNode() {
    if(objID < PRELOAD) {
        objID += 2;
        return (*currObj)+(objID-2);
    }
    list<ObjList*>::iterator it = currObj++;
    if (currObj == objnodes.end()){
        objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
        currObj = it++;
    }
    objID = 0;
    return getObjListNode();
}
// SHOULD ONLY BE USED WHEN ALL REFERENCES ARE DEAD
void AlignedMManager::reset() {
    currObj = objnodes.begin();
    objID = 0;
    currKd = kdnodes.begin();
    kdID = 0;
}

AlignedMManager mManager;
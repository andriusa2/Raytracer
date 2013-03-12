#include "./AlignedMManager.h"

AlignedMManager::AlignedMManager(int preload) :PRELOAD(preload) {
    kdnodes.push_back(NEW_ALIGNED(KdTreeNode, PRELOAD, 16));
    objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
    kdhelpnodes.push_back(NEW_ALIGNED(KdHelperList, PRELOAD, 16));
    currkdh = kdhelpnodes.begin();
    currKd = kdnodes.begin();
    currObj = objnodes.begin();
    kdID = 0;
    objID = 0;
    kdhID = 0;
}

KdTreeNode * AlignedMManager::getKdNode() {
    if(kdID < PRELOAD) {
        kdID += 2;
        return (*currKd)+(kdID-2);
    }
    list<KdTreeNode*>::iterator it = currKd++;
    if (currKd == kdnodes.end()) {
        kdnodes.push_back(NEW_ALIGNED(KdTreeNode, PRELOAD, 16));
        currKd = kdnodes.end();
        currKd--;
    }
    kdID = 0;
    return getKdNode();
}

ObjList * AlignedMManager::getObjListNode() {
    if(objID < PRELOAD) {
        objID += 1;
        return (*currObj)+(objID-1);
    }
    list<ObjList*>::iterator it = currObj++;
    if (currObj == objnodes.end()){
        objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
        currObj = objnodes.end();
        currObj--;
    }
    objID = 0;
    return getObjListNode();
}

ObjList * AlignedMManager::getObjListNodes(int amount) {
    if(objID < PRELOAD && (PRELOAD - objID >= amount)) {
        objID += amount;
        return (*currObj)+(objID-amount);
    }
    if (PRELOAD < amount) {
        LogDefault->outStringN("Critical Error: not enough space for objnodes");
        LogDefault->criticalOutValue("Asked for nodes", amount);
        objnodes.push_front(NEW_ALIGNED(ObjList, amount+1, 16));
        return *(objnodes.begin());
    }
    list<ObjList*>::iterator it = currObj++;
    if (currObj == objnodes.end()){
        objnodes.push_back(NEW_ALIGNED(ObjList, PRELOAD, 16));
        currObj = objnodes.end();
        currObj--;
    }
    objID = 0;
    return getObjListNodes(amount);
}


KdHelperList * AlignedMManager::getKdHelperNode() {
    if(kdhID < PRELOAD) {
        kdhID += 1;
        return (*currkdh)+(kdhID-1);
    }
    list<KdHelperList*>::iterator it = currkdh++;
    if (currkdh == kdhelpnodes.end()){
        //kdhelpnodes.push_back(NEW_ALIGNED(KdHelperList, PRELOAD, 16));
        kdhelpnodes.push_back(new KdHelperList[PRELOAD]);
        currkdh = kdhelpnodes.end();
        currkdh--;
    }
    kdhID = 0;
    return getKdHelperNode();
}
KdHelperList * AlignedMManager::getKdHelperNodes(int amount) {
    if(kdhID < PRELOAD && (PRELOAD - kdhID >= amount)) {
        kdhID += amount;
        return (*currkdh)+(kdhID-amount);
    }
    if (PRELOAD < amount) {
        LogDefault->outStringN("Critical error: not enough space for kdhelpernodes");
        LogDefault->criticalOutValue("Asked for nodes", amount);
        //kdhelpnodes.push_front(NEW_ALIGNED(KdHelperList, amount+2, 16));
        kdhelpnodes.push_front(new KdHelperList[amount]);
        return *(kdhelpnodes.begin());
    }
    list<KdHelperList*>::iterator it = currkdh++;
    if (currkdh == kdhelpnodes.end()){
        //kdhelpnodes.push_back(NEW_ALIGNED(KdHelperList, PRELOAD, 16));
        kdhelpnodes.push_back(new KdHelperList[PRELOAD]);
        currkdh = kdhelpnodes.end();
        currkdh--;
    }
    kdhID = 0;
    return getKdHelperNodes(amount);
}
void AlignedMManager::releaseHelperNodes() {
    for (list<KdHelperList *>::iterator it = kdhelpnodes.begin();
        it!=kdhelpnodes.end(); it++)
        delete [] (*it);
    kdhelpnodes.clear();
    kdhelpnodes.push_front(0);
    kdhID = PRELOAD;
    currkdh = kdhelpnodes.begin();
}
// SHOULD ONLY BE USED WHEN ALL REFERENCES ARE DEAD
void AlignedMManager::reset() {
    currObj = objnodes.begin();
    objID = 0;
    currKd = kdnodes.begin();
    kdID = 0;
}

AlignedMManager mManager;
#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include <algorithm>
#include "./ObjList.h"
#include "./KdTree/KdTreeNode.h"
#include "../primitives/Triangle.h"
#include "../utils/AlignedMManager.h"
#include "./KdTree/KdHelperList.h"
#include "../primitives/AABB.h"
class KdStack
{
public:
	KdTreeNode* node;
	float t;
	Vector3D pb;
	int prev, dummy1, dummy2;
};
typedef bool (*fpKHListTest)(float, float);
class KdTree{
public:
    KdTree();
    KdTree(vector<Triangle *> & tris);
    void Load(vector<Triangle *> & tris);

    Triangle * intersect(Ray& ray, float& dist, float& u, float& v);
    Triangle * intersect(Ray& ray, float& dist);
    int debugIntersect(Ray& ray, float &dist);
    void DumpTree();
private:
    void DumpNode(KdTreeNode * node);
    void makeTree(vector<Triangle *> & tris);
    void subdivide(KdHelperList * heads[], KdTreeNode * node, AABB box,
        int depth, int triangles, int openL);
    unsigned int calcTriangles(KdHelperList * head, int axis,
        float splitpos, fpKHListTest test);
    void addTriangles(KdHelperList * head, int axis,
        ObjList * objs, float splitpos, fpKHListTest test);

    static bool testLeft(float head, float spos) { return head <= spos;}
    static bool testRight(float head, float spos) {return head > spos;}
    static bool testAll(float head, float spos) {return true;}
    void resetAll();
    KdTreeNode * root;    
    AABB scene_bound;
    KdStack ** mStack;
};


#endif

#ifndef VERTEX_H
#define VERTEX_H

#include <ostream>

#include "../utils/Vector3D.h"
using namespace std;
class Vertex
{
public:
	Vertex(){};
    Vertex(Vector3D& pos, float u, float v): mPos(pos), mU(u), mV(v),mNormal(V3D_BLANK){};
    Vertex(Vector3D& pos):mPos(pos), mU(0.0f), mV(0.0f),mNormal(V3D_BLANK){};
	float getU(){return mU;}
	float getV(){return mV;}
	Vector3D& getNormal(){return mNormal;}
	Vector3D& getPos(){return mPos;}
	void setUV(float u, float v){mU=u; mV=v;}
	void setPos(Vector3D& pos){mPos=pos;}
    void setNormal(Vector3D& norm) {mNormal = norm;}
    void accNormal(Vector3D& norm) {mNormal += norm;}
	void normalize(){mNormal.normalize();}
    friend ostream& operator<< (ostream& out, const Vertex& v){
        out << "Vertex:\n\tPos="<<v.mPos<<
            "\n\tNormal="<<v.mNormal<<
            "\n\t(u;v)=("<<v.mU<<";"<<v.mV<<")\n";
        return out;
    }
private:
	Vector3D mNormal, mPos;
	float mU, mV;//, mK,mDummy;
};

#endif
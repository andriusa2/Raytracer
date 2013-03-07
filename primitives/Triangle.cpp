#include "./Triangle.h"

Triangle::Triangle() {
    vertices[0] = 0;
    vertices[1] = 0;
    vertices[2] = 0;
}

Triangle::Triangle(Vertex* v1, Vertex* v2, Vertex* v3, bool calcNorms) {
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
    if (calcNorms)
        recalcNorms();
    precompute();
}

Triangle::~Triangle() {

}

int Triangle::intersect(Ray& ray, float& dist, float& u, float& v) {
    const unsigned int& ku=MOD3_ACCEL[k + 1];
	const unsigned int& kv=MOD3_ACCEL[k + 2];
	const Vector3D& O=ray.getO();
	const Vector3D& D=ray.getD();
	const Vector3D& A=vertices[0]->getPos();
	const float t=(nd-O.get(k)-nu*O.get(ku)-nv*O.get(kv))/(D.get(k)+nu*D.get(ku)+nv*D.get(kv));
	if(dist+EPS<t) return MISS;
	if(t<EPS) return MISS;
	const float hu=O.get(ku)+t*D.get(ku)-A.get(ku);
	const float hv=O.get(kv)+t*D.get(kv)-A.get(kv);
	const float alpha=hv*bnu+hu*bnv;
	if(alpha<0) return MISS;
	const float beta=hu*cnu+hv*cnv;
	if(beta<0) return MISS;
	if((alpha+beta)>1)return MISS;
	u=alpha;
	v=beta;
	dist=t;
	return HIT;
}

int Triangle::intersect(Ray& ray, float& dist) {
    const unsigned int& ku=MOD3_ACCEL[k + 1];
	const unsigned int& kv=MOD3_ACCEL[k + 2];
	const Vector3D& O=ray.getO();
	const Vector3D& D=ray.getD();
	const Vector3D& A=vertices[0]->getPos();
	const float t=(nd-O.get(k)-nu*O.get(ku)-nv*O.get(kv))/(D.get(k)+nu*D.get(ku)+nv*D.get(kv));
	if(dist+EPS<t) return MISS;
	if(t<EPS) return MISS;
	const float hu=O.get(ku)+t*D.get(ku)-A.get(ku);
	const float hv=O.get(kv)+t*D.get(kv)-A.get(kv);
	const float alpha=hv*bnu+hu*bnv;
	if(alpha<0) return MISS;
	const float beta=hu*cnu+hv*cnv;
	if(beta<0) return MISS;
	if((alpha+beta)>1)return MISS;
	dist=t;
	return HIT;
}

Vector3D Triangle::getNormal() {
    return vertices[0]->getNormal();
}

Vector3D Triangle::getNormal(const float& u, const float& v) {
    return getNormal();
}

Vertex* Triangle::getVertex(int ID) {
    if (ID >= 0 && ID < 3)
        return vertices[ID];
}

void Triangle::setVertex(int ID, Vertex* v) {
    if (ID >= 0 && ID < 3)
        vertices[ID] = v;
}

void Triangle::normalize() {
    for (int i = 0; i < 3; i++)
        vertices[i]->normalize();
}

void Triangle::precompute() {
    
	Vector3D A=vertices[0]->getPos();
	Vector3D B=vertices[1]->getPos();
	Vector3D C=vertices[2]->getPos();
	Vector3D c=B-A;
	Vector3D b=C-A;

	Vector3D mN=b.cross(c);
	int u, v;
    // find the max axis
	if(fabs(mN[0])>fabs(mN[1]))
	{
		if(fabs(mN[0])>fabs(mN[2])) k=0; else k=2;
	}
	else{
		if(fabs(mN[1])>fabs(mN[2])) k=1; else k=2;
	}
	u=(k+1)%3;//projection axis
	v=(k+2)%3;
	float koef=1.0f/(mN[k]);
	nu=mN[u]*koef;
	nv=mN[v]*koef;
	nd=mN.dot(A)*koef;
	float kofe=1.0f/(b[u]*c[v]-b[v]*c[u]);
	bnu=b[u]*kofe;
	bnv=b[v]*(-kofe);
	cnu=c[v]*kofe;
	cnv=c[u]*(-kofe);
}
void Triangle::recalcNorms() {
   	Vector3D A=vertices[0]->getPos();
	Vector3D B=vertices[1]->getPos();
	Vector3D C=vertices[2]->getPos();
	Vector3D c=B-A;
	Vector3D b=C-A;

	Vector3D mN=b.cross(c);
    mN.normalize();
	for(int i=0; i<3;++i)
		vertices[i]->accNormal(mN);
}
ostream& operator<< (ostream& out, const Triangle& tri) {
    out << "Triangle: \n";
    for (int i = 0; i < 3; i++)
        out << "\tvertex"<<i<<"="<<(*tri.vertices[i])<<"\n";
    out << "\t(nu,nv,nd)=(" << tri.nu << ";" << tri.nv << ";" << tri.nd <<")\n" <<
        "\t(bnu,bnv)=(" << tri.bnu << ";" << tri.bnv << ")\n" <<
        "\t(cnu,cnv)=(" << tri.cnu << ";" << tri.cnv << ")\n";
    return out;
}
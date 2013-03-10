#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
    Provides default values to some processess, should be only used in config
*/

#include "./Vector3D.h"
#include "../libs/dSFMT.h"
const float EPS =   0.00001f;	//small value
const int MAX_DEPTH = 12;		//how many bounces can ray do
const float PI = 3.14159265359f;
const float INF = 10000000.0;				//infinity
const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 600;
const Vector3D V3D_BLANK(0,0,0);
const Vector3D V3D_WHITE(1,1,1);
const Vector3D V3D_GREEN(0,1,0);
const unsigned int MOD3_ACCEL[] = {0,1,2,0,1,2,0};

extern dsfmt_t* __dsfmt_sz; // should be explicitly initialized somewhere

#define GET_RND(id) dsfmt_genrand_close_open(__dsfmt_sz + id)
#define DEFAULT_CONFIG "config.ini"
#define DEFAULT_LOG "raytracer.log"
#define DEFAULT_SCENE "scene.ini"
#define COL_SCREEN 0xFF88FF88
//hash table stuff
#define HASH		2097152 //size of hash table
#define P1			73856093 //prime numbers
#define P2			19349663
#define P3			83492791
#define HASHC		11	
#define HASHCF		11.0
//#define PT
//#define OUTBMPS


#endif
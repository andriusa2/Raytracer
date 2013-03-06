#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
    Provides default values to some processess, should be only used in config
*/
using namespace std;

const float EPS =   0.00001f;	//small value
const int MAX_DEPTH = 12;		//how many bounces can ray do
const float PI = 3.141592653589793238462f;
const float INF = 100000000000.0;				//infinity
const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 600;

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
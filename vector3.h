#ifndef VECTOR3_H
#define VECTOR3_H
#include <cmath>
#include <iostream>
struct V3 {
    float x ;
    float y ;
    float z ;
} ;

void sub(const V3* u, const V3* v, V3* w) ;
void add(const V3* u, const V3* v, V3* w) ;
void scale(float s, V3* v) ;
void cross(const V3* u, const V3* v, V3* w) ;
float mag2(const V3* v) ;
float mag(const V3* v) ;
float dot(const V3* u, const V3* v) ;
void normalize(V3* u) ;
void debug_v3(V3* v, std::string s) ;

#endif
#ifndef VECTOR3_H
#define VECTOR3_H
#include <cmath>
struct V3 {
    double x ;
    double y ;
    double z ;
} ;

void sub(const V3* u, const V3* v, V3* w) ;
void add(const V3* u, const V3* v, V3* w) ;
void scale(const double s, V3* v) ;
void cross(const V3* u, const V3* v, V3* w) ;
double mag2(const V3* v) ;
double mag(const V3* v) ;
double dot(const V3* u, const V3* v) ;
void normalize(V3* u) ;

#endif
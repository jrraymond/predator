/** The code for three dimensional vectors go here
*/
#include "vector3.h"

void add(const V3* u, const V3* v, V3* w) {
    w->x = u->x + v->x ;
    w->y = u->y + v->y ;
    w->z = u->z + v->z ;
}
void sub(const V3* u, const V3* v, V3* w) {
    w->x = u->x - v->x ;
    w->y = u->y - v->y ;
    w->z = u->z - v->z ;
}
void scale(float s, V3* v) {
    v->x *= s ;
    v->y *= s ;
    v->z *= s ;
}
void cross(const V3* u, const V3* v, V3* w) {
    w->x = u->y * v->z - u->z * v->y ;
    w->y = u->z * v->x - u->x * v->z ;
    w->z = u->x * v->y - u->y * v->x ;
}
float mag2(const V3* v) {
    return v->x * v->x + v->y * v->y + v->z * v->z ;
}
float mag(const V3* v) {
    return std::sqrt(mag2(v)) ;
}
float dot(const V3* u, const V3* v) {
    return u->x * v->x + u->y * v->y + u->z * v->z ;
}
void normalize(V3* u) {
    const float s = mag(u) ;
    u->x = u->x / s ;
    u->y = u->y / s ;
    u->z = u->z / s ;
}
void debug_v3(V3* v, std::string s) {
    std::cout << s << "| V3: " << v->x << "," << v->y << "," << v->z << std::endl ;
}

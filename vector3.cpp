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

bool is_zero_length(const V3 *u) {
    return mag2(u) == 0;
}

void normalize(V3* u) {
    const float m2 = mag2(u);
    if (m2 == 0) {
        u->x = 0;
        u->y = 0;
        u->z = 0;
        return;
    }
    const float s = std::sqrt(m2) ;
    u->x = u->x / s ;
    u->y = u->y / s ;
    u->z = u->z / s ;
}
void debug_v3(V3* v, std::string s) {
    std::cout << s << "| V3: " << v->x << "," << v->y << "," << v->z << std::endl ;
}

std::ostream &operator<<(std::ostream &stream, const V3 &v) {
    stream << "{" << v.x << "," << v.y << "," << v.z << "}";
    return stream;
}

void cap(float s, V3 *v) {
    float m = mag2(v);
    if (m > s*s) scale(s / std::sqrt(m),v) ;
}

bool operator==(const V3 &u, const V3 &v) {
    return u.x == v.x && u.y == v.y && u.z == v.z;
}

V3 fromGLM(const glm::vec3 v) {
    return V3 {v.x, v.y, v.z};
}

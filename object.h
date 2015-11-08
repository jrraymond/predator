#ifndef OBJECT_H
#define OBJECT_H
#include "vector3.h"
#include <vector>
using std::vector ;

struct Vtx {
    V3 position ;
    V3 color ;
    V3 normal ;
} ;

class Obj {
public:
    virtual V3 collides(V3 p, V3 fpos) = 0 ;
    virtual void render() = 0 ;
} ;

class InvertedBox : Obj {
public:
    float x, y, z, pdx, pdy, pdz, ndx, ndy, ndz, r, g, b, a ;

    V3 collides(V3 p, V3 fpos);

    void render() { };
    static void render(vector<InvertedBox>& iboxes, float* pts) ;

    V3 nur(); // n/f = near/far = +/- z axis
    V3 nul(); // u/d = up/down = +/- y axis
    V3 nlr(); // r/l = right/left = +/- x axis'
    V3 nll();
    V3 fur();
    V3 ful();
    V3 flr();
    V3 fll();
    InvertedBox() {
       x = 0; y = 0; z = 0; pdx = 10; pdy = 10; pdz = 10;
        r = 1; g = 1; b = 1; a =1 ;
    };
    InvertedBox (float ix, float iy, float iz,
            float ipdx, float ipdy, float ipdz,
            float ir, float ig, float ib, float ia) {
        x = ix ;
        y = iy ;
        z = iz ;
        pdx = ipdx ;
        pdy = ipdy ;
        pdz = ipdz ;
        r = ir ;
        g = ig ;
        b = ib ;
        a = ia ;
    }

} ;
float* gen_2d_grid(int* size, int* num_pts, int dim, int step, Axis fixed_axis, float fixed_at) ;
float* gen_3d_grid(int* size, int* num_pts, int dim, int step) ;
float* gen_boid_normals(Vtx* vertices, int num_vertices, int* num_pts) ;
void debug_vtx(const Vtx &v);
void debug_vertices(Vtx* vs, int size);
#endif
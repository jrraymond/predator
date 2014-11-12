/** The codes for the boids will go here
*/
#include "boids.h"

void update_boid(Boid* b, std::vector<Boid> bs) {
    V3 new_acc = V3{0,0,0} ;
    V3 sep = V3{0,0,0} ;
    V3 coh = V3{0,0,0} ;
    V3 ali = V3{0,0,0} ;
    seperate(1, b, bs, &sep) ;
    cohesion(1, b, bs, &coh) ;
    align(1, bs, &ali) ;
    add(&sep,&coh,&new_acc) ;
    add(&new_acc, &ali, &new_acc) ;
}

void seperate(const double s, const Boid* b, const std::vector<Boid> bs, V3* sep) {
    double d ; //distance between boid and other
    int c = 0 ; //number of boids near
    double ss = s * s ;
    V3 vd ; //distance vector
    V3 p = b->pos ; //position of boid
    for (int i = 0; i < bs.size(); ++i) {
        sub(&p, &bs[i].pos, &vd) ; //calculate distance vector
        d = mag2(&vd) ;
        if (d < ss and d != 0) {
            normalize(&vd) ;
            scale(1 / d, &vd) ; //scale by inverse of distance squared
            add(&vd, sep, sep) ;
            ++c ;
        }
    }
    if (c != 0) { scale(1 / c, sep) ; }
}

void cohesion(const double x, const Boid* b, const std::vector<Boid> bs, V3* coh) {
    V3 c = centroid(bs) ;
    unsigned long s = bs.size() ;
    sub(&c, &b->pos, coh) ;
    if (s != 0) { scale(1 / x, coh) ; }
}
void align(const double x, const std::vector<Boid> bs, V3* ali) {
    unsigned long s = bs.size() ;
    for (int i = 0; i < s; ++i) { add(&bs[i].vel, ali, ali) ; }
    if (s != 0) { scale(1 / x, ali) ; }
}
/* void avoid(double s, Boid* b, std::vector<Boid> bs) {  } */
V3 centroid(std::vector<Boid> bs) {
    double x = 0 ;
    double y = 0 ;
    double z = 0 ;
    V3 tmp ;
    double s = (double) bs.size() ;
    for (int i = 0; i < s; ++i) {
        tmp = bs[i].pos ;
        x += tmp.x ;
        y += tmp.y ;
        z += tmp.z ;
    }
    return V3{x / s, y / s, z / s} ;
}
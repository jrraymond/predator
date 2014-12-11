/** The codes for the boids will go here
*/
#include "boids.h"
using std::vector ;

void update_flock(vector<Boid>& bs) {
    unsigned long sz = bs.size() ;
    vector<Boid> old_bs(bs) ;
    for (int i = 0; i < sz; ++i) {
        Boid new_b(old_bs[i]) ;
        update_boid(&new_b, old_bs) ;
        bs[i] = new_b ;
    }
    std::cout << "\n====================================\n" ;
}
//TODO boids align grows super fast, seems to be better now
void update_boid(Boid* b, vector<Boid>& bs) {
    float max_acc = 0.001f ;
    float max_vel = 1.0f ;
    V3 acc, sep, coh, ali, grav, center = V3{0,0,0} ;

    seperate(50, b, bs, &sep) ; add(&sep, &acc, &acc) ;
    debug_v3(&sep, " SEP ") ;

    cohesion(1, b, bs, &coh) ; add(&coh, &acc, &acc) ;
    debug_v3(&coh, " COH ") ;

    //scale(100, &coh) ;
    align(1, bs, &ali) ; add(&ali, &acc, &acc) ;
    debug_v3(&ali, " ALI ") ;

    //add(&sep,&coh,&acc) ;
    gravity(0.25f, &center, b, &grav) ; add(&grav, &acc, &acc) ;
    debug_v3(&grav, " GRAV ") ;

    //debug_v3(&acc, " ACC ") ;
    V3 pos = b->pos ;
    V3 vel = b->vel ;
    add(&b->acc, &acc, &acc) ;
    add(&acc, &vel, &vel) ;
    scale(max_acc / mag(&acc), &acc) ;
    scale(max_vel / mag(&vel), &vel) ;
    //normalize(&vel) ;
    //debug_v3(&vel, " VEL ") ;
    add(&vel, &pos, &pos) ;
    b->pos = pos ;
    b->vel = vel ;
    b->acc = acc ;
    std::cout << "\n------------------------\n" ;
    if (pos.x == NAN || pos.y == NAN || pos.z == NAN) {
        pos.x, pos.y, pos.z = 1.0f ;
        b->pos = pos ;
        b->vel = V3{0.1f, 0.1f, 0.1f} ;
        b->acc = V3{0.1f, 0.1f, 0.1f} ;
    }
}

void seperate(float s, const Boid* b, const vector<Boid>& bs, V3* sep) {
    float d ; //distance between boid and other
    int c = 0 ; //number of boids near
    float ss = s * s ;
    V3 vd ; //distance vector
    V3 p = b->pos ; //position of boid
    for (int i = 0; i < bs.size(); ++i) {
        sub(&p, &bs[i].pos, &vd) ; //calculate distance vector
        d = mag2(&vd) ;
        if (d < ss and d != 0) {
            normalize(&vd) ;
            scale(1.0f / d, &vd) ; //scale by inverse of distance squared
            add(&vd, sep, sep) ;
            ++c ;
        }
    }
    if (c != 0) { scale(1.0f / c, sep) ; }
}

void cohesion(float x, const Boid* b, const vector<Boid>& bs, V3* coh) {
    V3 c = centroid(bs) ;
    unsigned long s = bs.size() ;
    sub(&c, &b->pos, coh) ;
    if (s != 0) { scale(1.0f / s * x, coh) ; }
}
void align(float x, const vector<Boid>& bs, V3* ali) {
    unsigned long s = bs.size() ;
    for (int i = 0; i < s; ++i) {
        add(&bs[i].vel, ali, ali) ;
    }
    if (s != 0) { scale(1.0f / s * x, ali) ; }
}
/* void avoid(float s, Boid* b, vector<Boid> bs) {  } */
V3 centroid(const vector<Boid>& bs) {
    float x = 0, y = 0, z = 0 ;
    V3 tmp ;
    float s = (float) bs.size() ;
    for (int i = 0; i < s; ++i) {
        tmp = bs[i].pos ;
        x += tmp.x ;
        y += tmp.y ;
        z += tmp.z ;
    }
    return V3 { x / s, y / s, z / s } ;
}
void debug_boid(Boid* b) {
    std::cout << "Boid: p[" << b->pos.x << "," << b->pos.y << "," << b->pos.z << "]" << std::endl ;
}
void orbit(vector<Boid> &bs, V3 v, float angle) {
    V3 p;
    V3 p_new ;
    float cos_a = (float) cos(angle) ;
    float sin_a = (float) sin(angle) ;
    for (int i=0; i < bs.size() ; ++i) {
        p = bs[i].pos ;
        p_new.x = cos_a * p.x - sin_a * p.y + v.x * p.z ;
        p_new.y = sin_a * p.x + cos_a * p.y + v.y * p.z ;
        p_new.z = v.z * p.z ;
        bs[i].pos = p_new ;
    }
}

/** Computes the gravitational force on boid b from body m with mass mass and stores
* it in v.
*/
void gravity(float mass, V3* m, const Boid* b, V3* v) {
    V3 dist ;
    sub(m, &b->pos, &dist) ;
    normalize(&dist) ;
    float s = mag2(&dist) ;
    scale(1 / s * mass, &dist) ;
    add(&dist, v, v) ;
}

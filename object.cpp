#include "object.h"
using std::vector ;

void InvertedBox::render(vector<InvertedBox>& iboxes, float* pts) {
    InvertedBox b ;
    for (int i=0; i<iboxes.size();++i) {
        b = iboxes[i];
        //nur to nul
        pts[i]   = b.x + b.pdx; //nur
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z + b.pdz;
        pts[++i] = b.x - b.pdx; //nul
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z + b.pdz;
        //nur to fur
        pts[++i]   = b.x + b.pdx; //nur
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z + b.pdz;
        pts[++i] = b.x + b.pdx; //fur
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z - b.pdz;
        //nur to nlr
        pts[++i]   = b.x + b.pdx; //nur
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z + b.pdz;
        pts[++i] = b.x + b.pdx; //nlr
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z + b.pdz;
        //flr to nlr
        pts[++i] = b.x + b.pdx; //flr
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z - b.pdz;
        pts[++i] = b.x + b.pdx; //nlr
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z + b.pdz;
        //flr to fur
        pts[++i] = b.x + b.pdx; //flr
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z - b.pdz;
        pts[++i] = b.x + b.pdx; //fur
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z - b.pdz;
        //flr to fll
        pts[++i] = b.x + b.pdx; //flr
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z - b.pdz;
        pts[++i] = b.x - b.pdx; //fll
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z - b.pdz;
        //ful to fur
        pts[++i] = b.x - b.pdx; //ful
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z - b.pdz;
        pts[++i] = b.x + b.pdx; //fur
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z - b.pdz;
        //ful to fll
        pts[++i] = b.x - b.pdx; //ful
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z - b.pdz;
        pts[++i] = b.x - b.pdx; //fll
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z - b.pdz;
        //ful to nul
        pts[++i] = b.x - b.pdx; //ful
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z - b.pdz;
        pts[++i] = b.x - b.pdx; //nul
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z + b.pdz;
        //nll to nul
        pts[++i] = b.x - b.pdx; //nll
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z + b.pdz;
        pts[++i] = b.x - b.pdx; //nul
        pts[++i] = b.y + b.pdy;
        pts[++i] = b.z + b.pdz;
        //nll to fll
        pts[++i] = b.x - b.pdx; //nll
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z + b.pdz;
        pts[++i] = b.x - b.pdx; //fll
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z - b.pdz;
        //nll to nlr
        pts[++i] = b.x - b.pdx; //nll
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z + b.pdz;
        pts[++i] = b.x + b.pdx; //nlr
        pts[++i] = b.y - b.pdy;
        pts[++i] = b.z + b.pdz;
    }
}
//TODO do correct bounce
V3 InvertedBox::collides(V3 p, V3 fpos) {
    V3 vel;
    sub(&fpos, &p, &vel);
    if (fpos.x > x + pdx || fpos.x < x - pdx ||
            fpos.y > y + pdy || fpos.y < y - pdy ||
            fpos.z > z + pdz || fpos.z < z - pdz) {
        scale(-1.0f, &vel) ; // reverse direction
        return vel ;
    }
    return V3 {0,0,0} ;
}


V3 InvertedBox::fll() { return V3 {x - pdx, y - pdy, z - pdz } ; }
V3 InvertedBox::flr() { return V3 {x + pdx, y - pdy, z - pdz } ; }
V3 InvertedBox::ful() { return V3 {x - pdx, y + pdy, z - pdz } ; }
V3 InvertedBox::fur() { return V3 {x + pdx, y + pdy, z - pdz } ; }
V3 InvertedBox::nll() { return V3 {x - pdx, y - pdy, z + pdz } ; }
V3 InvertedBox::nlr() { return V3 {x + pdx, y - pdy, z + pdz } ; }
V3 InvertedBox::nul() { return V3 {x - pdx, y + pdy, z + pdz } ; }
V3 InvertedBox::nur() { return V3 {x + pdx, y + pdy, z + pdz } ; }

float* gen_3d_grid(int* size, int* num_pts, int dim, int step) { //TODO gen only 1 dir then rotate 2x
    int vert_s = 3 ;
    *num_pts = 5292 ;//(2 * dim / step + 1) * 4 ;
    *size = *num_pts * vert_s * (int) sizeof(float) ;
    float* pts = (float*) malloc((size_t)*size) ;
    int i = 0 ;
    for (int x = -dim; x <= dim; x += step) {
        for (int y = -dim; y <= dim; y += step) {
            pts[i++] = (float) x;
            pts[i++] = (float) y;
            pts[i++] = (float) -dim;
            pts[i++] = (float) x;
            pts[i++] = (float) y; //repeat for grid max
            pts[i++] = (float) dim;
        }
        for (int z = -dim; z <= dim; z += step) { //now fix x
            pts[i++] = (float) x;
            pts[i++] = (float) -dim;
            pts[i++] = (float) z;
            pts[i++] = (float) x;
            pts[i++] = (float) dim;
            pts[i++] = (float) z;
        }
    }
    return pts ;
}
void debug_vtx(const Vtx &v) {
    std::cout << "|p(" << v.position.x << "," << v.position.y << "," << v.position.z << ")" ;
    std::cout << "|c(" << v.color.x << "," << v.color.y << "," << v.color.z << ")" ;
    std::cout << "|n(" << v.normal.x << "," << v.normal.y << "," << v.normal.z << ")" ;
}


void debug_vertices(Vtx* vs, int size) {
    std::cout << "Vertices|\n\t" ;
    for(int i=0; i<size; ++i) {
        debug_vtx(vs[i]) ;
    }
    std::cout << std::endl ;
}

float* gen_2d_grid(int* size, int* num_pts, int dim, int step, Axis fixed_axis, float fixed_at) {
    int vert_s = 3 ;
    *num_pts = (2 * dim / step + 1) * 4 ;
    *size = *num_pts * vert_s * (int) sizeof(float) ;
    float* pts = (float*) malloc((size_t)*size) ;
    int i = 0 ;
    switch (fixed_axis) {
        case X_AXIS :
            for (int y = -dim; y <= dim; y += step) {
                pts[i++] = fixed_at ;
                pts[i++] = (float) y ;
                pts[i++] = (float) -dim ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) y ; //repeat for grid max
                pts[i++] = (float) dim ;
            }
            for (int z = -dim; z <= dim; z += step) { //now fix x
                pts[i++] = fixed_at ;
                pts[i++] = (float) -dim ;
                pts[i++] = (float) z ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) dim ;
                pts[i++] = (float) z ;
            }
            break ;
        case Y_AXIS:
            for (int x = -dim; x <= dim; x += step) {
                pts[i++] = (float) x ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) -dim ;
                pts[i++] = (float) x ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) dim ;
            }
            for (int z = -dim; z <= dim; z += step) {
                pts[i++] = (float) -dim ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) z ;
                pts[i++] = (float) dim ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) z ;
            }
            break ;
        case Z_AXIS:
            for (int x = -dim; x <= dim; x += step) {
                pts[i++] = (float) x ;
                pts[i++] = (float) -dim ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) x ;
                pts[i++] = (float) dim ;
                pts[i++] = fixed_at ;
            }
            for (int y = -dim; y <= dim; y += step) { //now fix x
                pts[i++] = (float) -dim ;
                pts[i++] = (float) y ;
                pts[i++] = fixed_at ;
                pts[i++] = (float) dim ;
                pts[i++] = (float) y ;
                pts[i++] = fixed_at ;
            }
            break ;
    }
    return pts ;
}

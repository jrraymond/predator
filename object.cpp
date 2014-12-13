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
V3 InvertedBox::collides(V3 p, V3 v, float t) {
    V3 future_p ;
    add(&p, &v, &future_p) ;
    scale(t, &future_p) ;
    if (future_p.x > x + pdx || future_p.x < x - pdx ||
            future_p.y > y + pdy || future_p.y < y - pdy ||
            future_p.z > z + pdz || future_p.z < z - pdz) {
        scale(-1.0f, &future_p) ; // reverse direction
        return future_p ;
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
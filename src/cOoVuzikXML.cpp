#include "cOoVuzikXML.h"


void cOo::VuzikXML::test() {
    
}

void cOo::VuzikXML::init(long size, double line_w, int A, int R, int G, int B) {
    
    numPts = size;
    lineWidth = line_w;
    alpha = A;
    red = R;
    green = G;
    blue = B;
    
    X = new double[size];
    Y = new double[size];
}


int cOo::VuzikXML::parseVuzikLineType(int r) {

    //    {237,212,0}, //yellow: sop
    //    {206,115,140}, //pink: alto
    //    {168,0,51}, //red: tenor
    //    {245,121,0} //orange: bass

    switch (r) {
        case 237:
            return SOP;
            break;
        case 206:
            return ALT;
            break;
        case 168:
            return TEN;
            break;
        case 245:
            return BAS;
            break;
        default:
            return -1;
    }
}

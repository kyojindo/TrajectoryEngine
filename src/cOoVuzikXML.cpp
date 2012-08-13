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

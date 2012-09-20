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
            return YELLOW;
            break;
        case 206:
            return PINK;
            break;
        case 168:
            return RED;
            break;
        case 245:
            return ORANGE;
            break;
        case 114:
            return LIGHT_BLUE;
            break;
        case 0:
            return NAVY;
            break;
        case 141:
            return GREEN;
            break;
        case 195:
            return GRAY;
            break;
        default:
            printf("invalid line found!!\n");
            return -1;
    }
}

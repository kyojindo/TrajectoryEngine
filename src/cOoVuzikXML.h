#ifndef __cOoVuzikXML__
#define __cOoVuzikXML__

#define VUZIK_Y_MIN 0
#define VUZIK_Y_MAX 1056

#define VUZIK_PITCH_MIN 0.0f
#define VUZIK_PITCH_MAX 33.0f

#include <iostream>
#include "ofMain.h"

using namespace std;

namespace cOo {
    
    class VuzikXML {
        
        enum vuzikColor {
            
            PINK, RED, ORANGE, YELLOW,
            GREEN, LIGHT_BLUE, NAVY, GRAY
        };
        
      public:
        
        void init( long size, double line_w, int A, int R, int G, int B );
        
        void setX( double val, long idx ) { X[idx] = val; }
        void setY( double val, long idx ) { Y[idx] = val; }
        void setScale( int sc ) { scale = sc; }
        
        double getLineWidth( void ) { return lineWidth; }
        long getSize( void ) { return numPts; }
        
        double getX( long idx ) { return X[idx]; }
        double getY( long idx ) { return Y[idx]; }
        int getScale( void ) { return scale; }
        double getPitch( long idx );
        
        void test( void );
        
        int alpha;
        int red;
        int green;
        int blue;
        
        //we can find what voice it is based on red color alone
        static int parseVuzikLineType(int r);
        
      protected:
        
        long numPts;
        double* X; double* Y;
        double lineWidth;
        int scale;
    };
}

#endif

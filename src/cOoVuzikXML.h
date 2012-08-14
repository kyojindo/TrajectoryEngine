#ifndef __TrajectoryEngine__cOoVuzikXML__
#define __TrajectoryEngine__cOoVuzikXML__

#include <iostream>

using namespace std;

namespace cOo {

    
    class VuzikXML {
        
    public:
        void init(long size, double line_w, int A, int R, int G, int B);
        
        void test();
        
        void setX(double val, long idx) {X[idx] = val;}
        void setY(double val, long idx) {Y[idx] = val;}
        
        double getX(long idx) {return X[idx];}
        double getY(long idx) {return Y[idx];}
        double getPitch(long idx);
        
        long getSize() {return numPts;}
        
        int alpha;
        int red;
        int green;
        int blue;
        
    private:
        
        long numPts;
        double* X;
        double* Y;
        
        double lineWidth;
        

    };
}



#endif /* defined(__TrajectoryEngine__cOoVuzikXML__) */

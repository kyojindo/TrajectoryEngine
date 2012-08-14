#ifndef __cOoSketchedCurve__
#define __cOoSketchedCurve__

#include "cOoScreenMapper.h"
#include "cOoBreakPointFunction.h"

#include "ofMain.h"

namespace cOo {
    
    class SketchedCurve {
    
      public:
        
        void set( BreakPointFunction *function, ScreenMapper *smap );
        
        void draw( void );
        
      protected:
        
        ScreenMapper *screenMapper;
        BreakPointFunction *bpf;
        
        float xVal, yVal, rVal;
        Record current, previous;
        float xPrevious, yPrevious;
        float xCurrent, yCurrent;
        
        int bpfColor;
    };
}

#endif

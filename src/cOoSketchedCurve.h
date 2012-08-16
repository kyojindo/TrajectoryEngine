#ifndef __cOoSketchedCurve__
#define __cOoSketchedCurve__

#include "cOoScreenMapper.h"
#include "cOoBreakPointFunction.h"

#include "ofMain.h"

namespace cOo {
    
    class SketchedCurve {
    
      public:
        
        void set( BreakPointFunction *function, ScreenMapper *smap );
        void redrawFbo( void ); void draw( void ); // ------------
        
      protected:
        
        BreakPointFunction *bpf;
        ScreenMapper *screenMapper;
        
        float fboWidth;
        float fboHeight;
        ofFbo *fbo;
    };
}

#endif

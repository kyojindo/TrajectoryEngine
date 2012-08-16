#ifndef __cOoSketchedCurve__
#define __cOoSketchedCurve__

#include "cOoScreenMapper.h"
#include "cOoBreakPointFunction.h"

#include "ofMain.h"

namespace cOo {
    
    class SketchedCurve {
    
      public:
        
        void link( BreakPointFunction *function, ScreenMapper *smap );
        void generate( void ); void draw( void ); bool isVisible( void );
        
      //protected:
        
        bool visible;
        
        BreakPointFunction *bpf;
        ScreenMapper *screenMapper;
        
        float fboWidth;
        float fboHeight;
        ofFbo *fbo;
    };
}

#endif

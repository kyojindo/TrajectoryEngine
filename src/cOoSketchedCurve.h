#ifndef __cOoSketchedCurve__
#define __cOoSketchedCurve__

#include "cOoScreenMapper.h"
#include "cOoBreakPointFunction.h"
#include "cOoVuzikXML.h"
#include "cOoColorMap.h"

#include "ofMain.h"

namespace cOo {
    
    class SketchedCurve {
    
      public:
        
        void link( BreakPointFunction *function, ScreenMapper *smap );
        void generate( void ); void destroy( void ); // managing FBO
        void draw( void ); bool isVisible( void ); // managing draw
        
      protected:
        
        bool visible;
        
        BreakPointFunction *bpf;
        ScreenMapper *screenMapper;
        
        ColorMap colorMap;
        
        float fboWidth;
        float fboHeight;
        ofFbo *fbo;
    };
}

#endif

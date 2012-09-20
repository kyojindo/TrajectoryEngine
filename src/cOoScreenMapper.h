#ifndef __cOoScreenMapper__
#define __cOoScreenMapper__

#include "cOoDataSet.h"
#include "ofMain.h"

namespace cOo {
    
    class ScreenMapper {
    
      public:
        
        ScreenMapper( void );
        
        void setPixelPerSec( double pps );
        void incTimeOffset( Time shift );
        void setTimeOffset( Time to );
        
        double getPixelPerSec( void );
        Time getTimeOffset( void );
        
        float getXfromTime( Time time );
        Time getTimefromX( float x );
        
      protected:
        
        double pixelPerSec;
        Time timeOffset;
    };
}

#endif

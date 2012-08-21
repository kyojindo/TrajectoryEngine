#ifndef __cOoColorMap__
#define __cOoColorMap__

#include <vector>

namespace cOo {
    
    class ColorMap {
    
      public:
        
        ColorMap( void );
        float get( int index );
        
      protected:
        
        std::vector<float> map;
    };
}

#endif

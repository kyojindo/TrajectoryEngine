#ifndef __cOoDataSet__
#define __cOoDataSet__

namespace cOo {
    
    typedef double Time;
    
    class DataSet {
    
      public:
        
        double &getPitch( void );
        double &getVelocity( void );
        int &getScale( void ) {return scale; }
        
        void set( double dataPitch, double dataVelocity );
        DataSet interpolate( DataSet &data, double factor );
        
      //protected:
        
        double velocity; // velocity in [0.0f-1.0f]
        double pitch; // pitch as decimal MIDI
        Time time; // time to display set
        int scale;
    };
}

#endif

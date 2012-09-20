#ifndef __cOoAudioTimer__
#define __cOoAudioTimer__

#include "ofMain.h"

namespace cOo {
    
    typedef void (*UserCallback)( void *usrPtr );
    const int MinAudioBufferSize = 128;
    
    class AudioTimer;
    
    class TimerThread : public ofThread {
    
      public:
        
        void toClass( AudioTimer *at, int step );
        void threadedFunction( void );
        
        AudioTimer *audioTimer;
        int stepInMs;
    };
    
    class AudioTimer {
    
      public:
        
        AudioTimer( void ); // audio timer setup needs buffer size, time inc and cb
        bool setup( int audioBufferSize, double timeInterval, UserCallback callback, void *appPtr );
        void moveOffset( double offsetTime ); // move offset to a given location
        
        bool isRunning( void ); // check if the timer is running
        double getTime( void ); // get audio time from the timer
        
        void start( void ); // start the timer
        void stop( void ); // stop the timer
        
        void timeInc( void );
        
      protected:
        
        TimerThread timerThread; // thread that replace the audio timer
        
        double audioCallbackTime; // time stamp updated by audio callbacks
        double userTimeInterval; // time increment between user callbacks
        double userCallbackTime; // time stamp udated by user callbacks
        UserCallback userCallback; // user callback function
        void *userAppPtr; // user application pointer
        
        float deviceSampleRate;
        int deviceBufferSize;
        
        bool running;
    };
}

#endif

#ifndef __cOoAudioTimer__
#define __cOoAudioTimer__

#include <CoreAudio/AudioHardware.h>

namespace cOo {
    
    typedef void (*UserCallback)( void *usrPtr );
    const int MinAudioBufferSize = 128;
    
    class AudioTimer {
    
      public:
        
        AudioTimer( void ); // audio timer setup needs buffer size, time inc and cb
        bool setup( int audioBufferSize, double timeInterval, UserCallback callback, void *appPtr );
        void moveOffset( double offsetTime ); // move offset to a given location
        
        bool isRunning( void ); // check if the timer is running
        double getTime( void ); // get audio time from the timer
        
        void start( void ); // start the timer
        void stop( void ); // stop the timer
        
      protected:
        
        // the CoreAudio audio callback -------------
        static OSStatus ioProc( AudioDeviceID inDevice,
        const AudioTimeStamp *inNow, const AudioBufferList *inInputData,
        const AudioTimeStamp *inInputTime, AudioBufferList *outOutputData,
        const AudioTimeStamp *inOutputTime, void* defptr );
        
        double audioCallbackTime; // time stamp updated by audio callbacks
        double userTimeInterval; // time increment between user callbacks
        double userCallbackTime; // time stamp udated by user callbacks
        UserCallback userCallback; // user callback function
        void *userAppPtr; // user application pointer
        
        AudioStreamBasicDescription deviceFormat;
        AudioObjectPropertyAddress propertyAddress;
        AudioDeviceIOProcID deviceIOProcId;
        
        Float64 deviceSampleRate;
        UInt32 deviceBufferSize;
        AudioDeviceID device;
        UInt32 ioDataSize;
        
        bool running;
    };
}

#endif

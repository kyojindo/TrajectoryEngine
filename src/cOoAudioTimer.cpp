#include "cOoAudioTimer.h"

cOo::AudioTimer::AudioTimer( void ) {
    
    audioCallbackTime = 0.0f;
    userCallback = NULL;
    running = false;
}

bool cOo::AudioTimer::setup( int audioBufferSize, double timeInterval, UserCallback callback, void *appPtr ) {

    device = kAudioDeviceUnknown;
    OSStatus err = kAudioHardwareNoError;
    
    // set items of property address that never change
    propertyAddress.mElement = kAudioObjectPropertyElementMaster;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    
    propertyAddress.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
    ioDataSize = sizeof( AudioDeviceID ); // ask default output device
    
    err = AudioObjectGetPropertyData( kAudioObjectSystemObject, &propertyAddress, 0, NULL, &ioDataSize, &device );
    if( err != kAudioHardwareNoError ) return false; // send the request to master kAudioObjectSystemObject
    
    propertyAddress.mSelector = kAudioDevicePropertyActualSampleRate;
    ioDataSize = sizeof( Float64 ); // ask for the samplerate
    
    err = AudioObjectGetPropertyData( device, &propertyAddress, 0, NULL, &ioDataSize, &deviceSampleRate );
    if( err != kAudioHardwareNoError ) return false; // send the request to our own audio output device
    
    propertyAddress.mSelector = kAudioDevicePropertyBufferSize; // ask to modify buffersize
    ioDataSize = sizeof( UInt32 ); deviceBufferSize = (UInt32)audioBufferSize;
    
    err = AudioObjectSetPropertyData( device, &propertyAddress, 0, NULL, ioDataSize, &deviceBufferSize );
    if( err != kAudioHardwareNoError ) return false; // send the request to our own audio output device
    
    err = AudioDeviceCreateIOProcID( device, ioProc, (void *)this, &deviceIOProcId );
    if( err != kAudioHardwareNoError ) return false; // create the audio callback
    
    userTimeInterval = timeInterval; // the time interval is set to user one
    userCallbackTime = timeInterval; // first time to catch = 0 + interval
    userCallback = callback; // we store the user callback
    userAppPtr = appPtr; // we store the user app pointer
    
    return true;
}

void cOo::AudioTimer::moveOffset( double offsetTime ) {

    audioCallbackTime = offsetTime; // move things
    userCallbackTime = offsetTime + userTimeInterval;
}

bool cOo::AudioTimer::isRunning( void ) {

    return( running );
}

double cOo::AudioTimer::getTime( void ) {

    return( audioCallbackTime );
}

void cOo::AudioTimer::start( void ) {

    AudioDeviceStart( device, deviceIOProcId );
    running = true;
}

void cOo::AudioTimer::stop( void ) {

    AudioDeviceStop( device, deviceIOProcId );
    running = false;
}

OSStatus cOo::AudioTimer::ioProc( AudioDeviceID inDevice,
const AudioTimeStamp *inNow, const AudioBufferList *inInputData,
const AudioTimeStamp *inInputTime, AudioBufferList *outOutputData,
const AudioTimeStamp *inOutputTime, void* defptr ) {
    
    AudioTimer *timer = (AudioTimer *) defptr;
    
    if( timer->audioCallbackTime > timer->userCallbackTime ) {
        
        timer->userCallback( timer->userAppPtr ); // callback
        timer->userCallbackTime += timer->userTimeInterval;
    }
    timer->audioCallbackTime += ( (double)timer->deviceBufferSize / timer->deviceSampleRate ) / 8.0f;
    return kAudioHardwareNoError; // we increment the time stamp each time audio is callbacked
}

#include "cOoAudioTimer.h"

void cOo::TimerThread::toClass( AudioTimer *at, int step ) {
    
    audioTimer = at;
    stepInMs = step;
}

void cOo::TimerThread::threadedFunction( void ) {

    while( isThreadRunning() ) {
        
        audioTimer->timeInc();
        sleep( 10 );
    }
}

cOo::AudioTimer::AudioTimer( void ) {
    
    audioCallbackTime = 0.0f;
    userCallback = NULL;
    running = false;
}

bool cOo::AudioTimer::setup( int audioBufferSize, double timeInterval, UserCallback callback, void *appPtr ) {
    
    deviceSampleRate = 44100.0f;
    deviceBufferSize = audioBufferSize;
    
    userTimeInterval = timeInterval; // the time interval is set to user one
    userCallbackTime = timeInterval; // first time to catch = 0 + interval
    userCallback = callback; // we store the user callback
    
    timerThread.toClass( this, (int)(1000.0f*timeInterval) );
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

    timerThread.startThread();
    running = true;
}

void cOo::AudioTimer::stop( void ) {

    timerThread.waitForThread();
    running = false;
}

void cOo::AudioTimer::timeInc( void ) {

    if( audioCallbackTime > userCallbackTime ) {
        
        userCallback( userAppPtr ); // callback
        userCallbackTime += userTimeInterval;
    }
    
    audioCallbackTime += ( (double)deviceBufferSize / deviceSampleRate );
}

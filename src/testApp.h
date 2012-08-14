#pragma once

#include <list>
#include <vector>

#include "ofMain.h"
#include "ofxOsc.h"

#include "cOoAudioTimer.h"
#include "cOoFunctionTimeline.h"
#include "cOoSketchedCurve.h"
#include "cOoScreenMapper.h"

using namespace cOo;

class testApp : public ofBaseApp {
    
  public:
    
    void setup( void );
    void exit( void );
    
    void update( void );
    void draw( void );
    
    void movePlaybackTime( Time time );
    void zoomTimeline( double factor );
    void moveTimeline( Time shift );
    
    void startPlayback( void );
    void pausePlayback( void );
    void stopPlayback( void );
    bool isPlaying( void );

    void keyPressed( int key );
    void keyReleased( int key );
    
    void mousePressed( int x, int y, int button );
    void mouseReleased( int x, int y, int button );
    void mouseDragged( int x, int y, int button );
    void mouseMoved( int x, int y );
    
    void dragEvent( ofDragInfo dragInfo );
    void windowResized( int w, int h );
    void gotMessage( ofMessage msg );
    
  protected:
    
    // - method to send OSC messages -
    void sendTouchedAsOscMessages( void );
    
    // - methods to go switch between x pixels and time --
    float getXfromTime( Time time, Time offset, double pps );
    Time getTimefromX( float x, Time offset, double pps );
    
    // - methods to manipulate the playback -
    static void playbackTimeInc( void *usrPtr );
    
    // - timeline of breakpoint functions ---
    FunctionTimeline timeline; // BPF timeline
    
    // - duplicated touched ( for memory barriers ) ----
    vector<Record> tTouched, uTouched, dTouched, sTouched;
    
    // - display -------------------
    list<SketchedCurve> sketchedCurve;
    ScreenMapper screenMapper;
    
    // - OSC interface -----
    ofxOscReceiver oscReceiver;
    ofxOscSender oscSender;
    ofxOscMessage message;
    
    // - playback -
    AudioTimer timer;
    ofMutex playbackAccess;
    Time playbackTime;
    bool playAsLoop;
    
    double zoomFactor;
    bool isSliding;
    bool drawBPFs;
};

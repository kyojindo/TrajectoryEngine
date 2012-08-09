#pragma once

#include <list>
#include <vector>
#include <deque>

#include "ofMain.h"
#include "cOoFunctionTimeline.h"
#include "cOoAudioTimer.h"
#include "ofxOsc.h"

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
    list<BreakPointFunction *>::iterator bpf;
    vector<Record> touched; // touched sets
    
    // - OSC interface --
    ofxOscSender oscSender;
    ofxOscReceiver oscReceiver;
    ofxOscMessage message;
    
    // - playback ---
    Time playbackTime;
    AudioTimer timer;
    bool playAsLoop;
    
    // - display --
    double timeOffset;
    double pixelPerSec;
    bool drawBPFs;
};

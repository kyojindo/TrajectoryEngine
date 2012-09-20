#pragma once

#include <list>
#include <vector>

#include "ofMain.h"
#include "ofxOsc.h"

#include "cOoAudioTimer.h"
#include "cOoFunctionTimeline.h"
#include "cOoSketchedCurve.h"
#include "cOoScreenMapper.h"
#include "cOoVuzikXML.h"
#include "cOoColorMap.h"

using namespace cOo;

const int WHOLETONE = 5; const int CHROMATIC = 7;
const int scaleTableSize = (int)VUZIK_PITCH_MAX;

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
    
    static void playbackTimeInc( void *usrPtr ); // increment the pb head position
    void sendTouchedAsOscMessages( void ); // send OSC messages for touched sets
    void regenerateVisibleCurves( void ); // regenerate visible FBO curves
    
    FunctionTimeline timeline; // timeline of breakpoint functions
    vector<Record> tTouched, uTouched, dTouched, sTouched; // touched
    
    ScreenMapper screenMapper; // time/screen mapping object
    list<SketchedCurve> sketchedCurve; // curve rendering obj
    
    AudioTimer timer; // audio-based timer with callback registration
    ofMutex playbackAccess; // mutex for accessing the playback head
    Time playbackTime; // playback head in seconds ( shared mem )
    bool playAsLoop; // flag to set if we play as loop or not
    
    double zoomFactor; // stored zoom factor on the score
    bool fullScreen; // flag to redraw all the curves
    ColorMap colorMap; // load the color map
    
    ofxOscReceiver oscReceiver; // OSC receiver
    ofxOscSender oscSender; // OSC sender
    ofxOscMessage message; // message
    
    float scaleChromatic[scaleTableSize];
    float scaleWholeTone[scaleTableSize];
    
    bool showSplashScreen;
    ofImage splashScreen;
};

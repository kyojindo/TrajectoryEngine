#include "testApp.h"

void testApp::setup( void ) {
    
    ofSetFrameRate( 25 ); ofBackground( 30, 30, 30 );
    ofEnableAlphaBlending(); ofEnableSmoothing();
    
    timeline.load( 32*4, 64, 60.0f ); // [TODO] load( filename vuzikFile )
    timer.setup( 128, 0.01, &playbackTimeInc, this ); // register the callback
    
    oscSender.setup( "127.0.0.1", 7000 ); // send OSC on port 7000
    oscReceiver.setup( 8000 ); // receive OSC on port 8000
    
    zoomTimeline( 1.0f );
    timeOffset = 0.0;
    
    playAsLoop = false;
    drawBPFs = true;
}

void testApp::exit( void ) {
    
    if( timer.isRunning() ) timer.stop();
}

void testApp::update( void ) {
    
    ofxOscMessage m;
    
    while( oscReceiver.hasWaitingMessages() ) {
        
        oscReceiver.getNextMessage( &m );
        
        if( m.getAddress() == "/play" ) startPlayback();
        if( m.getAddress() == "/stop" ) stopPlayback();
        if( m.getAddress() == "/pause" ) pausePlayback();
        
        if( m.getAddress() == "/move" ) movePlaybackTime( (Time)m.getArgAsFloat( 0 ) );
        
        if( m.getAddress() == "/zoom" ) zoomTimeline( (double)m.getArgAsFloat( 0 ) );
        if( m.getAddress() == "/shift" ) moveTimeline( (Time)m.getArgAsFloat( 0 ) );
    }
}

void testApp::draw( void ) {
    
    int bpfColor;
    float xVal, yVal, rVal;
    Record current, previous;
    float xPrevious, yPrevious;
    float xCurrent, yCurrent;
    float yTouch, tVal;
    
    if( drawBPFs ) {
        
        for( bpf=timeline.getBegin(); bpf!=timeline.getEnd(); bpf++ ) {
            
            if( (*bpf)->isActive() ) bpfColor = 200;
            else bpfColor = 100; // active = light
            
            // line between each point of the BPF
            for( long k=1; k<(*bpf)->getSize(); k++ ) {
                
                (*bpf)->getRecord( k-1, previous );
                (*bpf)->getRecord( k, current );
                
                yPrevious = ofMap( previous.data.getPitch(), 0, 1, ofGetHeight(), 0 );
                xPrevious = getXfromTime( previous.time, timeOffset, pixelPerSec );
                
                yCurrent = ofMap( current.data.getPitch(), 0, 1, ofGetHeight(), 0 );
                xCurrent = getXfromTime( current.time, timeOffset, pixelPerSec );
                
                ofSetColor( bpfColor, bpfColor, bpfColor, 200 );
                ofLine( xPrevious, yPrevious, xCurrent, yCurrent );
            }
        }
    }
    
    // get playback head location
    tVal = getXfromTime( playbackTime, timeOffset, pixelPerSec );
    
    if( tVal > ofGetWidth()/2 && timeline.getMaxTime() > getTimefromX( ofGetWidth(), timeOffset, pixelPerSec ) ) {
    
        // and shift the time offset to stay centered @ playback
        timeOffset -= playbackTime-getTimefromX( ofGetWidth()/2, timeOffset, pixelPerSec );
    }
    
    ofSetColor( 200, 200, 200 ); ofLine( tVal, 0, tVal, ofGetHeight() );
    
    // draw circles for touched sets
    for( long k=0; k<touched.size(); k++ ) {
        
        yTouch = ofMap( touched[k].data.getPitch(), 0, 1, ofGetHeight(), 0 );
        
        ofNoFill(); ofSetColor( 250, 250, 250, 220 );
        ofCircle( tVal, yTouch, 6 );
    }
    
    ofSetColor( 200, 200, 200 ); // draw playback time
    ofDrawBitmapString( ofToString( playbackTime ), 25, 30 );
    ofDrawBitmapString( ofToString( touched.size() ), 25, 60 );
}

void testApp::movePlaybackTime( Time time ) {
    
    timer.moveOffset( time );
    playbackTime = timer.getTime();
    timeline.getTouched( touched, time );
    sendTouchedAsOscMessages();
}

void testApp::zoomTimeline( double factor ) {
    
    // no no, we cannot unzoom to infinite
    if( factor < 0.000001f ) factor = 0.000001f;
    
    // save the pixel position of the playback time in current window
    float pOffset = getXfromTime( playbackTime, timeOffset, pixelPerSec );
    
    // update boundaries from the new zooming factor
    pixelPerSec = factor * ( ofGetWidth() / 10.0f );
    
    // compute time of the previous playback position in new window
    double newPbt = getTimefromX( pOffset, timeOffset, pixelPerSec );
    
    // shift offset to preserve position
    timeOffset += ( newPbt-playbackTime );
}

void testApp::moveTimeline( Time shift ) {

    timeOffset += shift;
}

void testApp::startPlayback( void ) {

    if( !timer.isRunning() ) timer.start();
}

void testApp::pausePlayback( void ) {

    if( timer.isRunning() ) timer.stop();
}

void testApp::stopPlayback( void ) {

    if( timer.isRunning() ) timer.stop();
    movePlaybackTime( 0.0f );
}

bool testApp::isPlaying( void ) {

    return( timer.isRunning() );
}

void testApp::keyPressed( int key ) {
    
    if( key == ' ' ) {
        
        if( !isPlaying() ) startPlayback();
        else if( isPlaying() ) pausePlayback();
    }

    if( key == '+' ) zoomTimeline( 2.0 );
    if( key == '-' ) zoomTimeline( 0.5 );
    if( key == '=' ) zoomTimeline( 1.0 );
    
    if( key == '>' ) moveTimeline( 0.1f );
    if( key == '<' ) moveTimeline( -0.1f );
}

void testApp::keyReleased( int key ){

    
}

void testApp::mousePressed( int x, int y, int button ) {
    
    movePlaybackTime( getTimefromX( x, timeOffset, pixelPerSec ) );
}

void testApp::mouseReleased( int x, int y, int button ) {
    
    
}

void testApp::mouseDragged( int x, int y, int button ) {
    
    movePlaybackTime( getTimefromX( x, timeOffset, pixelPerSec ) );
}

void testApp::mouseMoved( int x, int y ) {
    
    
}

void testApp::windowResized( int w, int h ) {

    
}

void testApp::dragEvent( ofDragInfo dragInfo ) { 

    
}

void testApp::gotMessage( ofMessage msg ) {
    
    
}

void testApp::sendTouchedAsOscMessages( void ) {
    
    for( long k=0; k<touched.size(); k++ ) {
        
        message.clear();
        message.setAddress( "/bpf" );
        
        message.addIntArg( touched[k].id );
        message.addStringArg( touched[k].type );
        message.addFloatArg( touched[k].data.getPitch() );
        message.addFloatArg( touched[k].data.getVelocity() );
        
        oscSender.sendMessage( message );
    }
}

float testApp::getXfromTime( Time time, Time offset, double pps ) {
    
    return( ofMap( time, -offset, 1-offset, 0, pps ) );
}

Time testApp::getTimefromX( float x, Time offset, double pps ) {
    
    return( (Time)ofMap( x, 0, pps, -offset, 1-offset ) );
}

void testApp::playbackTimeInc( void *usrPtr ) {
    
    testApp *app = (testApp *)usrPtr;
    
    app->playbackTime = app->timer.getTime();
    app->timeline.getNextTouched( app->touched, app->playbackTime );
    app->sendTouchedAsOscMessages();
    
    if( app->playbackTime > app->timeline.getMaxTime() ) {
        
        if( !app->playAsLoop ) app->timer.stop();
        app->movePlaybackTime( 0.0f );
        app->timeOffset = 0.0f;
    }
}

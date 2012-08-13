#include "testApp.h"

void testApp::setup( void ) {
    
    ofSetFrameRate( 25 ); ofBackground( 30, 30, 30 );
    ofEnableAlphaBlending(); ofEnableSmoothing();
    
    timeline.load( 8*10, 20, 50.0f ); // [TODO] load( filename vuzikFile )
    timer.setup( 128, 0.01, &playbackTimeInc, this ); // register the callback
    
    oscSender.setup( "127.0.0.1", 7000 ); // send OSC on port 7000
    oscReceiver.setup( 8000 ); // receive OSC on port 8000
    
    zoomTimeline( 1.0f ); timeOffset = 0.0;
    
    zoomFactor = 1.0f; playAsLoop = true;
    isSliding = false; drawBPFs = true;
}

void testApp::exit( void ) {
    
    if( isPlaying() ) pausePlayback();
}

void testApp::update( void ) {
    
    ofxOscMessage m;
    
    while( oscReceiver.hasWaitingMessages() ) {
        
        oscReceiver.getNextMessage( &m );
        
        if( m.getAddress() == "/play" ) startPlayback();
        if( m.getAddress() == "/stop" ) stopPlayback();
        if( m.getAddress() == "/pause" ) pausePlayback();
        
        if( m.getAddress() == "/zoom" ) zoomTimeline( (double)m.getArgAsFloat( 0 ) );
        if( m.getAddress() == "/shift" ) moveTimeline( (Time)m.getArgAsFloat( 0 ) );
        
        if( m.getAddress() == "/move" ) movePlaybackTime( (Time)m.getArgAsFloat( 0 ) );
    }
}

void testApp::draw( void ) {
    
    int bpfColor;
    
    float xVal, yVal, rVal;
    Record current, previous;
    
    float xPrevious, yPrevious;
    float xCurrent, yCurrent;
    
    float xTouch, yTouch;
    
    float tVal;
    
    OSMemoryBarrier();
    dTouched = sTouched;
    
    if( drawBPFs ) {
        
        for( bpf=timeline.getBegin(); bpf!=timeline.getEnd(); bpf++ ) {
            
            if( (*bpf)->isActive() ) bpfColor = 200;
            else bpfColor = 100; // active = light
            
            // line between each point of the BPF
            for( long k=1; k<(*bpf)->getSize(); k++ ) {
                
                (*bpf)->getRecord( k-1, previous );
                (*bpf)->getRecord( k, current );
                
                yPrevious = ofMap( previous.data.getPitch(), 0, 1, ofGetHeight(), 0 );
                xPrevious = getXfromTime( previous.data.time, timeOffset, pixelPerSec );
                
                yCurrent = ofMap( current.data.getPitch(), 0, 1, ofGetHeight(), 0 );
                xCurrent = getXfromTime( current.data.time, timeOffset, pixelPerSec );
                
                ofSetColor( bpfColor, bpfColor, bpfColor, 200 );
                ofLine( xPrevious, yPrevious, xCurrent, yCurrent );
            }
            
            // circle on each point of the BPF
            for( long k=0; k<(*bpf)->getSize(); k++ ) {
                
                (*bpf)->getRecord( k, current );
                
                yCurrent = ofMap( current.data.getPitch(), 0, 1, ofGetHeight(), 0 );
                xCurrent = getXfromTime( current.data.time, timeOffset, pixelPerSec );
                
                ofSetColor( bpfColor, bpfColor, bpfColor, 220 );
                ofNoFill(); ofCircle( xCurrent, yCurrent, 3 );
            }
        }
    }
    
    playbackAccess.lock();
    
    // get the playback head position on the screen
    tVal = getXfromTime( playbackTime, timeOffset, pixelPerSec );
    isSliding = false; // tricky flag to cheat on position
    
    if( tVal > ofGetWidth()/2 && timeline.getMaxTime() >
    getTimefromX( ofGetWidth(), timeOffset, pixelPerSec ) ) {
    
        // shift the time offset to stay centered @ playback
        timeOffset -= playbackTime-getTimefromX( ofGetWidth()/2,
        timeOffset, pixelPerSec ); isSliding = true;
    }
    
    playbackAccess.unlock();
    
    ofSetColor( 200, 200, 200 ); // line
    ofLine( tVal, 0, tVal, ofGetHeight() );
    
    // draw circles for touched sets
    for( long k=0; k<dTouched.size(); k++ ) {
        
        yTouch = ofMap( dTouched[k].data.getPitch(), 0, 1, ofGetHeight(), 0 );
        xTouch = getXfromTime( dTouched[k].data.time, timeOffset, pixelPerSec );
        
        if( isSliding ) xTouch += 4; // trick to avoid circles to be behind
        
        ofNoFill(); ofSetColor( 250, 250, 250, 220 );
        ofCircle( xTouch, yTouch, 6 );
    }
    
    ofSetColor( 200, 200, 200 ); // draw playback time
    
    playbackAccess.lock();
    ofDrawBitmapString( ofToString( playbackTime ), 25, 30 );
    playbackAccess.unlock();
    
    ofDrawBitmapString( ofToString( dTouched.size() ), 25, 60 );
}

void testApp::movePlaybackTime( Time time ) {
    
    OSMemoryBarrier(); uTouched = tTouched;
    
    timer.moveOffset( time ); // we move timer at new time
    
    playbackAccess.lock();
    playbackTime = timer.getTime(); // we change playback position
    playbackAccess.unlock();
    
    timeline.getTouched( uTouched, time ); // here we get the touched directly
    sendTouchedAsOscMessages(); // then we send OSC messages to report that change
    timeline.cleanEndTouched( uTouched ); // and we clean 'atEnd' ones right away
    
    OSMemoryBarrier(); tTouched = uTouched;    
    OSMemoryBarrier(); sTouched = tTouched;
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

    if( key == '+' ) { zoomFactor+=0.1; zoomTimeline( zoomFactor ); }
    if( key == '-' ) { zoomFactor-=0.1; zoomTimeline( zoomFactor ); }
    if( key == '=' ) { zoomFactor=1.0; zoomTimeline( zoomFactor ); }
    
    if( key == '<' ) moveTimeline( -0.1f );
    if( key == '>' ) moveTimeline( 0.1f );
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
    
    for( long k=0; k<tTouched.size(); k++ ) {
        
        message.clear();
        message.setAddress( "/bpf" );
        
        message.addIntArg( tTouched[k].id );
        message.addIntArg( tTouched[k].type );
        
        message.addFloatArg( tTouched[k].data.getPitch() );
        message.addFloatArg( tTouched[k].data.getVelocity() );
        message.addIntArg( tTouched[k].state );
        
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
    
    app->playbackAccess.lock();
    
    if( app->playbackTime > app->timeline.getMaxTime() ) {
        
        app->playbackAccess.unlock();
        if( !app->playAsLoop ) app->timer.stop();
        app->movePlaybackTime( 0.0f );
        app->timeOffset = 0.0f;
        
    } else {
    
        app->playbackTime = app->timer.getTime();
        app->timeline.getNextTouched( app->tTouched, app->playbackTime );
        app->playbackAccess.unlock();
        
        OSMemoryBarrier();
        app->sTouched = app->tTouched;
        
        app->sendTouchedAsOscMessages();
    }
}

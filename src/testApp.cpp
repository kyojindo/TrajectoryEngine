#include "testApp.h"

void testApp::setup( void ) {
    
    list<SketchedCurve>::iterator skc;
    list<BreakPointFunction *>::iterator bpf;
    
    ofEnableAlphaBlending(); ofEnableSmoothing();
    ofSetFrameRate( 120 ); ofBackground( 10, 10, 10 );
    
    //timeline.generate( 16, 10, 30, 60.0f ); // generate a given random score
    timeline.load(16, 10, 30);
    timer.setup( 128, 0.005, &playbackTimeInc, this ); // register the callback
    sketchedCurve.resize( timeline.getSize() ); // resize the BPF-rendering
    
    // link each BPF to its FBO rendering object and give it a first full rendering
    for( bpf=timeline.getBegin(), skc=sketchedCurve.begin(); bpf!= timeline.getEnd();
    bpf++, skc++ ) { (*skc).link( (*bpf), &screenMapper ); } // -------------------
    
    oscSender.setup( "127.0.0.1", 7000 ); // send OSC on port 7000
    oscReceiver.setup( 8000 ); // receive OSC on port 8000
    
    zoomFactor = 1.0f; // zoom
    zoomTimeline( zoomFactor );
    
    fullScreen = false;
    playAsLoop = false;
}

void testApp::exit( void ) {
    
    if( isPlaying() ) pausePlayback();
}

void testApp::update( void ) {
    
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    ofxOscMessage m;
    
    while( oscReceiver.hasWaitingMessages() ) {
        
        oscReceiver.getNextMessage( &m );
        
        if( m.getAddress() == "/play" ) startPlayback();
        if( m.getAddress() == "/pause" ) pausePlayback();
        if( m.getAddress() == "/stop" ) stopPlayback();
        
        if( m.getAddress() == "/move" ) movePlaybackTime( (Time)m.getArgAsFloat( 0 ) );
        
        if( m.getAddress() == "/zoom" ) zoomTimeline( (double)m.getArgAsFloat( 0 ) );
        if( m.getAddress() == "/shift" ) moveTimeline( (Time)m.getArgAsFloat( 0 ) );
    }
}

void testApp::draw( void ) {
    
    float tVal, xTouch, yTouch;
    list<SketchedCurve>::iterator skc;
    
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA,
    GL_ONE_MINUS_CONSTANT_ALPHA );
    
    for( int k=0; k<33; k++ ) {
    
        ofSetColor( 255, 255, 255, 30 );
        ofLine( 0, ofMap( k, 0, 32, 0, ofGetHeight() ),
        ofGetWidth(), ofMap( k, 0, 32, 0, ofGetHeight() ) );
    }
    
    OSMemoryBarrier();
    dTouched = sTouched;
    
    playbackAccess.lock(); // get the head position
    tVal = screenMapper.getXfromTime( playbackTime );
    playbackAccess.unlock();
    
    ofSetColor( 255, 255, 255, 120 ); // line
    ofLine( tVal, 0, tVal, ofGetHeight() );
    
    for( skc=sketchedCurve.begin();
    skc!=sketchedCurve.end(); skc++ ) {
    
        (*skc).draw();
    }
    
    // draw circles for touched sets
    for( long k=0; k<dTouched.size(); k++ ) {
        
        yTouch = ofMap( dTouched[k].data.getPitch(), 0, 33, ofGetHeight(), 0 );
        xTouch = screenMapper.getXfromTime( dTouched[k].data.time );
        
        color.setHue( ofMap( dTouched[k].type, 0, 3, 0, 128 ) );
        color.setBrightness( 255 ); color.setSaturation( 200 );
        
        ofNoFill();
        ofSetColor( color, 200 );
        ofCircle( xTouch, yTouch, 13 );
        ofCircle( xTouch, yTouch, 14 );
        ofCircle( xTouch, yTouch, 15 );
    }
    
    playbackAccess.lock();
    ofSetColor( 200, 200, 200 ); // draw playback time
    ofDrawBitmapString( ofToString( playbackTime ), 20, 30 );
    playbackAccess.unlock();
    
    // manage the shifting of the screen ~ playback
    if( tVal > ofGetWidth()/2 && timeline.getMaxTime() >
       screenMapper.getTimefromX( ofGetWidth() ) ) {
        
        playbackAccess.lock();
        screenMapper.incTimeOffset( -( playbackTime-
        screenMapper.getTimefromX( ofGetWidth()/2 ) ) );
        playbackAccess.unlock();
    }
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
    
    if( factor < 0.000001f ) factor = 0.000001f;
    
    float pOffset = screenMapper.getXfromTime( playbackTime );
    screenMapper.setPixelPerSec( factor * ( ofGetWidth() / 20.0f ) );
    double newPbt = screenMapper.getTimefromX( pOffset );
    screenMapper.incTimeOffset( newPbt-playbackTime );
    
    regenerateVisibleCurves();
}

void testApp::moveTimeline( Time shift ) {

    screenMapper.incTimeOffset( shift );
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
    
    if( key == 'f' ) {
        
        fullScreen = !fullScreen;
        ofSetFullscreen( fullScreen );
        
        if( fullScreen ) ofHideCursor();
        else ofShowCursor();
    }

    if( key == '+' ) { zoomFactor+=0.1; zoomTimeline( zoomFactor ); }
    if( key == '-' ) { zoomFactor-=0.1; zoomTimeline( zoomFactor ); }
    if( key == '=' ) { zoomFactor=1.0; zoomTimeline( zoomFactor ); }
    
    if( key == '<' ) moveTimeline( -0.1f );
    if( key == '>' ) moveTimeline( 0.1f );
}

void testApp::keyReleased( int key ){
    
    if( key == 'f' ) regenerateVisibleCurves();
}

void testApp::mousePressed( int x, int y, int button ) {
    
    movePlaybackTime( screenMapper.getTimefromX( x ) );
}

void testApp::mouseReleased( int x, int y, int button ) {
    
    
}

void testApp::mouseDragged( int x, int y, int button ) {
    
    movePlaybackTime( screenMapper.getTimefromX( x ) );
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

void testApp::playbackTimeInc( void *usrPtr ) {
    
    testApp *app = (testApp *)usrPtr;
    
    app->playbackAccess.lock();
    
    if( app->playbackTime > app->timeline.getMaxTime() ) {
        
        app->playbackAccess.unlock();
        
        if( !app->playAsLoop ) app->timer.stop();
        
        app->movePlaybackTime( 0.0f );
        app->screenMapper.setTimeOffset( 0.0f );
        
    } else {
    
        app->playbackTime = app->timer.getTime();
        app->timeline.getNextTouched( app->tTouched, app->playbackTime );
        app->playbackAccess.unlock();
        
        OSMemoryBarrier(); app->sTouched = app->tTouched;
        
        app->sendTouchedAsOscMessages();
    }
}

void testApp::regenerateVisibleCurves( void ) {

    list<SketchedCurve>::iterator skc;
    
    for( skc=sketchedCurve.begin(); skc!=sketchedCurve.end(); skc++ ) {
        
        if( (*skc).isVisible() ) (*skc).generate();
    }
}

#include "testApp.h"

void testApp::setup( void ) {
    
    list<SketchedCurve>::iterator skc;
    list<BreakPointFunction *>::iterator bpf;
    
    ofEnableAlphaBlending(); ofEnableSmoothing();
    ofSetFrameRate( 25 ); ofBackground( 30, 30, 30 );
    

    timeline.load( 8*10, 32, 20.0f ); // load the XML file with th score
    timer.setup( 128, 0.01, &playbackTimeInc, this ); // register the callback
    sketchedCurve.resize( timeline.getSize() ); // resize the BPF-rendering
    
    for( bpf=timeline.getBegin(), skc=sketchedCurve.begin(); bpf!=timeline.getEnd(); bpf++, skc++ ) {
        
        (*skc).set( (*bpf), &screenMapper ); // link every BPF to its rendering object
    }
    
    oscSender.setup( "127.0.0.1", 7000 ); // send OSC on port 7000
    oscReceiver.setup( 8000 ); // receive OSC on port 8000
    
    zoomFactor = 1.0f; playAsLoop = true;
    isSliding = false; drawBPFs = true;
    
    zoomTimeline( zoomFactor );
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
    
    float tVal, xTouch, yTouch;
    list<SketchedCurve>::iterator skc;
    
    OSMemoryBarrier(); dTouched = sTouched;
    
    for( skc=sketchedCurve.begin();
    skc!=sketchedCurve.end(); skc++ ) {
    
        (*skc).draw();
    }
    
    playbackAccess.lock();
    
    // get the playback head position on the screen
    tVal = screenMapper.getXfromTime( playbackTime );
    isSliding = false; // tricky flag to cheat on position
    
    if( tVal > ofGetWidth()/2 && timeline.getMaxTime() >
    screenMapper.getTimefromX( ofGetWidth() ) ) {
        
        screenMapper.incTimeOffset( -( playbackTime-
        screenMapper.getTimefromX( ofGetWidth()/2 ) ) );
        isSliding = true; // shift to stay centered
    }
    
    playbackAccess.unlock();
    
    ofSetColor( 200, 200, 200 ); // line
    ofLine( tVal, 0, tVal, ofGetHeight() );
    
    // draw circles for touched sets
    for( long k=0; k<dTouched.size(); k++ ) {
        
        yTouch = ofMap( dTouched[k].data.getPitch(), 0, 1, ofGetHeight(), 0 );
        xTouch = screenMapper.getXfromTime( dTouched[k].data.time );
        
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
    
    if( factor < 0.000001f ) factor = 0.000001f;
    float pOffset = screenMapper.getXfromTime( playbackTime );
    screenMapper.setPixelPerSec( factor * ( ofGetWidth() / 10.0f ) );
    double newPbt = screenMapper.getTimefromX( pOffset );
    screenMapper.incTimeOffset( newPbt-playbackTime );
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

    if( key == '+' ) { zoomFactor+=0.1; zoomTimeline( zoomFactor ); }
    if( key == '-' ) { zoomFactor-=0.1; zoomTimeline( zoomFactor ); }
    if( key == '=' ) { zoomFactor=1.0; zoomTimeline( zoomFactor ); }
    
    if( key == '<' ) moveTimeline( -0.1f );
    if( key == '>' ) moveTimeline( 0.1f );
}

void testApp::keyReleased( int key ){

    
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
        app->screenMapper.setTimeOffset( 0.0f );
        
    } else {
    
        app->playbackTime = app->timer.getTime();
        app->timeline.getNextTouched( app->tTouched, app->playbackTime );
        app->playbackAccess.unlock();
        
        OSMemoryBarrier();
        app->sTouched = app->tTouched;
        
        app->sendTouchedAsOscMessages();
    }
}

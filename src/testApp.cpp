#include "testApp.h"

double mvt1f = 0.825*(5*60+38)/483.6; //5:38
double mvt2f = (4*60+23)/156.88;//4:23
double mvt3f = (3*60+2)/165.3;//3:02

void testApp::setup( void ) {
    
    list<SketchedCurve>::iterator skc;
    list<BreakPointFunction *>::iterator bpf;
    
    ofSetFrameRate( 60 );
    ofBackground( 10, 10, 10 );
    
    for( int k=0; k<scaleTableSize; k++ ) {
    
        scaleWholeTone[k] = 24.0f + (float)(2*k);
        scaleChromatic[k] = 36.0f + (float)(k);
    }
    
    //loadScore("icmc/combined.xml", 1.0);
    //loadScore( "intertwine/smallPoints.xml", mvt1f );
    
    loadScore( "intertwine/1-Synchrome.xml", mvt1f );
    
//    //timeline.loadVuzikFile( "icmc/lineTypes.xml" ); // import the Vuzik
//    timeline.loadVuzikFile( "icmc/combined.xml" ); // import the Vuzik files
//    //timeline.loadVuzikFile( "calibration.xml" ); // import the Vuzik files
    timer.setup( 128, 0.01, &playbackTimeInc, this ); // register the callback
//    sketchedCurve.resize( timeline.getSize() ); // resize the BPF-rendering
//    
//    for( bpf=timeline.getBegin(), skc=sketchedCurve.begin(); bpf!= timeline.getEnd();
//    bpf++, skc++ ) (*skc).link( (*bpf), &screenMapper ); // link each BPF to its FBO    
    //oscSender.setup( "127.0.0.1", 7000 ); // send OSC on port 7000
    oscSender.setup( "192.168.1.255", 7000 ); // send OSC on port 7000
    //oscReceiver.setup( 8000 ); // receive OSC on port 8000 (local)
    
    zoomFactor = 1.0f; // set zoom factor to default
    zoomTimeline( zoomFactor ); // and apply the zoom
    
    splashScreen.loadImage( "splash.png" );
    showSplashScreen = false; // splash scr
    
    fullScreen = false;
    playAsLoop = false;
}

void testApp::loadScore(string fn, double timescale) {
    
    stopPlayback();
    timeline.clear();
    
    list<SketchedCurve>::iterator skc;
    list<BreakPointFunction *>::iterator bpf;
    timeline.loadVuzikFile( fn, timescale ); // import the Vuzik files
    sketchedCurve.resize( timeline.getSize() ); // resize the BPF-rendering
    
    for( bpf=timeline.getBegin(), skc=sketchedCurve.begin(); bpf!= timeline.getEnd();
        bpf++, skc++ ) (*skc).link( (*bpf), &screenMapper ); // link each BPF to its FBO
    
    zoomFactor = 1.5f; // set zoom factor to default
    zoomTimeline( zoomFactor ); // and apply the zoom
    
    splashScreen.loadImage( "splash.png" );
    showSplashScreen = false; // splash scr
    
    fullScreen = false;
    playAsLoop = false;
    
    movePlaybackTime(0.1);
    screenMapper.setTimeOffset( 0.0f );

}

void testApp::exit( void ) {
    
    // pause timer before quiting
    if( isPlaying() ) { pausePlayback(); }
}

void testApp::update( void ) {
    
    ofxOscMessage m;
    
    while( oscReceiver.hasWaitingMessages() ) {
        
        oscReceiver.getNextMessage( &m );
        
        if( m.getAddress() == "/play" ) startPlayback(); // start/resume the playback
        if( m.getAddress() == "/pause" ) pausePlayback(); // pause the playback at location
        if( m.getAddress() == "/stop" ) stopPlayback(); // stop the playback and reset
        
        if( m.getAddress() == "/move" ) movePlaybackTime( (Time)m.getArgAsFloat( 0 ) );
        if( m.getAddress() == "/zoom" ) zoomTimeline( (double)m.getArgAsFloat( 0 ) );
        if( m.getAddress() == "/shift" ) moveTimeline( (Time)m.getArgAsFloat( 0 ) );
    }
}

void testApp::draw( void ) {
    
    list<SketchedCurve>::iterator skc;
    
    glEnable( GL_LINE_SMOOTH ); glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    if( 0 ) {
        
        int x = ofGetWidth()/2 - splashScreen.width/2;
        int y = ofGetHeight()/2 - splashScreen.height/2;
        splashScreen.draw( x, y ); // show splash screen
        
    } else {
        
        for( int k=0; k<((int)VUZIK_PITCH_MAX)+1; k++ ) {
        
            // [TODO] replace this by a screenMapper-based call and stuff
            float semiLoc = ofMap( k, 0, VUZIK_PITCH_MAX, 0, ofGetHeight() );
            ofSetColor( 255, 255, 255, 30 ); ofSetLineWidth( 2 );
            ofLine( 0, semiLoc, ofGetWidth(), semiLoc );
        }
        
        Time secVal = 0.0f;
        float secLoc = 0.0f;
        
        while( secLoc < ofGetWidth() ) {
        
            secLoc = screenMapper.getXfromTime( secVal );
            secVal = secVal + 1.0f; ofSetColor( 255, 255, 255, 20 );
            ofSetLineWidth( 2 ); ofLine( secLoc, 0, secLoc, ofGetHeight() );
        }
        
        // memory-protected exchange of data
        OSMemoryBarrier(); dTouched = sTouched;
        
        playbackAccess.lock(); // playback time is locked
        float tVal = screenMapper.getXfromTime( playbackTime );
        playbackAccess.unlock(); // and used to update
        
        // we draw the line following the playback time
        ofSetColor( 255, 255, 255, 100 ); ofSetLineWidth( 2 );
        ofLine( tVal, 0, tVal, ofGetHeight() );
        
        for( skc=sketchedCurve.begin(); skc!=sketchedCurve.end();
        skc++ ) (*skc).draw(); // we draw the FBO-based curves
        
        // draw circles for touched data sets
        for( long k=0; k<dTouched.size(); k++ ) {
            
            if (dTouched[k].data.getVelocity() >0.105) {
                
                ofNoFill();
                ofColor tColor;
                
                float yTouch = ofMap( dTouched[k].data.getPitch(),
                VUZIK_PITCH_MIN, VUZIK_PITCH_MAX, ofGetHeight(), 0 );
                float xTouch = screenMapper.getXfromTime( dTouched[k].data.time );
                float tHue = colorMap.get( dTouched[k].type );
                
                if( tHue >= 0.0f ) tColor.setHsb( tHue, 140, 180 ); else tColor.setHsb( 0, 0, 180 );
                ofSetColor( tColor, 200 ); ofCircle( xTouch, yTouch, 13 );
                
                if( tHue >= 0.0f ) tColor.setHsb( tHue, 220, 220 ); else tColor.setHsb( 0, 0, 220 );
                ofSetColor( tColor, 200 ); ofCircle( xTouch, yTouch, 14 );
                
                if( tHue >= 0.0f ) tColor.setHsb( tHue, 180, 150 ); else tColor.setHsb( 0, 0, 150 );
                ofSetColor( tColor, 200 ); ofCircle( xTouch, yTouch, 15 );
            }
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
    
    showSplashScreen = false;
    if( !timer.isRunning() ) { timer.start(); }
}

void testApp::pausePlayback( void ) {

    if( timer.isRunning() ) { timer.stop(); }
}

void testApp::stopPlayback( void ) {

    if( timer.isRunning() ) { timer.stop(); }
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
    
    if( key == '1') {
        loadScore("intertwine/1-Synchrome.xml", mvt1f);
        zoomFactor = 0.75;
        zoomTimeline(zoomFactor);
    }
    if( key == '2') {
        loadScore("intertwine/2-Controverse.xml", mvt2f);
        zoomFactor = 0.75;
        zoomTimeline(zoomFactor);
    }
    if( key == '3') {
        loadScore("intertwine/3-Intertwine.xml", mvt3f);
        zoomFactor = 1.0;
        zoomTimeline(zoomFactor);

    }
}

void testApp::keyReleased( int key ){
    
    if( key == 'f' ) { usleep( 5000 ); regenerateVisibleCurves(); }
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
        
        float scaledPitch = 0.0f; int pIdx = (int) tTouched[k].data.getPitch();
        if( tTouched[k].data.getScale() == WHOLETONE ) scaledPitch = scaleWholeTone[pIdx];
        if( tTouched[k].data.getScale() == CHROMATIC ) scaledPitch = scaleChromatic[pIdx];
        
        message.addFloatArg( scaledPitch );
        message.addFloatArg( tTouched[k].data.getVelocity() );
        
        if( tTouched[k].craziness ) message.addIntArg( 1 );
        else message.addIntArg( 0 ); // convert craziness
        
        message.addIntArg( tTouched[k].state );
        
        oscSender.sendMessage( message );
    }
}

void testApp::playbackTimeInc( void *usrPtr ) {
    
    testApp *app = (testApp *)usrPtr;
    
    app->playbackAccess.lock();
    
    if( app->playbackTime > app->timeline.getMaxTime() ) {
        
        app->playbackAccess.unlock();
        
        if( !app->playAsLoop ) { app->timer.stop(); }
        
        app->movePlaybackTime( 0.0f );
        app->screenMapper.setTimeOffset( 0.0f );
        
        app->showSplashScreen = true;
        
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

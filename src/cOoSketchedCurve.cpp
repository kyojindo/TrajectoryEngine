#include "cOoSketchedCurve.h"

void cOo::SketchedCurve::link( BreakPointFunction *function, ScreenMapper *smap ) {

    visible = false; fbo = NULL;
    bpf = function; screenMapper = smap;
    ofSetLogLevel( OF_LOG_ERROR );
}

void cOo::SketchedCurve::generate( void ) {
    
    float radius, pitch, time;
    Record record; ofPath path; ofColor color;
    
    bool isChoirMob = false;
    int type = bpf->getType();
    //if ( (type == 0) || (type == 2) || (type == 5) ) {
        isChoirMob = true;
    //}
    int renderMode;
    

    bpf->getRecord(0, record);
    
    if (isChoirMob ) {
        if (record.data.getVelocity() > 0.995) renderMode = DRAW_HAIRY;
        else if (record.data.getVelocity() < 0.105) renderMode = DRAW_DOT;
    }
    else {
        if (record.data.getVelocity() > 0.995) renderMode = DRAW_BUBBLES;
        else if (record.data.getVelocity() < 0.105) renderMode = DRAW_HAIRY;
    }
        
    
    fboWidth = ( bpf->getMaxTime()-bpf->getMinTime() )
    * screenMapper->getPixelPerSec() + 128.0f;
    fboHeight = ofGetHeight();
    
    if( fbo != NULL ) delete fbo; fbo = new ofFbo();
    fbo->allocate( fboWidth, fboHeight, GL_RGBA32F_ARB );
    
    fbo->begin();
    
    ofClear( 255, 255, 255, 0 );
    glEnable( GL_LINE_SMOOTH ); glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );
    
    //if (renderMode== DRAW_NORMAL || renderMode == DRAW_HAIRY)
        
    for( int j=0; j<8; j++ ) {
    
        path.clear();
        path.setFilled( false );
        path.setStrokeWidth( 2.1f );
        
        bool drawOnce = false;
        
        float rAlpha = ofRandom( 100, 220 ); float rSat;
        float rHue = colorMap.get( bpf->getType() );
        
        if( rHue < 0.0f ) rSat = 0.0f;
        else rSat = ofRandom( 150, 220 );
        
        color.setHsb( rHue, rSat, 250, rAlpha );

        long bpf_n = bpf->getSize();
        if (bpf_n<4) path.setStrokeWidth(2.0);
        
        for( long k=0; k<bpf->getSize(); k++ ) {
            

            bpf->getRecord( k, record );
            
            //hairy cases
            if ( (isChoirMob && (record.data.velocity > 0.9975)) || (!isChoirMob && record.data.velocity < 0.1025) ) {
                radius = ofMap( record.data.getVelocity(), 0, 1, 0.1, 10 ) + ofRandom( -60, 60 );
                
            }
            else {	
                radius = ofMap( record.data.getVelocity(), 0, 1, 0.2, 8 ) + ofRandom( -4, 4 );
            }
           
            if( k == 0 ) {
                radius = 0.1;
            }
            
            if( k == bpf->getSize()-1 ) radius = 0.1;
            
            pitch = ofMap( record.data.getPitch(), VUZIK_PITCH_MIN,
            VUZIK_PITCH_MAX, ofGetHeight(), 0 ) + ofRandom( -radius, radius );
            
            time = ( screenMapper->getXfromTime( record.data.time ) -
            screenMapper->getXfromTime( bpf->getMinTime() ) ) + ofRandom( -radius, radius );
            
            if( k==0 ) {
                path.moveTo( time+64, pitch );
            }
            else {
                if (bpf_n<4) { //make curves at least 4 pts
                    for (int i=0; i<4-bpf_n; i++) {
                        path.curveTo( time+64-0.05+i*0.02, pitch + ofRandom(-radius, radius));
                    }
                }
                path.curveTo( time+64, pitch );
            }

            
            if (isChoirMob) {
                if (record.data.velocity < 0.1025){
                    //choirmob: thin = dotted
                    drawOnce = true; //stop drawing more lines on future passes
                    if (1) {
                        //set constant alpha for dots
                        color.setHsb( rHue, 255, 250, 255 );
                        ofFill();
                        ofSetColor(color);
                        ofSetCircleResolution(32);
                        ofCircle(time+64, pitch, 2.0);
                    }
                }
            }
            else { //MIXED PHASE
                //Mixed phase: thick = bubbles
                if (record.data.velocity > 0.9975) {
                    drawOnce = true;
                    radius = ofMap( record.data.getVelocity(), 0, 1, 2.0, 5.0 ) + ofRandom( -2, 2 );
                    //set constant alpha for bubbles
                    color.setHsb( rHue, 255, 250, 255 );
                    ofNoFill();
                    ofEnableSmoothing();
                    ofSetColor(color);
                    ofSetCircleResolution(64);
                    ofCircle(time+64, pitch, radius);
                }
            }
            
            
        }
        if (drawOnce) break;
        else {
            color.setHsb( colorMap.get( bpf->getType() ), rSat, 255, rAlpha );
            path.curveTo( time+64, pitch );
            path.setColor( color );
            path.draw();
        }
    }
    
    fbo->end();
}

void cOo::SketchedCurve::destroy( void ) {
    
    
}

void cOo::SketchedCurve::draw( void ) {
    
    float xMin = screenMapper->getXfromTime( bpf->getMinTime() );
    float xMax = screenMapper->getXfromTime( bpf->getMaxTime() );
    
    if( xMin > ofGetWidth() || xMax < 0 ) {
    
        if( fbo != NULL ) { delete fbo; fbo = NULL; }
        visible = false; // out of the window
        
    } else {
        
        if( !visible ) { generate(); visible = true; }
        
        if( bpf->isActive() ) ofSetColor( 255, 255, 255, 255 );
        else ofSetColor( 255, 255, 255, 150 ); // change alpha
        fbo->draw( xMin-64, 0 ); // redraw only the first time
    }
}

bool cOo::SketchedCurve::isVisible( void ) {

    return( visible );
}

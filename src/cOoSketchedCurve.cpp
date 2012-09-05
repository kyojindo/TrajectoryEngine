#include "cOoSketchedCurve.h"

void cOo::SketchedCurve::link( BreakPointFunction *function, ScreenMapper *smap ) {

    visible = false; fbo = NULL;
    bpf = function; screenMapper = smap;
    ofSetLogLevel( OF_LOG_ERROR );
}

void cOo::SketchedCurve::generate( void ) {
    
    float radius, pitch, time;
    Record record; ofPath path; ofColor color;
    
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
        
    for( int k=0; k<12; k++ ) {
    
        path.clear();
        path.setFilled( false );
        path.setStrokeWidth( 1.5f );
        
        float rSat = ofRandom( 150, 200 );
        float rAlpha = ofRandom( 20, 240 );
        
        bool drawOnce = false;
        
        color.setHsb( colorMap.get( bpf->getType() ), rSat, 255, rAlpha );
        
        for( long k=0; k<bpf->getSize(); k++ ) {
            
            bpf->getRecord( k, record );

            if( !bpf->isCrazy() ) radius = ofMap( record.data.getVelocity(), 0, 1, 0.2, 8 ) + ofRandom( -4, 4 );
            else radius = ofMap( record.data.getVelocity(), 0, 1, 0, 10 ) + ofRandom( -60, 60 );
            
            if( k == 0 ) {
                radius = 0.1;
                //path.setStrokeWidth(record.data.getVelocity()*2.0+2.0);
            }
            
            if (record.data.velocity == 0.1) {
                drawOnce = true; //stop drawing more lines on future passes
                path.setStrokeWidth(0.0 ); //don't draw any lines...
            }
                
            if( k == bpf->getSize()-1 ) radius = 0.1;
            
                
            
            pitch = ofMap( record.data.getPitch(), 0, 33,
            ofGetHeight(), 0 ) + ofRandom( -radius, radius );
            
            time = ( screenMapper->getXfromTime( record.data.time ) -
            screenMapper->getXfromTime( bpf->getMinTime() ) ) + ofRandom( -radius, radius );
            
            if( k==0 ) path.moveTo( time+64, pitch );
            else path.curveTo( time+64, pitch );
            
            if (record.data.velocity == 0.1) {
                drawOnce = true; //stop drawing more lines on future passes
                path.setStrokeWidth(0.0 ); //don't draw any lines...
                if (k%2) {
                    color.setHsb( colorMap.get( bpf->getType() ), 255, 255, 255 );
                    ofSetColor(color);

                    //ofFill();
                    //ofSetCircleResolution(64);
                    //ofCircle(time+64, pitch, radius*2.0);
                    ofEnableSmoothing();
                    ofNoFill();
                    ofSetCircleResolution(32);
                    ofCircle(time+64, pitch, radius*2.0);
                }
            }
        }
        if (drawOnce) break;
        else {
            path.curveTo( time+64, pitch );
            path.setColor( color );
            path.draw();
        }
        
        
    }
    
    fbo->end();
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

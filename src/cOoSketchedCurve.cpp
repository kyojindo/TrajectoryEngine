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
    
    //float rHue = ofRandom( 0, 255 );
    
    for( int k=0; k<50; k++ ) {
    
        path.clear();
        path.setFilled( false );
        
        float rSat = ofRandom( 100, 200 );
        float rAlpha = ofRandom( 20, 255 );
        
        for( long k=0; k<bpf->getSize(); k++ ) {
            
            bpf->getRecord( k, record );
            
            radius = ofMap( record.data.getVelocity(), 0, 1, 0, 2 ) + ofRandom( -5, 5 );
            if( k == 0 ) radius = 0.5; if( k == bpf->getSize()-1 ) radius = 0.5;
            
            pitch = ofMap( record.data.getPitch(), 0, 1,
            ofGetHeight(), 0 ) + ofRandom( -radius, radius );
            
            time = ( screenMapper->getXfromTime( record.data.time ) -
            screenMapper->getXfromTime( bpf->getMinTime() ) ) + ofRandom( -radius, radius );
            
            if( k==0 ) path.moveTo( time+64, pitch );
            else path.curveTo( time+64, pitch );
        }
        
        path.curveTo( time+64, pitch );
        //color.setHsb( rHue, rSat, 255, rAlpha );
        color.set( bpf->r, bpf->g, bpf->b, rAlpha );
        color.setSaturation( rSat );
        color.setBrightness( 255 );
        path.setColor( color );
        path.draw();
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

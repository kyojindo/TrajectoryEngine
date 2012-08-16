#include "cOoSketchedCurve.h"

void cOo::SketchedCurve::set( BreakPointFunction *function, ScreenMapper *smap ) {

    bpf = function; screenMapper = smap;
    ofSetLogLevel( OF_LOG_ERROR );
}

void cOo::SketchedCurve::redrawFbo( void ) {
    
    Record record; ofPath path; ofColor color;
    
    fboWidth = ( bpf->getMaxTime()-bpf->getMinTime() ) * screenMapper->getPixelPerSec() + 128;
    //if( fboWidth < 16.0f ) fboWidth = 16.0f; fboHeight = ofGetHeight(); // FBO size
    
    if( fbo ) delete fbo; fbo = new ofFbo(); // realloc
    fbo->allocate( fboWidth, fboHeight, GL_RGBA32F_ARB );
    
    fbo->begin();
    
    ofClear( 255, 255, 255, 0 );
    glEnable( GL_LINE_SMOOTH ); glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE );
    
    float rHue = ofRandom( 0, 255 );
    
    for( int k=0; k<32; k++ ) {
    
        path.clear();
        path.setFilled( false );
        
        float rSat = ofRandom( 100, 200 );
        float rAlpha = ofRandom( 50, 250 );
        
        for( long k=0; k<bpf->getSize(); k++ ) {
            
            bpf->getRecord( k, record );
            
            float radius = ofMap( record.data.getVelocity(), 0, 1, 1, 5 ) + ofRandom( -2, 2 );
            if( k == 0 ) radius = 0; if( radius == bpf->getSize()-1 ) radius = 0;
            
            float pitch = ofMap( record.data.getPitch(), 0, 1,
            ofGetHeight(), 0 ) + ofRandom( -radius,radius );
            
            float time = ( screenMapper->getXfromTime( record.data.time ) -
            screenMapper->getXfromTime( bpf->getMinTime() ) )
            + ofRandom( -radius,radius );
            
            if( k==0 ) path.moveTo( time, pitch );
            else path.curveTo( time, pitch );
        }
        
        //color.setHsb( rHue, rSat, 255, rAlpha );
        color.set( bpf->r, bpf->g, bpf->b, rAlpha );
        path.setColor( color ); path.draw();
    }
    
    fbo->end();
}

void cOo::SketchedCurve::draw( void ) {
    
    if( bpf->isActive() ) ofSetColor( 255, 255, 255, 255 );
    else ofSetColor( 255, 255, 255, 100 ); // active = bigger alpha
    
    float min = screenMapper->getXfromTime( bpf->getMinTime() );
    float max = screenMapper->getXfromTime( bpf->getMaxTime() );
    
    if( ( min < ofGetWidth() || max > 0 ) && bpf->getSize() > 2 ) {
        
        fbo->draw( min, 0 );
    }
}

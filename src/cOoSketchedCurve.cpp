#include "cOoSketchedCurve.h"

void cOo::SketchedCurve::set( BreakPointFunction *function, ScreenMapper *smap ) {

    screenMapper = smap;
    bpf = function;
}

void cOo::SketchedCurve::draw( void ) {

    if( bpf->isActive() ) bpfColor = 200;
    else bpfColor = 100; // active = light
    
    // line between each point of the BPF
    for( long k=1; k<bpf->getSize(); k++ ) {
        
        bpf->getRecord( k, current );
        bpf->getRecord( k-1, previous );
        
        yPrevious = ofMap( previous.data.getPitch(), 0, 1, ofGetHeight(), 0 );
        xPrevious = screenMapper->getXfromTime( previous.data.time );
        
        yCurrent = ofMap( current.data.getPitch(), 0, 1, ofGetHeight(), 0 );
        xCurrent = screenMapper->getXfromTime( current.data.time );
        
        ofSetColor( bpfColor, bpfColor, bpfColor, 200 );
        ofLine( xPrevious, yPrevious, xCurrent, yCurrent );
    }
    
    // circle on each point of the BPF
    for( long k=0; k<bpf->getSize(); k++ ) {
        
        bpf->getRecord( k, current );
        
        yCurrent = ofMap( current.data.getPitch(), 0, 1, ofGetHeight(), 0 );
        xCurrent = screenMapper->getXfromTime( current.data.time );
        
        ofSetColor( bpfColor, bpfColor, bpfColor, 220 );
        ofNoFill(); ofCircle( xCurrent, yCurrent, 3 );
    }
}

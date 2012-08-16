#include "cOoScreenMapper.h"

cOo::ScreenMapper::ScreenMapper( void ) {

    pixelPerSec = 50.0f;
    timeOffset = 0.0f;
}

void cOo::ScreenMapper::setPixelPerSec( double pps ) {

    pixelPerSec = pps;
}

void cOo::ScreenMapper::incTimeOffset( Time shift ) {

    timeOffset += shift;
}

void cOo::ScreenMapper::setTimeOffset( Time to ) {

    timeOffset = to;
}

double cOo::ScreenMapper::getPixelPerSec( void ) {

    return( pixelPerSec );
}

cOo::Time cOo::ScreenMapper::getTimeOffset( void ) {

    return( timeOffset );
}

float cOo::ScreenMapper::getXfromTime( Time time ) {

    return( ofMap( time, -timeOffset, 1-timeOffset, 0, pixelPerSec ) );
}

cOo::Time cOo::ScreenMapper::getTimefromX( float x ) {
    
    return( (Time)ofMap( x, 0, pixelPerSec, -timeOffset, 1-timeOffset ) );
}

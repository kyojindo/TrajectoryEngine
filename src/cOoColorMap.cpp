#include "cOoColorMap.h"

cOo::ColorMap::ColorMap( void ) {

    map.push_back( 300.0f );
    map.push_back( 000.0f );
    map.push_back( 025.0f );
    map.push_back( 058.0f );
    map.push_back( 113.0f );
    map.push_back( 195.0f );
    map.push_back( 240.0f );
    map.push_back( -10.0f );
}

float cOo::ColorMap::get( int index ) {

    return( (map[index]/365.0f)*255.0f );
}

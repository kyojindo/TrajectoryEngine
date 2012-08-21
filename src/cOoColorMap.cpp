#include "cOoColorMap.h"

cOo::ColorMap::ColorMap( void ) {

    map.push_back( 292.0f ); map.push_back( 28.0f );
    map.push_back( 61.0f ); map.push_back( 106.0f );
    map.push_back( 193.0f ); map.push_back( 0.0f );
}

float cOo::ColorMap::get( int index ) {

    return( (map[index]/365.0f)*255.0f );
}

#include "cOoColorMap.h"

cOo::ColorMap::ColorMap( void ) {
    
    map.push_back( 315.0f ); // PINK
    map.push_back( 000.0f ); // RED
    map.push_back( 025.0f ); // ORANGE
    map.push_back( 058.0f ); // YELLOW
    map.push_back( 140.0f ); // GREEN
    map.push_back( 195.0f ); // LIGHT_BLUE
    map.push_back( 240.0f ); // NAVY
    map.push_back( -10.0f ); // GRAY
}

float cOo::ColorMap::get( int index ) {

    return( ( map[index]/365.0f ) * 255.0f );
}

#include "cOoDataSet.h"

double &cOo::DataSet::getPitch( void ) {

    return( pitch );
}

double &cOo::DataSet::getVelocity( void ) {

    return( velocity );
}

void cOo::DataSet::set( double dataPitch, double dataVelocity ) {

    velocity = dataVelocity;
    pitch = dataPitch;
}

cOo::DataSet cOo::DataSet::interpolate( DataSet &data, double factor ) {

    DataSet result;
    
    // we interpolate pitch & velocity ( factor=0 -> 100% of this one )
    result.velocity = factor * data.velocity + (1.0f-factor) * velocity;
    result.pitch = factor * data.pitch + (1.0f-factor) * pitch;
    result.time = factor * data.time + (1.0f-factor) * time;
    result.scale = data.getScale();
    
    return( result );
}

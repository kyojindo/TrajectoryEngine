#include "cOoBreakPointFunction.h"

cOo::BreakPointFunction::BreakPointFunction( void ) {
    
    active = false;
    id = 0; type = "";
    head = 0;
}

// [TODO] This function now fills with crap. If the Vuzik
// XML file can be parsed properly, at some point, we should
// have something like load( double *data, ..., long size, ... )
// to replace it, importing raw data from outside as BPFs 

void cOo::BreakPointFunction::load( Time start,
Time stop, long size, long bpfId, string bpfType ) {
    
    record.resize( size ); // set the BPF size
    id = bpfId; type = bpfType; // and properties
    
    // <CRAP>
    
    Time time = start; // set the time axis
    Time step = ( stop-start ) / (Time)size;
    
    double pitch = (double)rand() / (double)RAND_MAX;
    double velo = (double)rand() / (double)RAND_MAX;
    
    for( long k=0; k<record.size(); k++ ) {
        
        pitch += ( 2.0f*( (double)rand() / (double)RAND_MAX ) - 1.0f ) / 100.0f;
        velo += ( 2.0f*( (double)rand() / (double)RAND_MAX ) - 1.0f ) / 100.0f;
        record[k].data.set( pitch, velo ); // filled with some CRAP to test
        
        record[k].time = time;
        time += step; // inc
    }
    
    // </CRAP>
}

bool cOo::BreakPointFunction::startTimeSortPredicate(
const BreakPointFunction *left,  const BreakPointFunction *right ) {
    
    // compare start times betwenn two BPFs
    return( left->record[0].time < right->record[0].time );
}

bool cOo::BreakPointFunction::stopTimeSortPredicate(
const BreakPointFunction *left, const BreakPointFunction *right ) {
    
    // compare stop times betwenn two BPFs
    return( left->record[left->record.size()-1].time
    < right->record[right->record.size()-1].time );
}

long cOo::BreakPointFunction::getNextDataSetFrom(
long fromIndex, Time &time, DataSet &fromQuery ) {
    
    double factor; // interpolation factor
    long foundIndex = IndexNotFound; // init
    
    if( fromIndex > record.size()-1 ) return foundIndex; // check input values
    if( time < record[fromIndex].time || time > getStopTime() ) return foundIndex;
    
    // search from the given index to 1 before last
    for( long k=fromIndex; k<record.size()-1; k++ ) {
    
        // if next time one is over it
        if( record[k+1].time >= time ) {
        
            factor = ( time-record[k].time ) / ( record[k+1].time-record[k].time );
            fromQuery = record[k].data.interpolate( record[k+1].data, factor );
            foundIndex = k; break; // we interpolate the surrounding data
        }
    }
    
    return( foundIndex );
}

bool cOo::BreakPointFunction::getNextDataSet( Time &time, DataSet &fromQuery ) {
    
    long found = getNextDataSetFrom( head, time, fromQuery );
    if( found != IndexNotFound ) { head = found; return( true ); }
    else return( false );
}

bool cOo::BreakPointFunction::getDataSet( Time &time, DataSet &fromQuery ) {
    
    long found = getNextDataSetFrom( 0, time, fromQuery );
    if( found != IndexNotFound ) { head = found; return( true ); }
    else return( false );
}

void cOo::BreakPointFunction::getRecord( long index, Record &query ) {

    query = record[index];
}

cOo::Time &cOo::BreakPointFunction::getStartTime( void ) {

    return( record[0].time );
}

cOo::Time &cOo::BreakPointFunction::getStopTime( void ) {

    return( record[record.size()-1].time );
}

string &cOo::BreakPointFunction::getType( void ) {

    return( type );
}

long cOo::BreakPointFunction::getSize( void ) {

    return( record.size() );
}

long &cOo::BreakPointFunction::getId( void ) {
    
    return( id );
}

void cOo::BreakPointFunction::activate( void ) {

    active = true;
    head = 0;
}

void cOo::BreakPointFunction::deactivate( void ) {

    active = false;
    head = 0;
}

bool cOo::BreakPointFunction::isActive( void ) {

    return( active );
}

void cOo::BreakPointFunction::print( void ) {
    
    cout << isActive() << " | type: " << type.c_str() << " | start: " << getStartTime() << " | stop: "
    << getStopTime() << " | size: " << getSize() << " | head: " << head << " | id: " << id << endl;
}

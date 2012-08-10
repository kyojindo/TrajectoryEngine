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
    
    for( re=record.begin(); re!=record.end(); re++ ) {
        
        pitch += ( 2.0f*( (double)rand() / (double)RAND_MAX ) - 1.0f ) / 10.0f;
        velo += ( 2.0f*( (double)rand() / (double)RAND_MAX ) - 1.0f ) / 10.0f;
        (*re).data.set( pitch, velo ); // filled with some CRAP to test
        
        (*re).data.time = time;
        (*re).time = time;
        time += step;
    }
    
    // This is actually filled backwards to work at this point. It just means that there are
    // two sequences of time stamps. One is inside the DataSet and correspond to the one entered
    // by the user ( containing the anticausal parts ): the other one is basically the sorted
    // list of these time stamps so that it's back in a causal form. When the system reads
    // and check collisions, it uses that one but when we display, we use the user one
    
    // permutate times
    /*for( long k=0; k<256; k++ ) {
        
        long previousIdx = (long)( ( (double)rand() / (double)RAND_MAX ) * (double)record.size() );
        long currentIdx = (long)( ( (double)rand() / (double)RAND_MAX ) * (double)record.size() );
        
        Time previousTime = record[previousIdx].data.time;
        Time currentTime = record[currentIdx].data.time;
        
        record[previousIdx].data.time = currentTime;
        record[currentIdx].data.time = previousTime;
    }*/
    
    // </CRAP>
}

void cOo::BreakPointFunction::addDataSet( DataSet &dataSet ) {

    // 1) we add the new data set at the end of the BPF
    // 2) we save the time sequence stored in record[k].data.time
    // 3) we sort the list by time regarding record[k].time
    // 4) we reapply the saved time sequence on data.time
}

bool cOo::BreakPointFunction::startTimeSortPredicate(
const BreakPointFunction *left,  const BreakPointFunction *right ) {
    
    Time leftTime, rightTime;
    list<Record>::const_iterator it;
    
    it = left->record.begin(); leftTime = (*it).time;
    it = right->record.begin(); rightTime = (*it).time;
    
    return( leftTime < rightTime );
}

bool cOo::BreakPointFunction::stopTimeSortPredicate(
const BreakPointFunction *left, const BreakPointFunction *right ) {
    
    Time leftTime, rightTime;
    list<Record>::const_iterator it;
    
    for( it=left->record.begin(); it!=left->record.end(); it++ ) { leftTime = (*it).time; }
    for( it=right->record.begin(); it!=right->record.end(); it++ ) { rightTime = (*it).time; }
    
    return( leftTime < rightTime );
}

long cOo::BreakPointFunction::getNextDataSetFrom(
long fromIndex, Time &time, DataSet &fromQuery ) {
    
    double factor; // interpolation factor
    long foundIndex = IndexNotFound; // init
    
    // initialize iterators
    re = record.begin(); re++;
    ne = re; re = record.begin();
    
    // then move then until they reach fromIndex
    for( long k=0; k<fromIndex; k++ ) { re++; ne++; }
    
    if( fromIndex > record.size()-1 ) return foundIndex; // check input
    if( time < (*re).time || time > getStopTime() ) return foundIndex;
    
    // search from the given index to 1 before last
    for( long k=fromIndex; k<record.size()-1; k++, re++, ne++ ) {
    
        // if next time one is >=
        if( (*ne).time >= time ) {
        
            factor = ( time - (*re).time ) / ( (*ne).time - (*re).time );
            fromQuery = (*re).data.interpolate( (*ne).data, factor );
            foundIndex = k; break; // we interpolate the data sets
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

void cOo::BreakPointFunction::getRecord( long index, Record &fromQuery ) {

    re = record.begin(); // move iter
    for( long k=0; k<index; k++ ) re++;
    fromQuery = (*re); // copy record
}

cOo::Time cOo::BreakPointFunction::getStartTime( void ) {

    Time time; list<Record>::iterator it;
    
    it = record.begin(); time = (*it).time;
    
    return( time );
}

cOo::Time cOo::BreakPointFunction::getStopTime( void ) {

    Time time; list<Record>::iterator it;
    
    for( it=record.begin(); it!=record.end(); it++ ) { time = (*it).time; }
    
    return( time );
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

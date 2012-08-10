#include "cOoBreakPointFunction.h"

cOo::BreakPointFunction::BreakPointFunction( void ) {
    
    active = false;
    id = 0; type = 0;
    head = 0;
}

void cOo::BreakPointFunction::setProperties( long bpfId, long bpfType  ) {

    id = bpfId; type = bpfType;
}

void cOo::BreakPointFunction::addDataSet( DataSet &dataSet ) {
    
    Record newRec;
    RecordIter re;
    
    list<Time> time;
    list<Time>::iterator t;
    
    newRec.data = dataSet;
    
    record.push_back( newRec );
    time.resize( record.size() );
    
    for( re=record.begin(), t=time.begin(); re!=record.end();
    re++, t++ ) (*t) = (*re).data.time; // save data times
    
    time.sort(); // - the sequence of times is sorted ---
    
    for( re=record.begin(), t=time.begin(); re!=record.end();
    re++, t++ ) (*re).time = (*t); // linear remap BPF time
    
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
    RecordIter re, ne; // iterators
    
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

    RecordIter re;
    
    re = record.begin(); // move iter
    for( long k=0; k<index; k++ ) re++;
    fromQuery = (*re); // copy record
}

cOo::Time cOo::BreakPointFunction::getStartTime( void ) {

    Time time; RecordIter it;
    
    it = record.begin(); time = (*it).time;
    
    return( time );
}

cOo::Time cOo::BreakPointFunction::getStopTime( void ) {

    Time time; RecordIter it;
    
    for( it=record.begin(); it!=record.end(); it++ ) { time = (*it).time; }
    
    return( time );
}

long cOo::BreakPointFunction::getType( void ) {

    return( type );
}

long cOo::BreakPointFunction::getSize( void ) {

    return( record.size() );
}

long cOo::BreakPointFunction::getId( void ) {
    
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
    
    cout << isActive() << " | type: " << getType() << " | start: " << getStartTime() << " | stop: "
    << getStopTime() << " | size: " << getSize() << " | head: " << head << " | id: " << id << endl;
}

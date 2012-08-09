#include "cOoFunctionTimeline.h"

cOo::FunctionTimeline::FunctionTimeline( void ) {
    
    startHead = stopHead = 0;
}

cOo::FunctionTimeline::~FunctionTimeline( void ) {
    
    for( t=startList.begin(); t!=startList.end(); ++t ) {
        
        delete (*t);
    }
}

// [TODO] This function now fills with crap. If the Vuzik XML file
// can be properly parsed, it should be inputed here and interatively
// the various BPFs it contains + properties should be filled in the
// list. Space is created here and start/stop lists are sorted here
// too. As long as start/stop times are provided, it should work

// /!\ I thought that we could use a relative time inside [0.-1.]
// in the time stamps, and keep the absolute time in ms + speed
// ratio in the timeline object for being used at runtime

void cOo::FunctionTimeline::load( long tlSize, long bpfSize, Time maxTime ) {
    
    long k = 1;
    double time1, time2;
    double startTime, stopTime;
    
    startList.resize( tlSize );
    stopList.resize( tlSize );
    scoreMaxTime = maxTime;
    
    for( t=startList.begin(); t!=startList.end(); ++t, k++ ) {
        
        // <CRAP>
        
        time1 = time2 = 0.0f;
        
        while( fabs(time1-time2) < (0.2*scoreMaxTime) ) {
            
            // we generate random start/stop times
            time1 = ( (double)rand() / (double)RAND_MAX ) * scoreMaxTime;
            time2 = ( (double)rand() / (double)RAND_MAX ) * scoreMaxTime;
            
            // but we make sure starts are always smaller than stops
            if( time1 < time2 ) { startTime = time1; stopTime = time2; }
            if( time2 < time1 ) { startTime = time2; stopTime = time1; }
        }
        
        // </CRAP>
        
        (*t) = new BreakPointFunction(); // allocate the memory for that BPF
        (*t)->load( startTime, stopTime, bpfSize, k, "tenor" ); // load with stuff
        
        // -> here we should get real data from the XML file to go in (*t)
    }
    
    // sort startList by startTime order ( using overladed predicate )
    startList.sort( cOo::BreakPointFunction::startTimeSortPredicate );
    
    stopList = startList; // copy and sort stopList by stopTime order
    stopList.sort( cOo::BreakPointFunction::stopTimeSortPredicate );
}

void cOo::FunctionTimeline::activateFrom( long fromIndex, Time &time ) {
    
    // no need to go here if end reached
    if( fromIndex == EndOfList ) return;
    
    t = startList.begin(); // move iterator
    for( long k=0; k<fromIndex; k++ ) t++;
    
    for( long k=fromIndex; t!=startList.end(); t++, k++ ) {
        
        if( (*t)->getStartTime() <= time ) {
            
            if( !(*t)->isActive() ) {
                
                touchedList.push_back( *t );
                (*t)->activate();
            }
            
            if( k == startList.size()-1 ) {
                
                startHead = EndOfList;
            }
            
        } else {
        
            startHead = k;
            break;
        }
    }
}

void cOo::FunctionTimeline::deactivateFrom( long fromIndex, Time &time ) {
    
    if( fromIndex == EndOfList ) return;
    
    t = stopList.begin(); // move iterator
    for( long k=0; k<fromIndex; k++ ) t++;
    
    for( long k=fromIndex; t!=stopList.end(); t++, k++ ) {
        
        if( (*t)->getStopTime() <= time ) {
            
            if( (*t)->isActive() ) {
                
                touchedList.remove( *t );
                (*t)->deactivate();
            }
            
            if( k == stopList.size()-1 ) {
                
                stopHead = EndOfList;
            }
         
        } else {
            
            stopHead = k;
            break;
        }
    }
}

void cOo::FunctionTimeline::deactivateAll( void ) {
    
    for( t=startList.begin(); t!=startList.end(); ++t ) (*t)->deactivate();
    startHead = stopHead = 0; // reset activations and head positions
}

void cOo::FunctionTimeline::getNextTouched( vector<Record> &touched, Time &time ) {
    
    Record record;
    
    touched.clear();
    
    activateFrom( startHead, time );
    deactivateFrom( stopHead, time );
    
    for( t=touchedList.begin(); t!=touchedList.end(); ++t ) {
        
        (*t)->getNextDataSet( time, record.data );
        record.type = (*t)->getType(); record.id = (*t)->getId();
        record.time = time; touched.push_back( record );
    }
}

void cOo::FunctionTimeline::getTouched( vector<Record> &touched, Time &time ) {
    
    Record record;
    
    touchedList.clear();
    touched.clear();
    
    deactivateAll();
    activateFrom( 0, time );
    deactivateFrom( 0, time );
    
    for( t=touchedList.begin(); t!=touchedList.end(); ++t ) {
    
        (*t)->getDataSet( time, record.data );
        record.type = (*t)->getType(); record.id = (*t)->getId();
        record.time = time; touched.push_back( record );
    }
}

list<cOo::BreakPointFunction *>::iterator cOo::FunctionTimeline::getBegin( void ) {

    return startList.begin();
}

list<cOo::BreakPointFunction *>::iterator cOo::FunctionTimeline::getEnd( void ) {
    
    return startList.end();
}

cOo::Time cOo::FunctionTimeline::getMaxTime( void ) {

    return( scoreMaxTime );
}

long cOo::FunctionTimeline::getSize( void ) {

    return( startList.size() );
}

void cOo::FunctionTimeline::print( void ) {
    
    cout << "--- start-sorted list ---" << endl << endl;
    for( t=startList.begin(); t!=startList.end(); ++t ) (*t)->print();
    
    cout << endl << "--- stop-sorted list ---" << endl << endl;
    for( t=stopList.begin(); t!=stopList.end(); ++t ) (*t)->print();
}

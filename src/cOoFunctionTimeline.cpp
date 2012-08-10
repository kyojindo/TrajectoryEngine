#include "cOoFunctionTimeline.h"

cOo::FunctionTimeline::FunctionTimeline( void ) {
    
    startHead = stopHead = 0;
}

cOo::FunctionTimeline::~FunctionTimeline( void ) {
    
    for( t=startList.begin(); t!=startList.end(); ++t ) {
        
        delete (*t);
    }
}

void cOo::FunctionTimeline::load( long tlSize, long bpfSize, Time maxTime ) {
    
    long id = 1;
    double startTime;
    DataSet dataSet;
    
    startList.resize( tlSize );
    stopList.resize( tlSize );
    scoreMaxTime = maxTime;
    
    for( t=startList.begin(); t!=startList.end(); t++, id++ ) {
        
        (*t) = new BreakPointFunction();
        
        // <CRAP>
        
        // Here we fill the BPF using the addSetSet() ( = good ) but the sets
        // are made out of incremental random data and times. At this point, the
        // Vuzik XML file should be parsed, a arbitrary time base defined, and
        // the data sets added as to respect the ordering of the painting
        // which is probably the ordering in the XML file anyway
        
        // set the BPF properties
        (*t)->setProperties( id, 0 );
        
        // start from a random point in data space
        dataSet.pitch = 0.5f*( (double)rand() / (double)RAND_MAX )+0.25f;
        dataSet.velocity = (double)rand() / (double)RAND_MAX;
        
        // and start from a random time in virtual time ( earliest = 0.5 from start )
        dataSet.time = ( (double)rand() / (double)RAND_MAX ) * ( scoreMaxTime-1.0f ) + 0.5f;
        
        for( long k=0; k<bpfSize; k++ ) {
        
            (*t)->addDataSet( dataSet );
            
            dataSet.pitch += 0.04f*(2.0f*((double)rand() / (double)RAND_MAX)-1.0f);
            dataSet.velocity += 0.04f*(2.0f*((double)rand() / (double)RAND_MAX)-1.0f);
            
            dataSet.time += 0.4f*(2.0f*((double)rand() / (double)RAND_MAX)-1.0f);
            if( dataSet.time < 0.5f ) dataSet.time = 0.5f;
        }
        
        // <CRAP>
    }
    
    // sort startList by startTime order ( using overladed predicate )
    startList.sort( cOo::BreakPointFunction::startTimeSortPredicate );
    
    stopList = startList; // copy and sort stopList by stopTime order
    stopList.sort( cOo::BreakPointFunction::stopTimeSortPredicate );
    
    scoreMaxTime += 2.0f; // trick to add some room at the end
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
    
    bool found;
    Record record;
    vector<Record> tmp;
    
    // activation routines
    activateFrom( startHead, time );
    deactivateFrom( stopHead, time );
    
    cleanEndTouched( touched );
    
    // then we force all the records to take the 'atEnd' state
    for( long k=0; k<touched.size(); k++ ) touched[k].state = atEnd;
    
    for( t=touchedList.begin(); t!=touchedList.end(); ++t ) {
        
        found = false;
        
        (*t)->getNextDataSet( time, record.data );
        record.type = (*t)->getType(); record.id = (*t)->getId();
        record.time = time; // create record from touched list
        
        // we check in the cleaned touch list
        for( long k=0; k<touched.size(); k++ ) {
            
            // if the BPF is already there
            if( touched[k].id == record.id ) {
                
                // we switch it back
                touched[k].data = record.data;
                touched[k].time = record.time;
                touched[k].state = atAny;
                found = true;
            }
        }
        
        // new BPF
        if( !found ) {
            
            record.state = atBegin;
            touched.push_back( record );
        }
    }
}

void cOo::FunctionTimeline::getTouched( vector<Record> &touched, Time &time ) {
    
    bool found;
    Record record;
    //vector<Record> tmp;
    
    deactivateAll();
    touchedList.clear();
    
    activateFrom( 0, time );
    deactivateFrom( 0, time );
    
    cleanEndTouched( touched );
    
    // then we force all the records to take the 'atEnd' state
    for( long k=0; k<touched.size(); k++ ) touched[k].state = atEnd;
    
    for( t=touchedList.begin(); t!=touchedList.end(); ++t ) {
    
        found = false;
        
        (*t)->getDataSet( time, record.data );
        record.type = (*t)->getType(); record.id = (*t)->getId();
        record.time = time; // create record from touched list
        
        // we check in the cleaned touch list
        for( long k=0; k<touched.size(); k++ ) {
            
            // if the BPF is already there
            if( touched[k].id == record.id ) {
                
                // update and switch back
                touched[k].data = record.data;
                touched[k].time = record.time;
                touched[k].state = atAny;
                found = true;
            }
        }
        
        // new BPF
        if( !found ) {
            
            record.state = atBegin;
            touched.push_back( record );
        }
    }
}

void cOo::FunctionTimeline::cleanEndTouched( vector<Record> &touched ) {

    vector<Record> tmp;
    
    for( long k=0; k<touched.size(); k++ ) if( touched[k].state != atEnd ) tmp.push_back( touched[k] );
    touched.clear(); touched = tmp; // this trick is just my way to remove all 'atEnd' records
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

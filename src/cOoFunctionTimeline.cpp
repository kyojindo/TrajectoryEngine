#include "cOoFunctionTimeline.h"

cOo::FunctionTimeline::FunctionTimeline( void ) {
    
    startHead = stopHead = 0;

}

cOo::FunctionTimeline::~FunctionTimeline( void ) {
    
    clear();
}

// --- fill the timeline with random curves ----
void cOo::FunctionTimeline::generate( Time maxTime ) {
    
    long id = 0;
    int genType;
    
    Time currentTime;
    Time deltaTime;
    
    DataSet dataSet;
    long bpfSize;
    
    float minBound, vMinBound;
    float maxBound, vMaxBound;
    float angle, radius;
    float genDistrib;
    
    scoreMaxTime = maxTime;
    
    for( int v=0; v<nOfVoices; v++ ) {
    
        currentTime = ofRandom( 2.0f, 5.0f );
        
        while( currentTime < scoreMaxTime ) {
            
            genDistrib = ofRandom( 0.0f, 100.0f );
            
            if( genDistrib > 0.0f && genDistrib <= 20.0f ) genType = Curves;
            if( genDistrib > 20.0f && genDistrib <= 70.0f ) genType = Notes;
            if( genDistrib > 70.0f && genDistrib <= 95.0f ) genType = Silence;
            if( genDistrib > 95.0f && genDistrib <= 100.0f ) genType = Noise;
            
            deltaTime = ofRandom( 4.0f, 25.0f );
            
            switch( genType ) {
                    
                case Notes:
                    
                    id++; startList.resize( id ); stopList.resize( id );
                    t=startList.begin(); for( long k=0; k<(id-1); k++ ) t++;
                    bpfSize = (long)deltaTime; dataSet.time = currentTime;
                    
                    minBound = ofRandom( 3.0f, 16.0f ); maxBound = ofRandom( 17.0f, 30.0f );
                    vMinBound = minBound + (float)v * ( (maxBound-minBound)/((float)(nOfVoices+1)) );
                    vMaxBound = minBound + (float)(v+2) * ( (maxBound-minBound)/((float)(nOfVoices+1)) );
                    
                    dataSet.pitch = round( ofRandom( vMinBound, vMaxBound ) );
                    dataSet.velocity = ofRandom( 0.2f, 1.0f );
                    
                    (*t) = new BreakPointFunction();
                    (*t)->setProperties( id, v, false );
                    
                    for( long k=0; k<bpfSize; k++ ) {
                        
                        if( dataSet.time > ( maxTime - 2.0f ) || dataSet.pitch < 1.0f ||
                        dataSet.pitch > 32.0f ) break; else (*t)->addDataSet( dataSet );
                        
                        dataSet.time += 1.0f;
                    }
                    
                    break;
                    
                case Curves:
                    
                    id++; startList.resize( id ); stopList.resize( id );
                    t=startList.begin(); for( long k=0; k<(id-1); k++ ) t++;
                    bpfSize = (long)deltaTime; dataSet.time = currentTime;
                    
                    minBound = ofRandom( 7.0f, 16.0f ); maxBound = ofRandom( 17.0f, 26.0f );
                    vMinBound = minBound + (float)v * ( (maxBound-minBound)/((float)(nOfVoices+1)) );
                    vMaxBound = minBound + (float)(v+2) * ( (maxBound-minBound)/((float)(nOfVoices+1)) );
                    
                    (*t) = new BreakPointFunction();
                    (*t)->setProperties( id, v, false );
                    
                    dataSet.pitch = round( ofRandom( vMinBound, vMaxBound ) );
                    dataSet.velocity = ofRandom( 0.2f, 1.0f );
                    
                    for( long k=0; k<bpfSize; k++ ) {
                    
                        angle = ofRandom( -0.75f*PI, 0.75f*PI );
                        radius = ofRandom( 1.0f, 3.0f );
                        
                        dataSet.time += radius*cos( angle );
                        dataSet.pitch += radius*sin( angle );
                        
                        if( dataSet.time < 0 || dataSet.time > maxTime ||
                        dataSet.pitch < 0.0f || dataSet.pitch > 33.0f ) break;
                        else (*t)->addDataSet( dataSet ); // fill or leave
                    }
                    
                    break;
                    
                case Noise:
                    
                    id++; startList.resize( id ); stopList.resize( id );
                    t=startList.begin(); for( long k=0; k<(id-1); k++ ) t++;
                    bpfSize = (long)deltaTime; dataSet.time = currentTime;
                    
                    minBound = ofRandom( 2.0f, 16.0f ); maxBound = ofRandom( 17.0f, 29.0f );
                    vMinBound = minBound + (float)v * ( (maxBound-minBound)/((float)(nOfVoices+1)) );
                    vMaxBound = minBound + (float)(v+2) * ( (maxBound-minBound)/((float)(nOfVoices+1)) );
                    
                    (*t) = new BreakPointFunction();
                    (*t)->setProperties( id, v, true );
                    
                    dataSet.pitch = round( ofRandom( vMinBound, vMaxBound ) );
                    dataSet.velocity = ofRandom( 0.2f, 1.0f );
                    
                    for( long k=0; k<bpfSize; k++ ) {
                        
                        angle = ofRandom( -0.75f*PI, 0.75f*PI );
                        radius = ofRandom( 1.0f, 3.0f );
                        
                        dataSet.time += radius*cos( angle );
                        dataSet.pitch += radius*sin( angle );
                        
                        if( dataSet.time < 0 || dataSet.time > maxTime ||
                           dataSet.pitch < 0.0f || dataSet.pitch > 33.0f ) break;
                        else (*t)->addDataSet( dataSet ); // fill or leave
                    }
                    
                    break;
            }
            
            currentTime += deltaTime;
        }
    }
    
    for( t=startList.begin(); t!=startList.end(); t++ ) {
        
        // if the BPF is too small, we remove it from the timeline ---
        if( (*t)->getSize() < 3 ) { delete (*t); startList.erase( t ); }
    }
    
    // sort startList by startTime order ( using overladed predicate )
    startList.sort( cOo::BreakPointFunction::startTimeSortPredicate );
    
    stopList = startList; // copy and sort stopList by stopTime order
    stopList.sort( cOo::BreakPointFunction::stopTimeSortPredicate );
    
    scoreMaxTime += 30.0f; // trick to add some room at the end
}

void cOo::FunctionTimeline::clear( void ) {

    for( t=startList.begin(); t!=startList.end(); ++t ) delete (*t);
    startList.clear(); stopList.clear(); startHead = stopHead = 0;
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
        record.craziness = (*t)->isCrazy(); record.time = time;
        
        // we check in the cleaned touch list
        for( long k=0; k<touched.size(); k++ ) {
            
            // if the BPF is already there
            if( touched[k].id == record.id ) {
                
                // we switch it back
                touched[k].data = record.data;
                touched[k].time = record.time;
                touched[k].type = record.type;
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
        record.craziness = (*t)->isCrazy(); record.time = time;
        
        // we check in the cleaned touch list
        for( long k=0; k<touched.size(); k++ ) {
            
            // if the BPF is already there
            if( touched[k].id == record.id ) {
                
                // update and switch back
                touched[k].data = record.data;
                touched[k].time = record.time;
                touched[k].type = record.type;
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

double cOo::FunctionTimeline::tempPitchConverter(double in_p) {
    
    return VUZIK_PITCH_MIN + (VUZIK_Y_MAX-in_p)*(VUZIK_PITCH_MAX-VUZIK_PITCH_MIN)/(VUZIK_Y_MAX-VUZIK_Y_MIN);
}

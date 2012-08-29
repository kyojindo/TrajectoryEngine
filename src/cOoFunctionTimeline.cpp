#include "cOoFunctionTimeline.h"

cOo::FunctionTimeline::FunctionTimeline( void ) {
    
    startHead = stopHead = 0;

}

cOo::FunctionTimeline::~FunctionTimeline( void ) {
    
    for( t=startList.begin(); t!=startList.end(); ++t ) {
        
        delete (*t);
    }
}

void cOo::FunctionTimeline::loadMidiImport() {
    ofBuffer file = ofBufferFromFile("midi_import.txt");
    string line;
    
    MIDIText voices[NUM_MIDI_VOICES];
    
        
    long numNotesRead = 0;
    long numDiscarded = 0;
    
    int max_pitch = INT_MIN;
    int min_pitch = INT_MAX;
    
    while (!file.isLastLine()) {

        line = file.getNextLine();
        
        int ch;
        double time;
        int note;
        int vel;
        
        int numArgs = sscanf(line.c_str(), "%i %lf %i %i", &ch, &time, &note, &vel);
        //printf("loaded: %s \n", line.c_str());
        //printf("parsed: %i %lf %i %i\n",ch, time, note, vel);
    
        if ( (numArgs == 4) && (ch>0) && (note>0) ) {
            numNotesRead++;
            
            //if previous point on same track has same time stamp, disregard it (double).
            if (voices[ch-1].time.size() && (time == voices[ch-1].time.back()) ) {
                voices[ch-1].time.pop_back();
                voices[ch-1].pitch.pop_back();
                voices[ch-1].velocity.pop_back();
                numDiscarded++;
            }
            
            // add to list
            voices[ch-1].time.push_back(time);
            voices[ch-1].pitch.push_back(note);
            voices[ch-1].velocity.push_back(vel);
            
            
            if (note < min_pitch)
                min_pitch = note;
            
            if (note > max_pitch)
                max_pitch = note;
        }
        
    }
    printf("numRead = %li numDiscarded= %li\n", numNotesRead, numDiscarded);
    printf("minpitch = %i max = %i\n", min_pitch, max_pitch);
    
    long id = 0;
    long bpfSize;
    DataSet dataSet;
    startList.resize(4);
    stopList.resize( 4 );
    scoreMaxTime = 30.0;
    
    for( t=startList.begin(); t!=startList.end(); t++, id++ ) {
        //printf("adding line %li\n",id);
        
        (*t) = new BreakPointFunction();
    
            
        int lineType =id;
        
        // set the BPF properties
        (*t)->setProperties( id, lineType, false );
        
        
        bpfSize = voices[id].time.size();
        
        dataSet.pitch = voices[id].pitch.front() - 50;
        
        dataSet.velocity = voices[id].velocity.front();
        
        dataSet.time = voices[id].time.front();
        
        for( long k=0; k<bpfSize; k++ ) {
            
            (*t)->addDataSet( dataSet );
            
            dataSet.pitch = voices[id].pitch[k] - 50;
            dataSet.velocity = voices[id].velocity[k];
            dataSet.time = voices[id].time[k];
            printf("%lf\n",dataSet.time);
            
        }
    }
    
    
    // sort startList by startTime order ( using overladed predicate )
    startList.sort( cOo::BreakPointFunction::startTimeSortPredicate );
    
    stopList = startList; // copy and sort stopList by stopTime order
    stopList.sort( cOo::BreakPointFunction::stopTimeSortPredicate );
    
    scoreMaxTime += 2.0f; // trick to add some room at the end
    
}

void cOo::FunctionTimeline::load( long tlSize, long bpfSize, Time maxTime ) {
    
    long id = 1;
    double startTime;
    DataSet dataSet;
    ofxXmlSettings xmlFile;
    VuzikXML* vuzikLines;
    
    
    //test load data
    if (xmlFile.loadFile("data.xml")){
		printf("data.xml loaded!\n");
	}else{
		printf("unable to load data.xml check data/ folder");
	}
    bool push = xmlFile.pushTag("Graphics");
    if (push) printf("Graphics OK\n");
    push = xmlFile.pushTag("Graphics");
    if (push) printf("OK\n");
    int numLines = xmlFile.getNumTags("PropertiesGraphicsPolyLine");
    printf("num polyLines read in data = %i\n", numLines);
    
    if (numLines > 0) {
        //only parse the file if there is at least one line tag
        
        vuzikLines = new VuzikXML[numLines];
                
        double x_min = DBL_MAX;
        double x_max = DBL_MIN;
        double y_min = DBL_MAX;
        double y_max = DBL_MIN;
        
        for(long l=0; l<numLines; l++) {

            push = xmlFile.pushTag("PropertiesGraphicsPolyLine", l);
 //           if (push) printf("loading polyLine %li...  ", l);
            
            double line_w = xmlFile.getValue("LineWidth", -1.0);
 //           printf("line_w = %f\n",line_w);
            
            //load line color
            
            push = xmlFile.pushTag("ObjectColor");
            
            double valA = xmlFile.getValue("A", -1.0);
            
            double valR = xmlFile.getValue("R", -1.0);
            
            double valG = xmlFile.getValue("G", -1.0);
            
            double valB = xmlFile.getValue("B", -1.0);
            
            xmlFile.popTag(); //ObjectColor
            
            push = xmlFile.pushTag("Points");
//            if (push) printf("Points OK\n");
            
            int numPts = xmlFile.getNumTags("Point");
//            printf("num points read in line = %i\n", numPts);
            
            //save line data into structure
            vuzikLines[l].init(numPts, line_w, valA, valR, valG, valB);
            
            if (numPts>0) {
                for (int i=0; i<numPts; i++) {
                    double x = xmlFile.getValue("Point:X", 0.0, i);
                    double y = xmlFile.getValue("Point:Y", 0.0, i);
                    if (x>x_max) x_max = x;
                    if (x<x_min) x_min = x;
                    if (y>y_max) y_max = y;
                    if (y<y_min) y_min = y;
                    
                    vuzikLines[l].setX(x, i);
                    vuzikLines[l].setY(y, i);
                    //printf("x:y = %f:%f\n", x,y);
                }
            }
            xmlFile.popTag(); // Points
            
            
            xmlFile.popTag(); //PolyLine
        }
        printf("x_min x_max: %f %f\n", x_min, x_max);
        printf("y_min y_max: %f %f\n", y_min, y_max);
        
        pitch_in_min = 16;
        pitch_in_max = 1048;
        pitch_out_min = 0.0;
        pitch_out_max = 33.0;
        
        x_in_min = x_min;
        x_in_max = x_max;
        maxTime = x_max/100.0;
    }
    
    startList.resize( numLines );
    stopList.resize( numLines );
    scoreMaxTime = maxTime;
    
    for( t=startList.begin(); t!=startList.end(); t++, id++ ) {
        //printf("adding line %li\n",id);
        
        (*t) = new BreakPointFunction();
        
        // <CRAP>
        
        // Here we fill the BPF using the addSetSet() ( = good ) but the sets
        // are made out of incremental random data and times. At this point, the
        // Vuzik XML file should be parsed, a arbitrary time base defined, and
        // the data sets added as to respect the ordering of the painting
        // which is probably the ordering in the XML file anyway

        (*t)->a = vuzikLines[id-1].alpha;
        (*t)->r = vuzikLines[id-1].red;
        (*t)->g = vuzikLines[id-1].green;
        (*t)->b = vuzikLines[id-1].blue;
        
        int lineType =VuzikXML::parseVuzikLineType(vuzikLines[id-1].red);
        
        // set the BPF properties
        (*t)->setProperties( id, lineType, false );

        
        bpfSize = vuzikLines[id-1].getSize();
        
        dataSet.pitch = tempPitchConverter(vuzikLines[id-1].getY(0));
        
        dataSet.velocity = (double)rand() / (double)RAND_MAX;
        
        
        dataSet.time = 0.5+(vuzikLines[id-1].getX(0)-x_in_min)*maxTime/(x_in_max-x_in_min);
        
        for( long k=0; k<bpfSize; k++ ) {
        
            (*t)->addDataSet( dataSet );
            
            dataSet.pitch = tempPitchConverter(vuzikLines[id-1].getY(k));
            dataSet.velocity += 0.04f*(2.0f*((double)rand() / (double)RAND_MAX)-1.0f);
            
            dataSet.time = 0.5+(vuzikLines[id-1].getX(k)-x_in_min)*maxTime/(x_in_max-x_in_min);
            
        }
    }
    
    
    // sort startList by startTime order ( using overladed predicate )
    startList.sort( cOo::BreakPointFunction::startTimeSortPredicate );
    
    stopList = startList; // copy and sort stopList by stopTime order
    stopList.sort( cOo::BreakPointFunction::stopTimeSortPredicate );
    
    scoreMaxTime += 2.0f; // trick to add some room at the end
    
    //cleanup
    
    delete vuzikLines;
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

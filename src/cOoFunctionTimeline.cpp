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
    ofBuffer file = ofBufferFromFile("midi_import2.txt");
    string line;
    
    MIDITextTrack voices[NUM_MIDI_VOICES];
    long numMidiSigs = 0;
    long numDiscarded = 0;
    long numNotes = 0;
    long numJoins = 0; //when a note off happens at the same time as a new note: join
                        // these two notes together to form one bpf
    long numBpfs = 0;
    
    int max_pitch = INT_MIN;
    int min_pitch = INT_MAX;
    
    double min_time = DBL_MAX;
    double max_time = DBL_MIN;

    
    //STEP 1: parse. fill vector with raw midi track data, seperate by channel
    while (!file.isLastLine()) {

        line = file.getNextLine();
        
        int ch;
        double time;
        int note;
        int vel;
        
        int numArgs = sscanf(line.c_str(), "%i %i %i %lf", &note, &vel, &ch, &time);
        

        //printf("loaded: %s \n", line.c_str());
        //printf("parsed: %i %lf %i %i\n",ch, time, note, vel);
        
        //every note is accompanied by a note off, so we can count notes this way
        if (vel == 0)
            numNotes++;
        
        if ( (numArgs == 4) && (ch>0) && (note>0) ) {
            //we parsed a valid midi note value
            numMidiSigs++;
            
            // add current point to list
            voices[ch-1].time.push_back(time);
            voices[ch-1].pitch.push_back((double)note);
            voices[ch-1].velocity.push_back((double)vel/127.0);
            
            if (note < min_pitch) min_pitch = note;
            if (note > max_pitch) max_pitch = note;
            
            if (time < min_time) min_time = time;
            if (time > max_time) max_time = time;
        }
        
    }
    printf("min = %i max = %i\n", min_pitch, max_pitch);
    printf("t = %lf to %lf sec\n", min_time, max_time);
    
    //STEP 2: convert. go through array of MIDI tracks (by channel) and turn them into BPF's
    vector <BreakPointFunction> parsedBPFs;
    (*t) = new BreakPointFunction();
    long bpf_idx = 0;
    
    for (int i=0; i<NUM_MIDI_VOICES; i++) {
        //i == MIDI channel == line type
        for (int j=0; j<voices[i].time.size(); j++) {
            
            DataSet rec;
            //if current record is a keyoff (vel==0):
            // 1.) duplicate previous point in track to extend the drawing horizontally to the end of the note
            // 2.) add that to the line before inserting the keyoff point.
            // 3.) add keyoff
            if (voices[i].velocity[j] == 0) {
                rec.time = voices[i].time[j];
                rec.pitch = voices[i].pitch[j];
                rec.velocity = voices[i].velocity[j];
                (*t)->addDataSet(rec);
            }
            //add current point
            rec.time = voices[i].time[j];
            rec.pitch = voices[i].pitch[j];
            rec.velocity = voices[i].velocity[j];
            (*t)->addDataSet(rec);
            
            
            if (0) { //end of BPF found; add current to list and start new one
                (*t) = new BreakPointFunction();
                
            }
        }
    }
    
    scoreMaxTime = max_time;
       
    
    // sort startList by startTime order ( using overladed predicate )
    startList.sort( cOo::BreakPointFunction::startTimeSortPredicate );
    
    stopList = startList; // copy and sort stopList by stopTime order
    stopList.sort( cOo::BreakPointFunction::stopTimeSortPredicate );
    
    scoreMaxTime += 2.0f; // trick to add some room at the end
    
}

void cOo::FunctionTimeline::loadVuzikFile(string filename) {
    
    Time maxTime = 0.0;
    long bpfSize;
    long id = 1;
    double startTime;
    DataSet dataSet;
    ofxXmlSettings xmlFile;
    vector <VuzikXML> vuzikLines;
    
    if (xmlFile.loadFile(filename)){
		printf("%s loaded!\n", filename.c_str());
	}else{
		printf("unable to load %s check data/ folder\n", filename.c_str());
	}
    int numCombinedScores = xmlFile.getNumTags("Graphics");
    printf("num scores = %i\n", numCombinedScores);
    
    long totalLines = 0;
    double x_offset = 0.0;
    
    x_in_min = 0;
    x_in_max = DBL_MIN;
    
    for (int s=0; s<numCombinedScores; s++) {
        
        double x_min = 0;
        double x_max = DBL_MIN;
        double y_min = DBL_MAX;
        double y_max = DBL_MIN;
    
        bool push = xmlFile.pushTag("Graphics",s);
    
        if (push) printf("score # %i OK\n", s);
        push = xmlFile.pushTag("Graphics");
        //if (push) printf("OK\n");
        int numLines = xmlFile.getNumTags("PropertiesGraphicsPolyLine");
        printf("num polyLines read in data = %i\n", numLines);
        
        if (numLines > 0) {
            //only parse the file if there is at least one line tag
            
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
                
                int numPts = xmlFile.getNumTags("Point");
                
                //save line data into structure
                VuzikXML line;
                line.init(numPts, line_w, valA, valR, valG, valB);
                
                if (numPts>0) {
                    for (int i=0; i<numPts; i++) {
                        double x = xmlFile.getValue("Point:X", 0.0, i);
                        double y = xmlFile.getValue("Point:Y", 0.0, i);
                        if (x>x_max) x_max = x;
                        //if (x<x_min) x_min = x;
                        if (y>y_max) y_max = y;
                        if (y<y_min) y_min = y;
                        
                        line.setX(x+x_offset, i);
                        line.setY(y, i);
                        //printf("x:y = %f:%f\n", x,y);
                    }
                }
                xmlFile.popTag(); // Points
                
                xmlFile.popTag(); //PolyLine

                //add line to list
                vuzikLines.push_back(line);
            }
            xmlFile.popTag(); //graphics
            xmlFile.popTag(); //graphics
        
            printf("x_min x_max: %f %f\n", x_min, x_max);
            printf("y_min y_max: %f %f\n", y_min, y_max);
            totalLines+= numLines;
            
            x_offset+= x_max; //increment begin time
            printf("x_offset pushed to %lf\n",x_offset);
            
        }
        //!!!! NOTE: this 100.0 is a tuning factor
        
        x_in_max+=x_max; //increment total time (combined offset)
        maxTime+=x_max/100.0;
    }
    printf("total lines read = %li\n", totalLines);
    
    startList.resize( totalLines );
    stopList.resize( totalLines );
    scoreMaxTime = maxTime;
    
    printf("x_max = %lf\n",x_in_max);
    printf("x_min = %lf\n",x_in_min);
    
    for( t=startList.begin(); t!=startList.end(); t++, id++ ) {
        //printf("adding line %li\n",id);
        
        (*t) = new BreakPointFunction();
        
        (*t)->a = vuzikLines[id-1].alpha;
        (*t)->r = vuzikLines[id-1].red;
        (*t)->g = vuzikLines[id-1].green;
        (*t)->b = vuzikLines[id-1].blue;
        
        int lineType =VuzikXML::parseVuzikLineType(vuzikLines[id-1].red);
        
        // set the BPF properties
        (*t)->setProperties( id, lineType, false );

        
        bpfSize = vuzikLines[id-1].getSize();
        
        dataSet.pitch = tempPitchConverter(vuzikLines[id-1].getY(0));
        
        dataSet.velocity = vuzikLines[id-1].getLineWidth()/10.0;
        
        
        dataSet.time = 0.5+(vuzikLines[id-1].getX(0))*maxTime/(x_in_max);
        
        for( long k=0; k<bpfSize; k++ ) {
        
            (*t)->addDataSet( dataSet );
            
            dataSet.pitch = tempPitchConverter(vuzikLines[id-1].getY(k));
            dataSet.velocity = vuzikLines[id-1].getLineWidth()/10.0;
            dataSet.time = 0.5+(vuzikLines[id-1].getX(k))*maxTime/(x_in_max);
        }
    }
    
    
    // sort startList by startTime order ( using overladed predicate )
    startList.sort( cOo::BreakPointFunction::startTimeSortPredicate );
    
    stopList = startList; // copy and sort stopList by stopTime order
    stopList.sort( cOo::BreakPointFunction::stopTimeSortPredicate );
    
    scoreMaxTime += 2.0f; // add some room at the end
    
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

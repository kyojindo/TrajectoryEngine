#ifndef __cOoFunctionTimeline__
#define __cOoFunctionTimeline__

#include <list>
#include <vector>
#include <math.h>

#include "cOoBreakPointFunction.h"
#include "ofxXmlSettings.h"
#include "cOoVuzikXML.h"
#include "cOoMIDITextTrack.h"

#include "ofMain.h"

using namespace std;

namespace cOo {
    
    enum {
        
        Silence,
        Notes,
        Curves,
        Noise
    };
    
    const long EndOfList = -1;
    
    const int nOfVoices = 6;
    
    class FunctionTimeline {
    
      public:
        
        FunctionTimeline( void );
        ~FunctionTimeline( void );
        
        void generate( Time maxTime );
        void loadVuzikFile( string filename );
        void loadMidiFile( string filename );
        void clear( void );
        
        void activateFrom( long fromIndex, Time &time );
        void deactivateFrom( long fromIndex, Time &time );
        void deactivateAll( void ); // activation routines
        
        void getNextTouched( vector<Record> &touched, Time &time );
        void getTouched( vector<Record> &touched, Time &time );
        void cleanEndTouched( vector<Record> &touched );
        
        list<BreakPointFunction *>::iterator getBegin( void );
        list<BreakPointFunction *>::iterator getEnd( void );
        
        Time getMaxTime( void );
        long getSize( void );
        void print( void );
        
      protected:
        
        double vuzikPitchMapping( double in_pitch );
        
        list<BreakPointFunction *>::iterator t; // iterator to browse the lists
        list<BreakPointFunction *> startList; // one list sorts BPFs by start time
        list<BreakPointFunction *> stopList; // the other one by stop time
        list<BreakPointFunction *> touchedList; // list of touched BPFs
        long startHead, stopHead; // saved playback heads
        Time scoreMaxTime; // maximum score duration
        
        //[TODO: add protocol for inputting time settings: how
        // much gap to put before/end, set piece duration, etc]
        double x_in_min; double x_in_max;
    };
}

#endif

#ifndef __cOoFunctionTimeline__
#define __cOoFunctionTimeline__

#include <list>
#include <vector>

#include "cOoBreakPointFunction.h"
#include "ofxXmlSettings.h"

using namespace std;

namespace cOo {
    
    const long EndOfList = -1;
    
    class FunctionTimeline {
    
      public:
        
        FunctionTimeline( void );
        ~FunctionTimeline( void );
        
        void load( long tlSize, long bpfSize, Time maxTime );
        
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
        
        list<BreakPointFunction *>::iterator t; // iterator to browse the lists
        list<BreakPointFunction *> startList; // one list sorts BPFs by start time
        list<BreakPointFunction *> stopList; // the other one by stop time
        list<BreakPointFunction *> touchedList; // list of touched BPFs
        long startHead, stopHead; // saved playback heads
        Time scoreMaxTime; // maximum score duration
    };
}

#endif

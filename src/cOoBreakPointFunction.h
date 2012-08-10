#ifndef __cOoBreakPointFunction__
#define __cOoBreakPointFunction__

#include <cassert>
#include <iostream>
#include <cstring>
#include <list>

#include "cOoRecord.h"

using namespace std;

namespace cOo {
    
    const long IndexNotFound = -1;
    
    class BreakPointFunction {
    
      public:
        
        BreakPointFunction( void );
        
        // load function: put stuff in the BPF, it's full of crap right now
        void load( Time start, Time stop, long size, long bpfId, string bpfType );
        
        // incremental add of new data set
        void addDataSet( DataSet &dataSet );
        
        // static functions defined to help any BPF to be sorted both by ascending start and stop times
        static bool startTimeSortPredicate( const BreakPointFunction *left, const BreakPointFunction *right );
        static bool stopTimeSortPredicate( const BreakPointFunction *left, const BreakPointFunction *right );
        
        // generic function to find next previous closest index from given one
        long getNextDataSetFrom( long fromIndex, Time &time, DataSet &fromQuery );
        
        // functions used as public access to sets
        bool getNextDataSet( Time &time, DataSet &fromQuery ); // incremental search
        bool getDataSet( Time &time, DataSet &fromQuery ); // overall search
        
        // this one is just to display the records
        void getRecord( long index, Record &fromQuery );
        
        Time getStartTime( void );
        Time getStopTime( void );
        
        void activate( void );
        void deactivate( void );
        bool isActive( void );
        
        string &getType( void );
        long getSize( void );
        long &getId( void );
        
        void print( void );
        
      protected:
        
        list<Record> record;
        list<Record>::iterator re;
        list<Record>::iterator ne;
        
        bool active; long head;
        string type; long id;
    };
}

#endif

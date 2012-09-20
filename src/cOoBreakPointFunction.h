#ifndef __cOoBreakPointFunction__
#define __cOoBreakPointFunction__

#include <cassert>
#include <iostream>
#include <cstring>
#include <vector>
#include <list>

#include "cOoRecord.h"

using namespace std;

namespace cOo {
    
    typedef list<Record>::iterator RecordIter;
    
    const long IndexNotFound = -1;
    
    class BreakPointFunction {
    
      public:
        
        BreakPointFunction( void );
        
        // to fill the BPF with data sets and properties
        void setProperties( long bpfId, long bpfType, bool bpfCraz );
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
        
        Time getStartTime( void ); Time getStopTime( void );
        Time getMinTime( void ); Time getMaxTime( void );
        
        void activate( void );
        void deactivate( void );
        bool isActive( void );
        
        long getType( void );
        long getSize( void );
        long getId( void );
        
        bool isCrazy( void );
        
        void print( void );
        
      protected:
        
        list<Record> record;        
        bool active; long head;
        long type; long id;
        bool craziness;
    };
}

#endif

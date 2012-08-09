#ifndef __cOoRecord__
#define __cOoRecord__

#include <string>

#include "cOoDataSet.h"

// a record is just a data
// set with a time stamp and
// some other junk to pass

using namespace std;

namespace cOo {
    
    typedef double Time;
    
    const short atBegin = 0;
    const short atEnd = 1;
    
    struct Record {
        
        DataSet data;
        
        short loc;
        string type;
        Time time;
        long id;
    };
}

#endif

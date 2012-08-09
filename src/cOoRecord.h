#ifndef __cOoRecord__
#define __cOoRecord__

#include <string>

#include "cOoDataSet.h"

// a record is just a data
// set with a time stamp and
// some other junk to pass

using namespace std;

namespace cOo {
    
    typedef short State;
    
    const State atAny = 0;
    const State atBegin = 1;
    const State atEnd = -1;
    
    struct Record {
        
        State state;
        DataSet data;
        string type;
        Time time;
        long id;
    };
}

#endif

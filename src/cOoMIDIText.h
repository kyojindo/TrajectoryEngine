//
//  cOoMIDIText.h
//  TrajectoryEngine
//
//  Created by johnty on 12-08-29.
//
//

#ifndef __TrajectoryEngine__cOoMIDIText__
#define __TrajectoryEngine__cOoMIDIText__

#include <iostream>
#include <vector>
#include "ofMain.h"

#define NUM_MIDI_VOICES 4

using namespace std;

namespace cOo {
    
    class MIDIText {
    public:
        
        vector<double> time;
        vector<double> pitch;
        vector<double> velocity;
        
        
        
    };
}


#endif /* defined(__TrajectoryEngine__cOoMIDIText__) */

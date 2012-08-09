#include "testApp.h"
#include "ofAppGlutWindow.h"

int main( void ) {
    
	ofAppGlutWindow window; // create a window
	ofSetupOpenGL( &window, 1024, 760, OF_WINDOW );
	ofRunApp( new testApp() ); // start the app
}

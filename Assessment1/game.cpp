#include <iostream>
#include "GamesEngineeringBase.h"

using namespace std;

int main() {
    // Create a canvas window with dimensions 1024x768 and title “Tiles"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "GE");
    bool running = true; // Variable to control the main loop's running state.


    while (running)
    {
        // Check for input (key presses or window events)
        canvas.checkInput();

        // Clear the window for the next frame rendering
        canvas.clear();

        

        // Draw(); 
        for (unsigned int x = 0; x < canvas.getWidth(); x++)
            for (unsigned int y = 0; y < canvas.getHeight(); y++)
                canvas.draw(x, y, 0, 0, 255);

        // draw the plane

    


        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }

}



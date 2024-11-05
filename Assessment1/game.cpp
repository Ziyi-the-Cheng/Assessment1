#include <iostream>
#include "GamesEngineeringBase.h"

using namespace std;

class sprite {
public:
    int HP;
    GamesEngineeringBase::Image image;

    sprite(){}
    void takeDamage(int damage = 1) {
        HP -= damage;
    }
    bool checkForLive() {
        return HP > 0;
    }
};

class Hero : public sprite {
public:
    Hero() {
        HP = 40;
        
        image.load("Resources/L.png");
    }

    Hero(int _HP) {
        HP = _HP;
        
    }

    void draw(GamesEngineeringBase::Window& canvas) {
        for (int i = 0; i < image.width;i++) {
            for (int j = 0; j < image.height;j++) {
                if (image.alphaAt(i, j) > 200)
                    canvas.draw(i + canvas.getWidth()/2 - image.width, j + canvas.getHeight()/2 - image.height, image.atUnchecked(i, j));
            }
        }
    }

};


const int canvasX = 1024;
const int canvasY = 768;
class NPC : public sprite {
public:
    int x;
    int y;
    NPC(int hp, string filename, int _x, int _y) {
        HP = hp;
        image.load(filename);
        x = _x;
        y = _y;
    }
    
    void draw(GamesEngineeringBase::Window& canvas) {
        for (int i = 0; i < image.width;i++) {
            if (i + x > 0 && i + x <= canvas.getWidth()) {
                for (int j = 0; j < image.height;j++) {
                    if (j + y > 0 && j + y < canvas.getHeight()) {
                        canvas.draw(i + x, j + y, image.atUnchecked(i, j));
                        //cout << i + x - image.width / 2 << "\n";
                    }
                }
            }
        }
    }

    //void getSpeed(GamesEngineeringBase::Window& canvas) {
    //    if (x != canvas.getWidth() / 2 || y != canvas.getHeight() / 2) {
    //        x += (2 * (canvas.getWidth() / 2 - x) / (sqrt((x - canvas.getWidth() / 2) * (x - canvas.getWidth() / 2) + (y - canvas.getHeight() / 2) * (y - canvas.getHeight() / 2))));
    //        y += (2 * (canvas.getHeight() / 2 - y) / (sqrt((x - canvas.getWidth() / 2) * (x - canvas.getWidth() / 2) + (y - canvas.getHeight() / 2) * (y - canvas.getHeight() / 2))));
    //        //cout << x << "\t" << y << "\n";
    //    }
    //}

    int getXspeed(GamesEngineeringBase::Window& canvas){
        int a;
        if (x != canvasX / 2) {
            a = 2 * (canvasX / 2 - x) / (sqrt((x - canvasX / 2) * (x - canvasX / 2) + (y - canvasY / 2) * (y - canvasY / 2)));
            //cout << ( - x) << "\n";
            return a;
        }
        return 0;
    }

    int getYspeed(GamesEngineeringBase::Window& canvas) {
        int b;
        if (y != canvasY / 2) {
            b = 2 * (canvasY / 2 - y) / (sqrt((x - canvasX / 2) * (x - canvasX / 2) + (y - canvasY / 2) * (y - canvasY / 2)));
            return b;
        }
        return 0;
    }

    void update(int _x, int _y) {
        x += _x;
        y += _y;
    }

    void goLeft() {
        x -= 2;
    }
    void goRight() {
        x += 2;
    }
    void goUp() {
        y -= 2;
    }
    void goDown() {
        y += 2;
    }
};

const unsigned int imageSize = 32;
class grass {
public:
    GamesEngineeringBase::Image image;
    grass() {
        image.load("Resources/0.png");
    }

    void draw(GamesEngineeringBase::Window& canvas, int x, int y) {
        for (int i = 0; i < imageSize;i++) {
            if (x + i >= 0 && x + i < canvas.getWidth()) {
                for (int j = 0; j < imageSize;j++) {
                    if (y + j >= 0 && y + j < canvas.getHeight()) {
                        canvas.draw(x + i, y + j, image.atUnchecked(i, j));
                    }
                }
            }
        }
    }

    void dall(GamesEngineeringBase::Window& canvas, int x, int y) {
        for (int i = 0; i < 8 * imageSize; i += 32) {
            for (int j = 0; j < 8 * imageSize; j += 32) {
                draw(canvas, i + x, j + y);
            }
        }
    }

};

class swarm {

};

int main() {
    // Create a canvas window with dimensions 1024x768 and title tiles"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "GE");
    bool running = true; // Variable to control the main loop's running state.

    GamesEngineeringBase::Timer tim;

    grass g;
    Hero h;
    int x = 0;
    int y = 0;
    NPC n(10, "Resources/3.png", 100, 100);

    while (running)
    {
        // Check for input (key presses or window events)
        canvas.checkInput();

        // Clear the window for the next frame rendering
        canvas.clear();

        float dt = tim.dt();
 
        if (canvas.keyPressed('W')) y += 2;
        if (canvas.keyPressed('A')) x += 2;
        if (canvas.keyPressed('S')) y -= 2;
        if (canvas.keyPressed('D')) x -= 2;

        if (canvas.keyPressed('W')) n.goDown();
        if (canvas.keyPressed('A')) n.goRight();
        if (canvas.keyPressed('S')) n.goUp();
        if (canvas.keyPressed('D')) n.goLeft();
        // Draw(); 
        for (unsigned int x = 0; x < canvas.getWidth(); x++)
            for (unsigned int y = 0; y < canvas.getHeight(); y++)
                canvas.draw(x, y, 0, 0, 255);

        // draw the plane
        g.dall(canvas, x + 1024, y + 768);
        h.draw(canvas);

        n.update(n.getXspeed(canvas), n.getYspeed(canvas));
        n.draw(canvas);


    


        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
        
    }

}



#include <iostream>
#include "GamesEngineeringBase.h"
#include<fstream>

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

int worldWidth = 1344;
int worldHeight = 1344;
class Hero : public sprite {
public:
    int x, y;

    Hero() {
        HP = 40;
        x = 0;
        y = 0;
        image.load("Resources/L.png");
    }

    Hero(int _HP, int _x, int _y) {
        HP = _HP;
        x = _x;
        y = _y;
        image.load("Resources/L.png");
    }

    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        for (int i = 0; i < image.width; i++) {
            if (x + i - image.width / 2 >= 0 && x + i - image.width / 2 < worldWidth) {
                for (int j = 0; j < image.height; j++) {
                    if (y + j - image.height / 2 >= 0 && y + j - image.height / 2 < worldHeight) {
                        if (image.alphaAt(i, j) > 200)
                            canvas.draw(x + i - image.width/2 - wx, y + j - image.height/2 - wy, image.atUnchecked(i, j));
                    } 
                }
            }
        }
    }

    void update(GamesEngineeringBase::Window& canvas, int _x, int _y) {
        x += _x;
        y += _y;

        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > worldWidth) x = worldWidth;
        if (y > worldHeight) y = worldHeight;
    }

};

class projectile {
public:
    GamesEngineeringBase::Image image;
    int x, y;
    int power;
    projectile(string filename, int _x, int _y, int p) {
        image.load("Resources/" + filename + ".png");
        x = _x;
        y = _y;
        power = p;
    }

    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        for (int i = 0; i < image.width; i++) {
            if (i + x - wx - image.width / 2 >= 0 && i + x - wx - image.width / 2 < canvasX) {
                for (int j = 0; j < image.height; j++) {
                    if (j + y - wy - image.height / 2 >= 0 && j + y - wy - image.height / 2 < canvasY) {
                        canvas.draw(i + x - wx - image.width / 2, j + y - wy - image.height / 2, image.atUnchecked(i, j));
                    }
                }
            }
        }
    }

    void update(int ox, int oy) {
        if (sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy)) > 50) {
            x += 2 * (ox - x) / (sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy)));
            y += 2 * (oy - y) / (sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy)));
        }

        if (x <= 0) x = 0;
        if (y <= 0) y = 0;
        if (x >= worldWidth) x = worldWidth;
        if (y >= worldHeight) y = worldHeight;
    }
};

const int canvasX = 1024;
const int canvasY = 768;
class NPC : public sprite {
public:
    int x;
    int y;

    //NPC(Hero& h):hero(h){}
    NPC() {}

    NPC(int hp, string filename, int _x, int _y) {
        HP = hp;
        image.load("Resources/" + filename + ".png");
        x = _x;
        y = _y;
    }
    
    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        for (int i = 0; i < image.width; i++) {
            if (i + x - wx - image.width / 2 >= 0 && i + x - wx - image.width / 2 < canvasX) {
                for (int j = 0; j < image.height; j++) {
                    if (j + y - wy - image.height / 2 >= 0 && j + y - wy - image.height / 2 < canvasY) {
                        canvas.draw(i + x - wx - image.width/2, j + y - wy - image.height/2, image.atUnchecked(i, j));
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

    //int getXspeed(){
    //    int a;
    //    if (x != hero->x) {
    //        a = 2 * (hero->x - x) / (sqrt((x - hero->x) * (x - hero->x) + (y - hero->y) * (y - hero->y)));
    //        //cout << hero->x << "\n";
    //        return a;
    //    }
    //    return 0;
    //}

    //int getYspeed() {
    //    int b;
    //    if (y != hero->y) {
    //        b = 2 * (hero->y - y) / (sqrt((x - hero->x) * (x - hero->x) + (y - hero->y) * (y - hero->y)));
    //        return b;
    //    }
    //    return 0;
    //}

    void update(int hx, int hy) {
        if (sqrt((x - hx) * (x - hx) + (y - hy) * (y - hy)) > 50) {
            x += 2 * (hx - x) / (sqrt((x - hx) * (x - hx) + (y - hy) * (y - hy)));
            y += 2 * (hy - y) / (sqrt((x - hx) * (x - hx) + (y - hy) * (y - hy)));
        }

        if (x <= 0) x = 0;
        if (y <= 0) y = 0;
        if (x >= worldWidth) x = worldWidth;
        if (y >= worldHeight) y = worldHeight;
    }
};

const unsigned int maxEnemyNum = 5;
class Swarm {
public:
    NPC* enemy[maxEnemyNum]{nullptr};
    unsigned int currentSize = 0;
    Swarm(){}

    void generateNPC() {
        if (currentSize < maxEnemyNum) {
            NPC* n = new NPC(10, "L2", rand()%500, rand()%500);
            enemy[currentSize++] = n;
        }
    }

    void checkDelete(){

    }

    void update(int hx, int hy) {
        generateNPC();
        for (int i = 0; i < currentSize;i++) {
            if (enemy[i] != nullptr) {
                enemy[i]->update(hx, hy);
            }
        }
    }

    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        for (int i = 0;i < currentSize;i++) {
            if (enemy[i] != nullptr)
                enemy[i]->draw(canvas, wx, wy);
        }
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

class tile {
    GamesEngineeringBase::Image sprite;
public:
    tile() {}
    void load(string filename) {
        sprite.load(filename);
    }
    void draw(GamesEngineeringBase::Window& canvas,int x, int y) {
        for (unsigned int i = 0; i < sprite.width; i++)
            if (x + i >= 0 && x + i < canvas.getWidth())
                for (unsigned int j = 0; j < sprite.height; j++)
                    if (y + j >= 0 && y + j < canvas.getHeight()) 
                        canvas.draw(x + i, y + j, sprite.atUnchecked(i, j));
    }
    unsigned int getHeight() { return sprite.height; }
    unsigned int getWidth() { return sprite.width; }
    GamesEngineeringBase::Image& getSprite() { return sprite; }
};

const unsigned int tileNum = 24;
class tileMap {
public:
    tile tiles[tileNum];
    int size = tileNum;
    tileMap() {}
    void load() {
        for (unsigned int i = 0; i < size; i++) {
            string filename;
            filename = "Resources/" + to_string(i) + ".png";
            tiles[i].load(filename);
        }
    }
    tile& operator[](unsigned int index) { return tiles[index]; }
};

const unsigned int mapWidth = 42;
const unsigned int mapHeight = 42;
class world{
public:
    tileMap tiles;
    unsigned int** map;
    world() {}

    world(string filename) {
        tiles.load();
        map = new unsigned int*[mapWidth];
        for (int i = 0; i < mapWidth; i++)
            map[i] = new unsigned int[mapHeight];
        ifstream infile("Resources/" + filename + ".txt");
        char input;
        int im = 0;
        for (int i = 0; i < mapWidth; i ++) {
            for (int j = 0; j < mapHeight; j ++) {
                infile >> input;
                if (input != ',') {
                    im += input - '0';
                    infile >> input;
                    if (input != ',') {
                        im = 10 * im + (input - '0');
                        infile >> input;
                    }
                }
                map[i][j] = im;
                im = 0;
            }
        }
        infile.close();
    }

    ~world() {
        delete[] map;
    }

    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        int length = 32;
        int X = wx / length;
        int Y = wy / length;
        int x = wx % length;
        int y = wy % length;
        for (int i = 0; i < mapWidth;i++) {
            for (int j = 0; j < mapHeight;j++) {
                tiles[map[i][j]].draw(canvas, j * length - wx, i * length - wy);
            }
        }

    }
};
//////////////////////////////////////////////

class preTiles {
public:
    GamesEngineeringBase::Image* allT;
    int size;
    int xOffset;
    int yOffset;
    preTiles() {
        size = 24;
        allT = new GamesEngineeringBase::Image[size];
        for (int i = 0;i < size;i++) {
            allT[i].load("Resources/" + to_string(i) + ".png");
        }
        xOffset = 0;
        yOffset = 0;
    }

    void draw(GamesEngineeringBase::Window& canvas, int x, int y, int n) {
        for (int i = 0;i < 32;i++) {
            if (x + i >= 0 && x + i < canvas.getWidth()) {
                for (int j = 0;j < 32;j++) {
                    if (y + j >= 0 && y + j < canvas.getHeight())
                        canvas.draw(x + i, y + j, allT[n].atUnchecked(i, j));
                }
            }
        }
    }

    void adraw(GamesEngineeringBase::Window& canvas) {
        ifstream infile("Resources/tiles.txt");
        char input;
        int im = 0;
        for (int i = 0; i < 1344; i += 32) {
            for (int j = 0; j < 1344; j += 32) {
                infile >> input;
                if (input != ',') {
                    im += input - '0';
                    infile >> input;
                    if (input != ',') {
                        im = 10 * im + (input - '0');
                        infile >> input;
                    }
                }
                //cout << im << ',';
                draw(canvas, j + xOffset, i + yOffset, im);
                im = 0;
            }
        }
        infile.close();

    }

    void update(int _x, int _y) {
        xOffset += _x;
        yOffset += _y;
    }

    /*void ttest() {
        ifstream infile("Resources/tiles.txt");
        char input;
        int im = 0;
        for (int i = 0; i < 115;i++) {
            infile >> input;
            if (input != ',') {
                im += input - '0';
                infile >> input;
                if (input != ',') {
                    im = 10 * im + (input - '0');
                    infile >> input;
                }
            }
            cout << im << ",";
            im = 0;
        }
            

        infile.close();
    }*/

};

class camera {
public:
    int x, y;
    Hero hero;
    world w;
    Swarm swarm;
    camera(Hero& _hero, world& _w, Swarm& _s) {
        hero = _hero;
        w = _w;
        swarm = _s;
        x = hero.x;
        y = hero.y;
    }

    void draw(GamesEngineeringBase::Window& canvas) {
        int xOffset = x - canvasX / 2;
        int yOffset = y - canvasY / 2;
        w.draw(canvas, xOffset, yOffset);
        hero.draw(canvas, xOffset, yOffset);
        swarm.draw(canvas, xOffset, yOffset);
        swarm.update(hero.x, hero.y);
    }

    void update(GamesEngineeringBase::Window& canvas, int _x, int _y) {
        hero.update(canvas, _x, _y);
        x = hero.x;
        y = hero.y;

        if (x < canvas.getWidth() / 2) x = canvas.getWidth() / 2;
        if (y < canvas.getHeight() / 2) y = canvas.getHeight() / 2;
        if (x > worldWidth - canvas.getWidth() / 2) x = worldWidth - canvas.getWidth() / 2;
        if (y > worldWidth - canvas.getHeight() / 2) y = worldWidth - canvas.getHeight() / 2;
    }


};

int main() {
    // Create a canvas window with dimensions 1024x768 and title tiles"
    GamesEngineeringBase::Window canvas;
    canvas.create(1024, 768, "GE");
    bool running = true; // Variable to control the main loop's running state.

    GamesEngineeringBase::Timer tim;

    Hero h(40,512,384);
    //int x = 0;
    //int y = 0;
    NPC n(10, "L2", 100, 100);
    //preTiles pp;
    
    world w("tiles");

    Swarm s;
    camera cam(h, w, s);

    while (running)
    {
        // Check for input (key presses or window events)
        canvas.checkInput();

        // Clear the window for the next frame rendering
        canvas.clear();

        float dt = tim.dt();

        if (canvas.keyPressed('W')) cam.update(canvas, 0, -3);
        if (canvas.keyPressed('A')) cam.update(canvas, -3, 0);
        if (canvas.keyPressed('S')) cam.update(canvas, 0, 3);
        if (canvas.keyPressed('D')) cam.update(canvas, 3, 0);
        
        cam.draw(canvas);
        
        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
        
    }

}



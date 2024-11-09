#include <iostream>
#include "GamesEngineeringBase.h"
#include <fstream>
#include <random>

using namespace std;

class sprite {
public:
    int x, y;
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

class vec {
    int x, y;
    vec(int _x, int _y) {
        x = _x;
        y = _y;
    }

    float distanceTo(vec input) {
        float a = (x - input.x) * (x - input.x);
        float b = (y - input.y) * (y - input.y);
        return sqrtf(a + b);
    }
};

class timeCount {
public:
    float timeElapsed;
    float timeThreshold;
    bool ready = true;

    timeCount(){
        timeElapsed = 0.f;
    }

    void set(float t) {
        timeThreshold = t;
    }

    bool count(float dt) {
        timeElapsed += dt;
        if (timeElapsed > timeThreshold) {
            timeElapsed = 0; 
            return true;
        }
        return false;
    }

    void coolDown(float dt) {
        if (!ready) {
            timeElapsed += dt;
            if (timeElapsed > timeThreshold) {
                timeElapsed = 0;
                ready = true;
            }
        }
    }
};

int worldWidth = 1344;
int worldHeight = 1344;
const unsigned int bulletSize = 100;
const int canvasX = 1024;
const int canvasY = 768;

class projectile {
public:
    GamesEngineeringBase::Image image;
    int x, y;
    int ox, oy;
    int power;
    int xs, ys;
    projectile(string filename, int _x, int _y, int p, int _ox, int _oy) {
        image.load("Resources/" + filename + ".png");
        x = _x;
        y = _y;
        power = p;
        ox = _ox;
        oy = _oy;
        xs = 2 * (ox - x) / (sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy)));
        ys = 2 * (oy - y) / (sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy)));
    }

    bool checkCollide(sprite& s) {
        if (sqrt((x - s.x) * (x - s.x) + (y - s.y) * (y - s.y)) < 15)
            return true;
        return false;
    }

    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        for (int i = 0; i < image.width; i++) {
            if (i + x - wx - image.width / 2 >= 0 && i + x - wx - image.width / 2 < canvasX) {
                for (int j = 0; j < image.height; j++) {
                    if (j + y - wy - image.height / 2 >= 0 && j + y - wy - image.height / 2 < canvasY) {
                        if (image.alphaAt(i, j) > 100)
                            canvas.draw(i + x - wx - image.width / 2, j + y - wy - image.height / 2, image.atUnchecked(i, j));
                    }
                }
            }
        }
    }

    void update() {
        if (sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy)) > 5) {
            x += xs;
            y += ys;
        }
    }
};

class Hero : public sprite {
public:
    unsigned int currentBullet = 0;
    projectile* bullets[bulletSize];
    timeCount fireCoolDown;
    Hero() {
        HP = 40;
        x = 0;
        y = 0;
        image.load("Resources/hero.png");
        fireCoolDown.set(3.f);
    }

    Hero(int _HP, int _x, int _y) {
        HP = _HP;
        x = _x;
        y = _y;
        image.load("Resources/hero.png");
        fireCoolDown.set(3.f);
    }

    void checkForDelete() {
        for (int i = 0; i < currentBullet;i++) {
            if (bullets[i] != nullptr) {
                if (sqrt((bullets[i]->x - bullets[i]->ox) * (bullets[i]->x - bullets[i]->ox) + (bullets[i]->y - bullets[i]->oy) * (bullets[i]->y - bullets[i]->oy)) <= 5) {
                    bullets[i] = nullptr;
                }
            }  
        }
    }


    void linearAttack(int ox, int oy, float dt) {
        if (fireCoolDown.count(dt)) {
            if (currentBullet < bulletSize) {
                projectile* p = new projectile("heroBullet", x, y, 3, ox, oy);
                bullets[currentBullet++] = p;
            }
        }
    }

    void aoeAttack() {
        
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

        for (int n = 0; n < currentBullet; n++)
            if (bullets[n] != nullptr)
                bullets[n]->draw(canvas, wx, wy);
        for (int m = 0; m < currentBullet; m++)
            if (bullets[m] != nullptr)
                bullets[m]->update();
        checkForDelete();
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

class NPC : public sprite {
public:
    unsigned int currentBullet = 0;
    projectile* bullets[bulletSize];
    timeCount fireCoolDown;
    virtual void update(int x, int y, float dt) {};
    virtual void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {};
    float distanceTo(int hx, int hy) {
        return sqrt(((float)x - (float)hx) * ((float)x - (float)hx) + ((float)y - (float)hy) * ((float)y - (float)hy));
    }
    bool melee(int hx, int hy) {
        return sqrt((x - hx) * (x - hx) + (y - hy) * (y - hy)) <= 20;
    }
};

class meleeNPC : public NPC {
public:

    //NPC(Hero& h):hero(h){}
    meleeNPC() {}

    meleeNPC(int hp, string filename, int _x, int _y) {
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
                        if (image.alphaAt(i, j) > 200)
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

    void update(int hx, int hy, float dt) {
        if (sqrt((x - hx) * (x - hx) + (y - hy) * (y - hy)) > 20) {
            x += 2 * (hx - x) / (sqrt((x - hx) * (x - hx) + (y - hy) * (y - hy)));
            y += 2 * (hy - y) / (sqrt((x - hx) * (x - hx) + (y - hy) * (y - hy)));
        }

        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > worldWidth) x = worldWidth;
        if (y > worldHeight) y = worldHeight;
    }
};

class rangeNPC : public NPC {
public:
    

    rangeNPC() {}
    rangeNPC(int hp, string filename, int _x, int _y) {
        HP = hp;
        image.load("Resources/" + filename + ".png");
        x = _x;
        y = _y;
        fireCoolDown.set(2.f);
    }

    void checkForDelete() {
        for (int i = 0; i < currentBullet;i++) {
            if (bullets[i] != nullptr) {
                if (sqrt((bullets[i]->x - bullets[i]->ox) * (bullets[i]->x - bullets[i]->ox) + (bullets[i]->y - bullets[i]->oy) * (bullets[i]->y - bullets[i]->oy)) <= 5) {
                    bullets[i] = nullptr;
                }
            }
        }
    }

    void rangeAttack(int ox, int oy) {
        if (currentBullet < bulletSize) {
            projectile* p = new projectile("NPCbullet", x, y, 3, ox, oy);
            bullets[currentBullet++] = p;
        }
    }

    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        for (int i = 0; i < image.width; i++) {
            if (i + x - wx - image.width / 2 >= 0 && i + x - wx - image.width / 2 < canvasX) {
                for (int j = 0; j < image.height; j++) {
                    if (j + y - wy - image.height / 2 >= 0 && j + y - wy - image.height / 2 < canvasY) {
                        if (image.alphaAt(i, j) > 200)
                            canvas.draw(i + x - wx - image.width / 2, j + y - wy - image.height / 2, image.atUnchecked(i, j));
                        //cout << i + x - image.width / 2 << "\n";
                    }
                }
            }
        }

        for (int n = 0; n < currentBullet; n++)
            if (bullets[n] != nullptr)
                bullets[n]->draw(canvas, wx, wy);
        for (int m = 0; m < currentBullet; m++)
            if (bullets[m] != nullptr)
                bullets[m]->update();
        checkForDelete();
    }

    void update (int ox, int oy, float dt) override{
        if (sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy)) > 350) {
            x += 2 * (ox - x) / (sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy)));
            y += 2 * (oy - y) / (sqrt((x - ox) * (x - ox) + (y - oy) * (y - oy)));
        }

        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x > worldWidth) x = worldWidth;
        if (y > worldHeight) y = worldHeight;
        if (fireCoolDown.count(dt)) {
            rangeAttack(ox, oy);
        }
    }
};

const unsigned int maxEnemyNum = 10;
class Swarm {
public:
    NPC* enemy[maxEnemyNum];
    unsigned int currentSize = 0;
    int position = 1;
    timeCount t;

    Swarm(){}

    void generateNPC(int cx, int cy, float dt) {
        if (currentSize < maxEnemyNum) {
            meleeNPC* n = new meleeNPC(10, "npc1", (cx + 528) * position, rand() % 800 - 400);
            rangeNPC* r = new rangeNPC(10, "npc2", (cx + 528) * position, rand() % 800 - 400);
            if (rand() % 10 < 3)
                enemy[currentSize++] = r;
            else 
                enemy[currentSize++] = n;
            position = -1 * position;
        }
    }

    void checkDelete(){

    }

    void update(int hx, int hy, int cx, int cy, float dt) {
        t.set(4.f);
        if (t.count(dt)) {
            generateNPC(cx, cy, dt);
        }
        for (int i = 0; i < currentSize;i++) {
            if (enemy[i] != nullptr) {
                enemy[i]->update(hx, hy, dt);
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
    bool passable = true;
public:
    tile() {}
    void load(string filename) {
        sprite.load(filename);
        if (filename == "14" || filename == "15" || filename == "16" || filename == "17" || filename == "18" || filename == "19" || filename == "20" || filename == "21" || filename == "22")
            passable = false;
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
    bool getPassable() { return passable; }
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
        for (int i = 0; i < mapWidth;i++) {
            for (int j = 0; j < mapHeight;j++) {
                tiles[map[i][j]].draw(canvas, j * length - wx, i * length - wy);
            }
        }

    }

    //Check if the tile at hero's position is passable;
    bool collide(int hx, int hy) {
        //Get the corresponding tile at hero position
        int X = hx / imageSize; 
        int Y = hy / imageSize;
        return tiles[map[X][Y]].getPassable();
    }
};

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
    Hero* hero;
    world w;
    Swarm* swarm;
    timeCount ht;
    timeCount aoeCoolDown;
    

    camera(Hero& _hero, world& _w, Swarm& _s) {
        hero = &_hero;
        w = _w;
        swarm = &_s;
        x = hero->x;
        y = hero->y;
        aoeCoolDown.set(3.f);
    }

    bool swarmEmpty() {
        for (int i = 0; i < swarm->currentSize;i++) {
            if (swarm->enemy[i] != nullptr)
                return false;
        }
        return true;
    }

    int checkClosestNPC() {
        float closest = 0.f;
        int current = 0;
        for (int i = 0; i < swarm->currentSize; i++) {
            if (swarm->enemy[i] != nullptr) {
                float n = swarm->enemy[i]->distanceTo(hero->x, hero->y);
                if (n > closest) {
                    closest = n;
                    current = i;
                }
            }            
        }
        return current;
    }

    bool inAOErange(NPC& n) {
        return n.distanceTo(x, y) < 300.f;
    }

    void aoeAttack() {
        if (aoeCoolDown.ready) {
            for (int i = 0;i < swarm->currentSize;i++) {
                if (swarm->enemy[i] != nullptr)
                    if (inAOErange(*swarm->enemy[i])) {
                        swarm->enemy[i]->takeDamage(15);
                        if (!swarm->enemy[i]->checkForLive())
                            swarm->enemy[i] = nullptr;
                    }
            }
            aoeCoolDown.ready = false;
        }
    }

    void heroAttack(float dt) {
        if (!swarmEmpty()) {
            int closest = checkClosestNPC();
            for (int i = 0; i < swarm->currentSize;i++) {
                if (swarm->enemy[i] != nullptr) {
                    //cout << swarm->enemy[closest]->x << "\t" << swarm->enemy[closest]->y << "\n";
                    hero->linearAttack(swarm->enemy[closest]->x, swarm->enemy[closest]->y, dt);
                }
            }
        }
    }

    void npcRangeAttack() {
        for (int i = 0; i < swarm->currentSize;i++) {
            if (swarm->enemy[i] != nullptr) {
                for (int j = 0;j < swarm->enemy[i]->currentBullet;j++) {
                    if (swarm->enemy[i]->bullets[j] != nullptr) {
                        if (swarm->enemy[i]->bullets[j]->checkCollide(*hero)) {
                            hero->takeDamage(swarm->enemy[i]->bullets[j]->power);
                            swarm->enemy[i]->bullets[j] = nullptr;
                        }
                    }
                }
            }
        }
    }
    
    void heroGetAttack() {
        for (int i = 0; i < swarm->currentSize;i++) {
            if (swarm->enemy[i] != nullptr) {
                if (swarm->enemy[i]->melee(hero->x, hero->y)) {
                    hero->takeDamage(20);
                    swarm->enemy[i] = nullptr;
                }
            } 
        }
    }

    void NPCgetAttack() {
        int bullet = 0;
        for (int i = 0;i < hero->currentBullet;i++) {
            if (hero->bullets[i] != nullptr) {
                for (int j = 0;j < swarm->currentSize;j++) {
                    if (swarm->enemy[j] != nullptr) {
                        float a = hero->bullets[i]->x - swarm->enemy[j]->x;
                        float b = hero->bullets[i]->y - swarm->enemy[j]->y;
                        float distance = sqrtf(a * a + b * b);
                        if (distance < 15) {
                            bullet = i;
                            swarm->enemy[j]->takeDamage(hero->bullets[i]->power);
                            if (!swarm->enemy[j]->checkForLive())
                                swarm->enemy[j] = nullptr;
                        }
                    }
                }
            }
        }
        hero->bullets[bullet] = nullptr;
    }

    void heroCollide() {
        if (w.collide(x, y)) {

        }
    }

    void draw(GamesEngineeringBase::Window& canvas) {
        int xOffset = x - canvasX / 2;
        int yOffset = y - canvasY / 2;
        /*NPCgetAttack();*/
        w.draw(canvas, xOffset, yOffset);
        hero->draw(canvas, xOffset, yOffset);
        swarm->draw(canvas, xOffset, yOffset);
        
        
    }

    void update(GamesEngineeringBase::Window& canvas, float dt) {
        x = hero->x; //Update camera coordinates
        y = hero->y;
        swarm->update(hero->x, hero->y, x, y, dt);
        heroAttack(dt);
        heroGetAttack();
        NPCgetAttack();
        npcRangeAttack();
        aoeCoolDown.coolDown(dt);
        
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
    meleeNPC n(10, "L2", 100, 100);
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
        

        if (canvas.keyPressed('W')) h.update(canvas, 0, -3);
        if (canvas.keyPressed('A')) h.update(canvas, -3, 0);
        if (canvas.keyPressed('S')) h.update(canvas, 0, 3);
        if (canvas.keyPressed('D')) h.update(canvas, 3, 0);
        if (canvas.keyPressed('Q')) cam.aoeAttack();

        cam.draw(canvas);
        cam.update(canvas, dt);
        
        if (!h.checkForLive()) break;
        // Display the frame on the screen. This must be called once the frame is finished in order to display the frame.
        canvas.present();
    }
}



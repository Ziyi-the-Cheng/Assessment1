#include <iostream>
#include "GamesEngineeringBase.h"
#include <fstream>
#include <random>

using namespace std;

class vec {
public:
    int x, y;
    vec() {
        x = 0;
        y = 0;
    }
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

class sprite {
public:
    vec position;
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
    void draw(GamesEngineeringBase::Window& canvas, int x, int y) {
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
const unsigned int imageSize = 32;
class world {
public:
    tileMap tiles;
    unsigned int** map;
    world() {}

    world(string filename) {
        tiles.load();
        map = new unsigned int* [mapWidth];
        for (int i = 0; i < mapWidth; i++)
            map[i] = new unsigned int[mapHeight];
        ifstream infile("Resources/" + filename + ".txt");
        char input;
        int im = 0;
        for (int i = 0; i < mapWidth; i++) {
            for (int j = 0; j < mapHeight; j++) {
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
    bool collide(vec hero) {
        //Get the corresponding tile at hero position
        int X = hero.x / imageSize;
        int Y = hero.y / imageSize;
        return tiles[map[X][Y]].getPassable();
    }

    vec tilePositionAt(vec hero) {
        vec t;
        int X = hero.x / imageSize;
        int Y = hero.y / imageSize;
        t.x = X * imageSize;
        t.y = Y * imageSize;
        return t;
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
    vec position;
    vec object;
    int power;
    vec speed;
    projectile(string filename, int _x, int _y, int p, vec _object) {
        image.load("Resources/" + filename + ".png");
        position.x = _x;
        position.y = _y;
        power = p;
        object.x = _object.x;
        object.y = _object.y;
        speed.x = 2 * (object.x - position.x) / position.distanceTo(object); //Get the acceleration of bullet in x and y direction
        speed.y = 2 * (object.y - position.y) / position.distanceTo(object);
    }

    //Check if the bullet hit its object
    bool checkCollide(sprite& s) {
        if (position.distanceTo(s.position) < 15)
            return true;
        return false;
    }

    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        for (int i = 0; i < image.width; i++) {
            if (i + position.x - wx - image.width / 2 >= 0 && i + position.x - wx - image.width / 2 < canvasX) {
                for (int j = 0; j < image.height; j++) {
                    if (j + position.y - wy - image.height / 2 >= 0 && j + position.y - wy - image.height / 2 < canvasY) {
                        if (image.alphaAt(i, j) > 100)
                            canvas.draw(i + position.x - wx - image.width / 2, j + position.y - wy - image.height / 2, image.atUnchecked(i, j));
                    }
                }
            }
        }
    }

    void update() {
        if (position.distanceTo(object) > 5) {
            position.x += speed.x;
            position.y += speed.y;
        }
    }
};

const unsigned int heroBulletDamage = 5;
class Hero : public sprite {
public:
    unsigned int currentBullet = 0;
    projectile* bullets[bulletSize];
    timeCount fireCoolDown;
    world* w = new world("tiles");
    Hero() {
        HP = 40;
        position.x = 0;
        position.y = 0;
        image.load("Resources/hero.png");
        fireCoolDown.set(3.f);
    }

    Hero(int _HP, int _x, int _y) {
        HP = _HP;
        position.x = _x;
        position.y = _y;
        image.load("Resources/hero.png");
        fireCoolDown.set(3.f);
    }
    //Delete the bullet once it get to its destination without hitting any enemy
    void checkForDelete() {
        for (int i = 0; i < currentBullet;i++) {
            if (bullets[i] != nullptr) {
                if (bullets[i]->position.distanceTo(bullets[i]->object) < 5) {
                    bullets[i] = nullptr;
                }
            }  
        }
    }

    //Generate a linear attack automatically over time
    void linearAttack(vec object, float dt) {
        if (fireCoolDown.count(dt)) {
            if (currentBullet < bulletSize) {
                projectile* p = new projectile("heroBullet", position.x, position.y, heroBulletDamage, object);
                bullets[currentBullet++] = p;
            }
        }
    }

    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        for (int i = 0; i < image.width; i++) {
            if (position.x + i - image.width / 2 >= 0 && position.x + i - image.width / 2 < worldWidth) {
                for (int j = 0; j < image.height; j++) {
                    if (position.y + j - image.height / 2 >= 0 && position.y + j - image.height / 2 < worldHeight) {
                        if (image.alphaAt(i, j) > 200)
                            canvas.draw(position.x + i - image.width/2 - wx, position.y + j - image.height/2 - wy, image.atUnchecked(i, j));
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
        /*if (!w->collide(position)) {
            vec t = w->tilePositionAt(position);
            if (_x > 0)
                position.x = t.x;
            if (_x < 0)
                position.x = t.x + imageSize;
            if (_y > 0)
                position.y = t.y;
            if (_y < 0)
                position.y = t.y + imageSize;
        }*/
        position.x += _x;
        position.y += _y;

        if (position.x < 0) position.x = 0;
        if (position.y < 0) position.y = 0;
        if (position.x > worldWidth) position.x = worldWidth;
        if (position.y > worldHeight) position.y = worldHeight;
    }

    
};

class NPC : public sprite {
public:
    unsigned int currentBullet = 0;
    projectile* bullets[bulletSize];
    timeCount fireCoolDown;
    virtual void update(vec hero, float dt) {};
    virtual void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {};
    float distanceTo(vec hero) { return position.distanceTo(hero); }
    bool meleeAttack(vec hero) { return position.distanceTo(hero) <= 20; }
};

class meleeNPC : public NPC {
public:

    //NPC(Hero& h):hero(h){}
    meleeNPC() {}

    meleeNPC(int hp, string filename, int _x, int _y) {
        HP = hp;
        image.load("Resources/" + filename + ".png");
        position.x = _x;
        position.y = _y;
    }
    
    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        for (int i = 0; i < image.width; i++) {
            if (i + position.x - wx - image.width / 2 >= 0 && i + position.x - wx - image.width / 2 < canvasX) {
                for (int j = 0; j < image.height; j++) {
                    if (j + position.y - wy - image.height / 2 >= 0 && j + position.y - wy - image.height / 2 < canvasY) {
                        if (image.alphaAt(i, j) > 200)
                            canvas.draw(i + position.x - wx - image.width/2, j + position.y - wy - image.height/2, image.atUnchecked(i, j));
                    }
                }
            }
        }
    }

    void update(vec hero, float dt) {
        if (position.distanceTo(hero) > 20) {
            position.x += 2 * (hero.x - position.x) / position.distanceTo(hero);
            position.y += 2 * (hero.y - position.y) / position.distanceTo(hero);
        }

        if (position.x < 0) position.x = 0;
        if (position.y < 0) position.y = 0;
        if (position.x > worldWidth) position.x = worldWidth;
        if (position.y > worldHeight) position.y = worldHeight;
    }
};

const unsigned int enemyBulletDamage = 5;
class rangeNPC : public NPC {
public:
    

    rangeNPC() {}
    rangeNPC(int hp, string filename, int _x, int _y) {
        HP = hp;
        image.load("Resources/" + filename + ".png");
        position.x = _x;
        position.y = _y;
        fireCoolDown.set(2.f);
    }

    void checkForDelete() {
        for (int i = 0; i < currentBullet;i++) {
            if (bullets[i] != nullptr) {
                if (bullets[i]->position.distanceTo(bullets[i]->object) < 5) {
                    bullets[i] = nullptr;
                }
            }
        }
    }

    void rangeAttack(vec hero) {
        if (currentBullet < bulletSize) {
            projectile* p = new projectile("NPCbullet", position.x, position.y, enemyBulletDamage, hero);
            bullets[currentBullet++] = p;
        }
    }

    void draw(GamesEngineeringBase::Window& canvas, int wx, int wy) {
        for (int i = 0; i < image.width; i++) {
            if (i + position.x - wx - image.width / 2 >= 0 && i + position.x - wx - image.width / 2 < canvasX) {
                for (int j = 0; j < image.height; j++) {
                    if (j + position.y - wy - image.height / 2 >= 0 && j + position.y - wy - image.height / 2 < canvasY) {
                        if (image.alphaAt(i, j) > 200)
                            canvas.draw(i + position.x - wx - image.width / 2, j + position.y - wy - image.height / 2, image.atUnchecked(i, j));
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

    void update (vec hero, float dt) override{
        if (position.distanceTo(hero) > 350) {
            position.x += 2 * (hero.x - position.x) / position.distanceTo(hero);
            position.y += 2 * (hero.y - position.y) / position.distanceTo(hero);
        }

        if (position.x < 0) position.x = 0;
        if (position.y < 0) position.y = 0;
        if (position.x > worldWidth) position.x = worldWidth;
        if (position.y > worldHeight) position.y = worldHeight;
        if (fireCoolDown.count(dt)) {
            rangeAttack(hero);
        }
    }
};

const unsigned int maxEnemyNum = 10;
class Swarm {
public:
    NPC* enemy[maxEnemyNum];
    unsigned int currentSize = 0;
    int side = 1;
    timeCount t;

    Swarm(){}

    void generateNPC(vec camera, float dt) {
        int determine = rand() % 4;
        if (currentSize < maxEnemyNum) {
            meleeNPC* n1 = new meleeNPC(10, "npc1", (camera.x + 528) * side, rand() % 800 - 400);
            meleeNPC* n2 = new meleeNPC(10, "npc3", (camera.x + 528) * side, rand() % 800 - 400);
            meleeNPC* n3 = new meleeNPC(10, "npc4", (camera.x + 528) * side, rand() % 800 - 400);
            rangeNPC* n4 = new rangeNPC(10, "npc2", (camera.x + 528) * side, rand() % 800 - 400);
            if (determine == 0)
                enemy[currentSize++] = n1;
            if (determine == 1)
                enemy[currentSize++] = n2;
            if (determine == 2)
                enemy[currentSize++] = n3;
            if (determine == 3)
                enemy[currentSize++] = n4;
            side = -1 * side;
        }
    }

    void checkDelete(){

    }

    void update(vec hero, vec camera, float dt) {
        t.set(4.f);
        if (t.count(dt)) {
            generateNPC(camera, dt);
        }
        for (int i = 0; i < currentSize;i++) {
            if (enemy[i] != nullptr) {
                enemy[i]->update(hero, dt);
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

const unsigned int enemyMeleeAttackDamage = 5;
const unsigned int heroAoeDamage = 15;
class camera {
public:
    vec position;
    Hero* hero;
    world w;
    Swarm* swarm;
    timeCount ht;
    timeCount aoeCoolDown;
    

    camera(Hero& _hero, world& _w, Swarm& _s) {
        hero = &_hero;
        w = _w;
        swarm = &_s;
        position.x = hero->position.x;
        position.y = hero->position.y;
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
                float n = swarm->enemy[i]->distanceTo(hero->position);
                if (n > closest) {
                    closest = n;
                    current = i;
                }
            }            
        }
        return current;
    }

    bool inAOErange(NPC& n) {
        return n.distanceTo(position) < 300.f;
    }

    void aoeAttack() {
        if (aoeCoolDown.ready) {
            for (int i = 0;i < swarm->currentSize;i++) {
                if (swarm->enemy[i] != nullptr)
                    if (inAOErange(*swarm->enemy[i])) {
                        swarm->enemy[i]->takeDamage(heroAoeDamage);
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
                    hero->linearAttack(swarm->enemy[closest]->position, dt);
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
                if (swarm->enemy[i]->meleeAttack(hero->position)) {
                    hero->takeDamage(enemyMeleeAttackDamage);
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
                        if (hero->bullets[i]->position.distanceTo(swarm->enemy[j]->position) < 15) {
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

    void draw(GamesEngineeringBase::Window& canvas) {
        int xOffset = position.x - canvasX / 2;
        int yOffset = position.y - canvasY / 2;
        /*NPCgetAttack();*/
        w.draw(canvas, xOffset, yOffset);
        hero->draw(canvas, xOffset, yOffset);
        swarm->draw(canvas, xOffset, yOffset);
    }

    void update(GamesEngineeringBase::Window& canvas, float dt) {
        position.x = hero->position.x; //Update camera coordinates
        position.y = hero->position.y;
        swarm->update(hero->position, position, dt);
        heroAttack(dt);
        heroGetAttack();
        NPCgetAttack();
        npcRangeAttack();
        aoeCoolDown.coolDown(dt);
        
        if (position.x < canvas.getWidth() / 2) position.x = canvas.getWidth() / 2;
        if (position.y < canvas.getHeight() / 2) position.y = canvas.getHeight() / 2;
        if (position.x > worldWidth - canvas.getWidth() / 2) position.x = worldWidth - canvas.getWidth() / 2;
        if (position.y > worldWidth - canvas.getHeight() / 2) position.y = worldWidth - canvas.getHeight() / 2;
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

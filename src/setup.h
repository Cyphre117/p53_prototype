#ifndef SETUP_H
#define SETUP_H

#include <SDL2/SDL.h>
#include <vector>
#include "node.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 700

#define RADIUS 32
#define GRID_WIDTH (SCREEN_WIDTH/(RADIUS*2))
#define GRID_HEGIHT int(SCREEN_HEIGHT/(RADIUS*1.5))

extern SDL_Renderer* ren;
extern SDL_Window* win;
extern Node* nodes[GRID_WIDTH * GRID_HEGIHT];

class Mouse {
public:
    Mouse():x(0),y(0),pressed(false),down(false),wasDown(false) {}
    ~Mouse() {}
    void Update() {
        wasDown = down;
        down = SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT);
        pressed = (down == true && wasDown == false);
    }
    int x,y;
    bool pressed;
    bool down;
private:
    bool wasDown;
};
extern Mouse mouse;

// setup SDL
void setup();

// processes events
// returns true when finished
bool done();

// flip buffers, clear display
void display();

// draws all the nodes on the board
void renderNodes();

void updateNodes();

void renderCursor();

void Menu();

// cleanup SDL resources
void cleanup();

#endif

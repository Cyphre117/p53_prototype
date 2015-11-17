#ifndef SETUP_H
#define SETUP_H

#include <SDL2/SDL.h>
#include <vector>
#include "node.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 700

// its probably better if this is always a square
#define BOARD_WIDTH 2048
#define BOARD_HEIGHT 2048

#define RADIUS 32
#define GRID_WIDTH (BOARD_WIDTH/(RADIUS*2))
#define GRID_HEGIHT int(BOARD_HEIGHT/(RADIUS*1.5))

extern SDL_Renderer* ren;
extern SDL_Window* win;
extern Node* nodes[GRID_WIDTH * GRID_HEGIHT];
extern SDL_Rect viewport;

class Mouse {
public:
    Mouse():x(SCREEN_WIDTH/2),y(SCREEN_HEIGHT/2),pressed(false),down(false),wasDown(false) {}
    ~Mouse() {}
    void Update() {
        wasDown = down;
		Uint32 buttons = SDL_GetMouseState(NULL, NULL);
		down = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		altDown = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
        pressed = (down == true && wasDown == false);
    }
    int x,y;
    bool pressed; // left mouse transitioned from up to down
    bool down; // left mouse is down
	bool altDown; // right mouse is down
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

void renderCursor(int x, int y);

void Menu();

// cleanup SDL resources
void cleanup();

#endif

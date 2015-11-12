#ifndef NODE_H
#define NODE_H

#include <SDL2/SDL.h>

extern SDL_Renderer* ren;

class Node {
public:
    Node(int x, int y, int r, Uint32 c = 0xffffffff);
    ~Node();

    // pretends the hexagon is a circle
    bool isOver(int x, int y);

    // draws the edges of the hexagon pointy topped
    void Render();
    
    // the centre of the node
    SDL_Point centre;

    // from flat edge to falt edge
    int radius;

    // colour of the outline
    // RGBA
    Uint32 colour;
};

#endif

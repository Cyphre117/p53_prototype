#include "node.h"

Node::Node(int x, int y, int r, Uint32 c):
centre{x,y},
radius(r),
colour(c)
{
}

Node::~Node()
{
}

bool Node::isOver(int x, int y)
{
    int dx = centre.x - x;
    int dy = centre.y - y;

    // true if the radius greater than the distance to the point
    return (radius*radius) > (dx*dx + dy*dy);
}

void Node::Render()
{
    SDL_SetRenderDrawColor( ren,
            colour >> 24 & 0xff,
            colour >> 16 & 0xff,
            colour >>  8 & 0xff,
            colour       & 0xff );

    // left side
    SDL_RenderDrawLine( ren, centre.x - radius, centre.y + radius/2 + 1,
            centre.x - radius, centre.y - radius/2 );

    // top left
    SDL_RenderDrawLine( ren, centre.x - radius, centre.y - radius/2,
            centre.x, centre.y - radius );

    // top right
    SDL_RenderDrawLine( ren, centre.x + 1, centre.y - radius,
            centre.x + radius + 1, centre.y - radius/2 );

    // right
    SDL_RenderDrawLine( ren, centre.x + radius + 1, centre.y - radius/2,
            centre.x + radius + 1, centre.y + radius/2 );
    
    // bottom right
    SDL_RenderDrawLine( ren, centre.x + radius + 1, centre.y + radius/2 + 1,
            centre.x + 1, centre.y + radius + 1 );
    
    // bottom left
    SDL_RenderDrawLine( ren, centre.x, centre.y + radius + 1,
            centre.x - radius, centre.y + radius/2 + 1 );

    
}

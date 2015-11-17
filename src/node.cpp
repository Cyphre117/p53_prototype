#include "node.h"

Node::Node(int x, int y, int r) :
centre(SDL_Point{x,y}),
radius(r),
texture(nullptr)
{
}

Node::~Node()
{
    if( texture )
        SDL_DestroyTexture(texture);
}

bool Node::isOverCircle(int x, int y)
{
    int dx = centre.x - x;
    int dy = centre.y - y;

    // true if the radius greater than the distance to the point
    return (radius*radius) > (dx*dx + dy*dy);
}

void Node::RenderOutline()
{
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
    SDL_RenderDrawLine( ren, centre.x + radius + 0, centre.y + radius/2 + 0,
            centre.x + 0, centre.y + radius + 0 );
    // bottom left
    SDL_RenderDrawLine( ren, centre.x, centre.y + radius + 1,
            centre.x - radius, centre.y + radius/2 + 1 );
}

void Node::RenderTexture()
{
    if( texture != nullptr )
    {
        SDL_Rect dest{centre.x - radius, centre.y - radius, radius * 2, radius * 2};
        SDL_RenderCopy( ren, texture, NULL, &dest );
    }
}

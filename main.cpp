#include <iostream>
#include "setup.h"
#include "node.h"

int main()
{
    setup();
    
    while( !done() )
    {
        // draw a point at the cursor position
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderDrawPoint(ren, mouse.x, mouse.y);

        renderNodes();

        display();
    }

    SDL_Delay(1000);

    cleanup();
    return 0;
}

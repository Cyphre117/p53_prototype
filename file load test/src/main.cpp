#include <iostream>
#include "setup.h"
#include "node.h"

// removed SDLs redefiniton of main when not necessary
#if defined(SDL_MAIN_AVAILABLE)
	#undef main
#endif

int main(int argc, char *argv[])
{
    setup();
    
    while( !done() )
    {
        if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE] )
        {
            Menu();
			renderCursor(mouse.x, mouse.y);
        }
        else
        {
            updateNodes();
			renderNodes();
			renderCursor(mouse.x + viewport.x, mouse.y + viewport.y);
        }
        
        display();
    }
    
    cleanup();
    return 0;
}

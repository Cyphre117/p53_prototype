#include <iostream>
#include "setup.h"
#include "node.h"

int main()
{
    setup();
    
    while( !done() )
    {
        if( SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE] )
        {
            Menu();
        }
        else
        {
            updateNodes();
            renderNodes();
        }

        renderCursor();        
        display();
    }
    
    cleanup();
    return 0;
}

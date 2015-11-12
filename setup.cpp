#include "setup.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

/* accesible by main */
SDL_Renderer* ren;
SDL_Window* win;
Mouse mouse;
Node* nodes[GRID_WIDTH * GRID_HEGIHT];

/* limited to setup */
SDL_Event event;
void initNodes();
void cleanupNodes();
std::string getProjectPath();
std::vector<std::string> getImageNames(std::string ProjectPath);

void setup()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    win = SDL_CreateWindow("P53 Prototype",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_ShowCursor(SDL_DISABLE);

    initNodes();

    getImageNames( getProjectPath() );
}

bool done()
{
    while( SDL_PollEvent(&event))
    {
        if( event.type == SDL_QUIT) return true;
        if( event.type == SDL_KEYDOWN )
        {
            if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) return true;
        }
        mouse.Update();
    }

    return false;
}

void display()
{
    SDL_RenderPresent(ren);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderClear(ren);
}

void cleanup()
{
    cleanupNodes();

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void initNodes()
{
    for(int y = 0; y < GRID_HEGIHT; y++)
    for(int x = 0; x < GRID_WIDTH; x++)
    {
        nodes[y * GRID_WIDTH + x] = new Node(x * RADIUS * 2 + ((y%2)?RADIUS:RADIUS*2), 
                y * RADIUS * 1.5 + RADIUS,
                RADIUS );
    }
}

void cleanupNodes()
{
    for(int i = 0; i < GRID_WIDTH * GRID_HEGIHT; i++)
    {
        delete nodes[i];
    }
}

void renderNodes()
{
    for(int i = 0; i < GRID_WIDTH * GRID_HEGIHT; i++)
    {
        nodes[i]->Render();
    }
}

std::string getProjectPath()
{
    char* charPath = SDL_GetBasePath();
    
    std::string strPath = charPath;
    
    SDL_free(charPath);

    return strPath;
}

std::vector<std::string> getImageNames(std::string projectPath)
{
    std::vector<std::string> imageNames;

    std::ifstream file(projectPath + "images.txt");

    while( file )
    {
        std::string line;

        std::getline( file, line );

        std::cout << line << std::endl;
    }

    return imageNames;
}

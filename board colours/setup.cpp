#include "setup.h"
#include <string>
#include <iostream>
#include <fstream>
#include <SDL2/SDL_image.h>

/* accesible by main */
SDL_Renderer* ren;
SDL_Window* win;
Mouse mouse;
Node* nodes[GRID_WIDTH * GRID_HEGIHT];
SDL_Texture* currentTexture;

/* limited to setup */
SDL_Event event;void initNodes();
void cleanupNodes();
std::vector<std::string> getImageNames();
void loadImages( std::vector<std::string> imageNames );
std::vector<Node*> textures;
void initMenu();

void setup()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    win = SDL_CreateWindow("P53 Prototype",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN);

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_ShowCursor(SDL_DISABLE);

    initNodes();

    initMenu();

    loadImages( getImageNames() );
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

    IMG_Quit();
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
        nodes[i]->RenderOutline();
    }
    for(int i = 0; i < GRID_WIDTH * GRID_HEGIHT; i++)
    {
        nodes[i]->RenderTexture();
    }    
    for(int i = 0; i < GRID_WIDTH * GRID_HEGIHT; i++)
    {
        if( nodes[i]->isOverCircle(mouse.x, mouse.y) )
        {
            nodes[i]->colour = 0xff00ffff;
            nodes[i]->RenderOutline();
            nodes[i]->colour = 0xffffffff;
        }
    }

}

void updateNodes()
{
    for( int i = 0; i < GRID_WIDTH * GRID_HEGIHT; i++ )
    {
        if( nodes[i]->isOverCircle( mouse.x, mouse.y ) )
        {
            if( mouse.down )
            {
                nodes[i]->texture = currentTexture;
            }
        }
    }
}

void initMenu()
{
    currentTexture = nullptr;
    textures.push_back( new Node(RADIUS, RADIUS, RADIUS) );
}

void Menu()
{
    for( auto& node : textures )
    {
        // display the textures
        node->RenderTexture();

        // highlight the currently over texture
        if( node->isOverCircle(mouse.x, mouse.y) )
        {
            node->RenderOutline();
            
            // set the current texture to that texture if pressed
            if( mouse.down )
            {
                currentTexture = node->texture;
            }
        }

    }
}

void renderCursor()
{
    SDL_SetRenderDrawColor(ren, 255, 0, 255, 255);
    SDL_RenderDrawLine(ren, mouse.x - 4, mouse.y, mouse.x + 4, mouse.y);
    SDL_RenderDrawLine(ren, mouse.x, mouse.y - 4, mouse.x, mouse.y + 4);
}

std::vector<std::string> getImageNames()
{
    std::vector<std::string> imageNames;
    std::string projectPath;
    
    char* charPath = SDL_GetBasePath();    
    projectPath = charPath;    
    SDL_free(charPath);

    std::ifstream file(projectPath + "images.txt");

    std::cout << "images.txt contains:" << std::endl;
    while( file )
    {
        std::string name;

        // whitespace seperated filenames
        file >> name;
        
        // ignore if string is empty
        if( name.empty() ) continue;

        std::cout << "\t" << name << std::endl;

        imageNames.push_back( name );
    }

    return imageNames;
}

void loadImages( std::vector<std::string> imageNames )
{
    int x = RADIUS;
    int y = RADIUS;
    for( int i = 0; i < (int)imageNames.size(); i++ )
    {
        std::string name = imageNames[i];

        std::cout << "loading " << name << "... ";

        // attempt to load the iamge
        SDL_Surface* bmp = IMG_Load(name.c_str());
        
        // if there was an error loading the file let the user know and move on
        if (bmp == nullptr) {
            std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
            continue;
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface( ren, bmp );
        if( tex == nullptr )
        {
            std::cout << "SDL_CreatTexturFromSurface Error: " << SDL_GetError() << std::endl;
            continue;
        }
        
        // tell the texture to use alpha
        SDL_SetTextureBlendMode( tex, SDL_BLENDMODE_BLEND );

        SDL_FreeSurface( bmp );

        x += RADIUS * 2;

        if( x > SCREEN_WIDTH )
        {
            x = RADIUS;
            y += RADIUS * 2;
        }

        textures.push_back( new Node(x,y,RADIUS) );
        textures.back()->texture = tex;

        std::cout << "Success!" << std::endl;
    }
}

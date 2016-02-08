#include "setup.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <SDL2/SDL_image.h>

/* accesible by main */
SDL_Renderer* ren;
SDL_Window* win;
Mouse mouse;
Node* nodes[GRID_WIDTH * GRID_HEGIHT];
SDL_Texture* currentTexture;

SDL_Texture* renderTarget;
SDL_Rect viewport{ BOARD_WIDTH / 2 - SCREEN_WIDTH / 2, BOARD_HEIGHT / 2 - SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT };
float viewScale = 1.0f;

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

	// render everything to the render target and copy the relevant section into the window for scaling / zooming
	renderTarget = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, BOARD_WIDTH, BOARD_HEIGHT);
	SDL_SetRenderTarget(ren, renderTarget);

    SDL_ShowCursor(SDL_DISABLE);

    initNodes();

    initMenu();

    loadImages( getImageNames() );
}

bool done()
{
	bool left = false, right = false, up = false, down = false;

    while( SDL_PollEvent(&event))
    {
        if( event.type == SDL_QUIT) return true;
        if( event.type == SDL_KEYDOWN )
        {
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) return true;
        }
		if (event.type == SDL_MOUSEWHEEL) {
			//zoom in and out
			if (event.wheel.y > 0) {
				viewScale += 0.05f;
			} else if (event.wheel.y < 0) {
				viewScale -= 0.05f;
			}
			std::cout << viewScale << std::endl;
		}

		/*
		if (event.type == SDL_MOUSEMOTION) {
			if (event.motion.x != SCREEN_WIDTH / 2 && event.motion.y != SCREEN_HEIGHT / 2)
			{
				mouse.x += std::ceil(event.motion.xrel * ((viewScale * 2 < 1.0f)?viewScale * 2:1.0f) );
				mouse.y += std::ceil(event.motion.yrel * ((viewScale * 2 < 1.0f)?viewScale * 2:1.0f) );
				SDL_WarpMouseInWindow(win, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

				// move the viewport while holding the right mouse
				if (mouse.altDown) {
					viewport.x -= std::ceil(event.motion.xrel * (viewScale));
					viewport.y -= std::ceil(event.motion.yrel * (viewScale));
				}
			}
		}*/
    }

	int mx, my;
	SDL_GetMouseState(&mx, &my);
	if (mx != SCREEN_WIDTH / 2 || my != SCREEN_HEIGHT / 2)
	{
		SDL_WarpMouseInWindow(win, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
		int xrel = mx - SCREEN_WIDTH / 2;
		int yrel = my - SCREEN_HEIGHT / 2;

		mouse.x += std::ceil(xrel * ((viewScale * 2 < 1.0f) ? viewScale * 2 : 1.0f));
		mouse.y += std::ceil(yrel * ((viewScale * 2 < 1.0f) ? viewScale * 2 : 1.0f));

		if (mouse.altDown) {
			viewport.x -= xrel;
			viewport.y -= yrel;
		}
	}


    mouse.Update();

	const Uint8* keyboard = SDL_GetKeyboardState(NULL);

	// constrin mouse
	if (mouse.x < 0) mouse.x = 0;
	if (mouse.y < 0) mouse.y = 0;
	if (mouse.x > viewport.w) mouse.x = viewport.w;
	if (mouse.y > viewport.h) mouse.y = viewport.h;

	// constain view scale
	if (viewScale < 0.1f) viewScale = 0.1f;
	if (viewScale > BOARD_WIDTH / (float)SCREEN_WIDTH) viewScale = BOARD_WIDTH / (float)SCREEN_WIDTH;
	viewport.w = SCREEN_WIDTH * viewScale;
	viewport.h = SCREEN_HEIGHT * viewScale;

	// constrain viewport position to window
	if (viewport.x < 0) viewport.x = 0;
	if (viewport.y < 0) viewport.y = 0;
	if (viewport.x > BOARD_WIDTH - viewport.w) viewport.x = BOARD_WIDTH - viewport.w;
	if (viewport.y > BOARD_HEIGHT - viewport.y) viewport.y = BOARD_HEIGHT - viewport.y;

    return false;
}

void display()
{
	//if your in the menu just use the full screen
	SDL_Rect fullscreen{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	// copy the big texture to the renderer
	SDL_SetRenderTarget(ren, NULL);

	if (SDL_GetKeyboardState(NULL)[SDL_SCANCODE_SPACE])
		SDL_RenderCopy(ren, renderTarget, &fullscreen, NULL);
	else
		SDL_RenderCopy(ren, renderTarget, &viewport, NULL);
    
	SDL_RenderPresent(ren);
	SDL_SetRenderTarget(ren, renderTarget);

	// clear the render target
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
	SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
    for(int i = 0; i < GRID_WIDTH * GRID_HEGIHT; i++)
    {
        //nodes[i]->RenderOutline();
    }
    for(int i = 0; i < GRID_WIDTH * GRID_HEGIHT; i++)
    {
        nodes[i]->RenderTexture();
    }    
    for(int i = 0; i < GRID_WIDTH * GRID_HEGIHT; i++)
    {
        if( nodes[i]->isOverCircle(mouse.x + viewport.x, mouse.y + viewport.y) )
		{
			SDL_SetRenderDrawColor(ren, 255, 0, 255, 255);
            nodes[i]->RenderOutline();
        }
    }
}

void updateNodes()
{
    for( int i = 0; i < GRID_WIDTH * GRID_HEGIHT; i++ )
    {
        if( nodes[i]->isOverCircle( mouse.x + viewport.x, mouse.y + viewport.y) )
        {
			if( mouse.pressed == true && nodes[i]->texture == currentTexture )
			{
				// cycle cell types
				switch (nodes[i]->type)
				{
				case Small: nodes[i]->type = Medium; break;
				case Medium: nodes[i]->type = Large; break;
				default: nodes[i]->type = Small; break;
				}
			}
            if( mouse.down )
            {
                nodes[i]->texture = currentTexture;

				// for the cell to normal if it's empty
				if (currentTexture == nullptr)
					nodes[i]->type = Small;
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
			SDL_SetRenderDrawColor(ren, 255, 0, 255, 255);
            node->RenderOutline();
            
            // set the current texture to that texture if pressed
            if( mouse.down )
            {
                currentTexture = node->texture;
            }
        }
    }
}

void renderCursor(int x, int y)
{
    SDL_SetRenderDrawColor(ren, 255, 0, 255, 255);
    SDL_RenderDrawLine(ren, x - 4, y, x + 4, y);
    SDL_RenderDrawLine(ren, x, y - 4, x, y + 4);
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

		// assume file paths contain forward slashes
#if defined(__WIN32__)
		// if on windows replace '/' with '\'
		for (auto& c : name)
		{
			if (c == '/')
				c = '\\';
		}
#endif

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

		// create a default texture and 3 for tints
		SDL_Texture* tex  = SDL_CreateTextureFromSurface(ren, bmp);
		SDL_Texture* texR = SDL_CreateTextureFromSurface(ren, bmp);
		SDL_Texture* texG = SDL_CreateTextureFromSurface(ren, bmp);
		SDL_Texture* texB = SDL_CreateTextureFromSurface(ren, bmp);
		SDL_FreeSurface(bmp);

		// if any of the textures failed report an error
		if (tex == nullptr || texR == nullptr || texG == nullptr || texB == nullptr)
        {
            std::cout << "SDL_CreatTextureFromSurface Error: " << SDL_GetError() << std::endl;
            continue;
        }
        
        // tell the texture to use alpha
		SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(texR, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(texG, SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(texB, SDL_BLENDMODE_BLEND);


		// Tint the textures
		SDL_SetTextureColorMod(texR, 255,   0,   0);
		SDL_SetTextureColorMod(texG,   0, 255,   0);
		SDL_SetTextureColorMod(texB,   0,   0, 255);

		// add the 4 new textures to the colour list
		for (int t = 0; t < 4; t++)
		{
			x += RADIUS * 2;
			if (x > SCREEN_WIDTH)
			{
				x = RADIUS;
				y += RADIUS * 2;
			}
			textures.push_back(new Node(x, y, RADIUS));

			switch (t)
			{
			case 0: textures.back()->texture = tex; break;
			case 1: textures.back()->texture = texR; break;
			case 2: textures.back()->texture = texG; break;
			case 3: textures.back()->texture = texB; break;
			default: break;
			}
			
		}
        std::cout << "Success!" << std::endl;
    }
}

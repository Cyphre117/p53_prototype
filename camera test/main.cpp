#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;

GLuint compile_frag_shader( std::string filename );
GLuint compile_vert_shader( std::string filename );

int main(int argc, char* argv[])
{
    // Dont use cmd line ards yet
    for( int i = 0; i < argc; i++ )
        std::cout << argv[i];
    std::cout << std::endl;

    SDL_Init(SDL_INIT_EVERYTHING);
    // Specify the version of OpenGL we want
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

    // Create the window ready for OpenGL rendering
    SDL_Window* window = SDL_CreateWindow("OpenGL", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    // Load OpenGL functions
    glewExperimental = GL_TRUE;
    glewInit();

    // Get openGL version
    std::cout << "OpenGL version " << glGetString( GL_VERSION ) << std::endl;

    // Create vertex buffer object
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat verts[] = {
       -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top left
        1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top right
       -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // bottom left
        1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f  // bottom right
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Create element array
    GLuint ebo;
    glGenBuffers( 1, &ebo );

    GLushort elements[] = {
        0, 1, 2,
        2, 3, 1
    };

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW );

    // Create texture
    GLuint textures[2];
    glGenTextures( 1, textures );

    GLfloat pixels[] = {
        1, 1, 1, 0, 1, 0,
        0, 0, 1, 1, 1, 1
    };

    // Send texture data
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, textures[0] );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels );

    // Load the textue from a file
    {
        SDL_Surface* file = SDL_LoadBMP( "cell.bmp" );
        if( !file ) {
            std::cout << "failed to load image" << std::endl;
            return -1;
        } else {
            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, file->w, file->h, 0, GL_RGB, GL_UNSIGNED_BYTE, file->pixels );
        }
    }

    // Set Texture Params
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    GLuint vertexShader = compile_vert_shader("vertex.glsl");
    if( vertexShader == 0 ) return -1;
    GLuint fragmentShader = compile_frag_shader("fragment.glsl");
    if( fragmentShader == 0 ) return -1;
    
    // Create the shader program, attach the vertex and fragment shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader );
    glBindFragDataLocation( shaderProgram, 0, "outColour" );
    glLinkProgram( shaderProgram );
    glUseProgram( shaderProgram );
    
    // Set the vertex attributes, to align with the vertex array
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLint positionAttrib = glGetAttribLocation( shaderProgram, "position" );
    glEnableVertexAttribArray( positionAttrib );
    glVertexAttribPointer( positionAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), 0 );

    GLint colourAttrib = glGetAttribLocation( shaderProgram, "vColour" );
    glEnableVertexAttribArray( colourAttrib );
    glVertexAttribPointer( colourAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)) );

    GLint texCoordAttrib = glGetAttribLocation( shaderProgram, "vTexCoord" );
    glEnableVertexAttribArray( texCoordAttrib );
    glVertexAttribPointer( texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(5*sizeof(GLfloat)) );

    // Set uniforms
    GLint tint = glGetUniformLocation( shaderProgram, "tint" );
    glUniform4f( tint, 1.0f, 1.0f, 0.0f, 1.0f );
    GLint line = glGetUniformLocation( shaderProgram, "line" );
    glUniform4f( line, 1.0f, 1.0f, 0.0f, 1.0f );

    // Ensure the element array buffer is bound
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );

    SDL_Event event;
    bool quit = false;
    while( !quit ) // START OF MAIN LOOP
    {
        // Handle window events
        while( SDL_PollEvent( &event ) ) {
            if( event.type == SDL_QUIT ) quit = true;
            if( event.type == SDL_KEYDOWN ) {
                if( event.key.keysym.scancode == SDL_SCANCODE_ESCAPE ) quit = true;
            }
        }
        // Clear the window
        glClearColor( 0.2f, 0.1f, 0.2f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );
        
        int mx, my;
        SDL_GetMouseState( &mx, &my );

        float green = mx / (float)SCREEN_WIDTH;
        float blue = my / (float)SCREEN_HEIGHT;
        float brightness = sin(SDL_GetTicks() / 1000.0f);

        glUniform4f( tint, 0.0, green, blue, 1.0);

        glUniform4f( line, brightness, brightness, brightness, 1.0);

        //glDrawArrays( GL_TRIANGLES, 1, 3 );
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
        
        // Show the graphics
        SDL_GL_SwapWindow( window );
    }
    
    // Cleanup
    glDeleteProgram( shaderProgram );
    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    glDeleteBuffers( 1, &ebo );
    glDeleteBuffers( 1, &vbo );
    glDeleteVertexArrays( 1, &vao );

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}

std::string load_file( std::string filename )
{
    std::ifstream file( filename );
    if( file.good() )
        std::cout << "Loaded " << filename << std::endl;
    else 
        std::cout << "Could not load " << filename << "!" << std::endl;

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

GLuint compile_vert_shader( std::string filename )
{
    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    
    // Load the shader to a string and convert to const GLchar*
    std::string source = load_file( filename );
    const GLchar* source_ptr = (const GLchar*)source.c_str();
    
    glShaderSource(vertexShader, 1, &source_ptr, NULL);
    glCompileShader(vertexShader);
    
    // Check the shader was compiled correctly
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if( status != GL_TRUE )
    {
        // There was an error compiling the shader
        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        std::cout << "Vertex Shader " << buffer << std::endl;
        return 0;
    }
    else std::cout << "Compiled vertex Shader" << std::endl;

    return vertexShader;
}

GLuint compile_frag_shader( std::string filename )
{
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Load the shader to a string and convert to const GLchar*
    std::string source = load_file( filename );
    const GLchar* source_ptr = (const GLchar*)source.c_str();
    
    glShaderSource(fragmentShader, 1, &source_ptr , NULL);
    glCompileShader(fragmentShader);

    // Check the shader was compiled succesfully
    GLint status;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if( status != GL_TRUE )
    {
        char buffer[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
        std::cout << "Fragment Shader " << buffer << std::endl;
        return 0;
    }
    else std::cout << "Compiled fragment Shader" << std::endl;

    return fragmentShader;
}

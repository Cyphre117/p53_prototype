#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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
    
    // Specify matricies
    glm::mat4 model_matix;
    glm::mat4 view_matrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 1.0f), // Position
        glm::vec3(0.0f, 0.0f, 0.0f), // Look at
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
    );
    //                                          verticle FOV, Aspect Ratio, Near plane, far plane
    glm::mat4 proj_matrix = glm::perspective( glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f );

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

    GLint model_uni = glGetUniformLocation( shaderProgram, "model" );
    glUniformMatrix4fv( model_uni, 1, GL_FALSE, glm::value_ptr( model_matix ) );

    GLint view_uni = glGetUniformLocation( shaderProgram, "view" );
    glUniformMatrix4fv( view_uni, 1, GL_FALSE, glm::value_ptr( view_matrix ) );

    GLint proj_uni = glGetUniformLocation( shaderProgram, "projection" );
    glUniformMatrix4fv( proj_uni, 1, GL_FALSE, glm::value_ptr( proj_matrix ) );

    // Ensure the element array buffer is bound
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ebo );

    SDL_Event event;
    bool quit = false;
    const Uint8* keys;
    int mx, my;
    Uint32 time = 0, oldtime = SDL_GetTicks();
    float dt = 0.0f;
    while( !quit ) // START OF MAIN LOOP
    {
        // Handle window events
        while( SDL_PollEvent( &event ) ) {
            if( event.type == SDL_QUIT ) quit = true;
            if( event.type == SDL_KEYDOWN ) {
                if( event.key.keysym.scancode == SDL_SCANCODE_ESCAPE ) quit = true;
            }
        }
        // Get input
        keys = SDL_GetKeyboardState(NULL);
        SDL_GetMouseState( &mx, &my );

        time = SDL_GetTicks();
        dt = (time - oldtime) / 1000.0f;
        oldtime = time;

        // Clear the window
        glClearColor( 0.2f, 0.1f, 0.2f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );
        
        float green = mx / (float)SCREEN_WIDTH;
        float blue = my / (float)SCREEN_HEIGHT;
        float brightness = sin(SDL_GetTicks() / 1000.0f);

        glUniform4f( tint, 0.0, green, blue, 1.0);
        glUniform4f( line, brightness, brightness, brightness, 1.0);

        if( keys[SDL_SCANCODE_RIGHT] ) model_matix = glm::translate( model_matix, glm::vec3( 1.f * dt, 0.0f, 0.0f) );
        if( keys[SDL_SCANCODE_LEFT ] ) model_matix = glm::translate( model_matix, glm::vec3(-1.f * dt, 0.0f, 0.0f) );

        // Send Matricies
        glUniformMatrix4fv( model_uni, 1, GL_FALSE, glm::value_ptr( model_matix ) );
        glUniformMatrix4fv( view_uni, 1, GL_FALSE, glm::value_ptr( view_matrix ) );
        glUniformMatrix4fv( proj_uni, 1, GL_FALSE, glm::value_ptr( proj_matrix ) );

        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );    // Render geometry
        SDL_GL_SwapWindow( window );                                // Show the graphics
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

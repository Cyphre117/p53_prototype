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

// Screen Resolution
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Specify matricies
glm::mat4 model_matix;
glm::vec3 position = glm::vec3( 0.0f, 0.0f, 1.0f );
glm::mat4 view_matrix = glm::lookAt(
    position, // Position
    glm::vec3(0.0f, 0.0f, 0.0f), // Look at
    glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
);
///////////////////////////////////////// verticle FOV, Aspect Ratio, Near plane, far plane
glm::mat4 proj_matrix = glm::perspective( glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f );

void move( glm::vec3 mv );
void update_cursor( GLint vbo, float x, float y);
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
    GLuint vbo[2];
    glGenBuffers(2, &vbo[0]);

    GLfloat cursor[] = {
        // cursor triangle
        0.0f,  0.1f, 1.0f, 1.0f, 1.0f, 0.5f, 0.0f, // top
       -0.1f, -0.1f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, // bottom left
        0.1f, -0.1f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f  // bottom right
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cursor), cursor, GL_DYNAMIC_DRAW);

    GLfloat verts[] = {
        // quad
       -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top left
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top right
       -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // bottom left
        0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // bottom right
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // Create element array
    GLuint ebo;
    glGenBuffers( 1, &ebo );

    GLushort elements[] = {
        0, 1, 2,
        2, 3, 1,
        4, 6, 5
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
    GLuint vao[2];
    glGenVertexArrays(2, &vao[0]);

    for( int i = 0; i < 2; i++ ) {
        
        glBindBuffer( GL_ARRAY_BUFFER, vbo[i] );
        glBindVertexArray(vao[i]);

        GLint positionAttrib = glGetAttribLocation( shaderProgram, "position" );
        glEnableVertexAttribArray( positionAttrib );
        glVertexAttribPointer( positionAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), 0 );

        GLint colourAttrib = glGetAttribLocation( shaderProgram, "vColour" );
        glEnableVertexAttribArray( colourAttrib );
        glVertexAttribPointer( colourAttrib, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(2*sizeof(GLfloat)) );

        GLint texCoordAttrib = glGetAttribLocation( shaderProgram, "vTexCoord" );
        glEnableVertexAttribArray( texCoordAttrib );
        glVertexAttribPointer( texCoordAttrib, 2, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*)(5*sizeof(GLfloat)) );
    }

    // Set uniforms
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
            if( event.type == SDL_MOUSEWHEEL ) {
                if( event.wheel.y > 0 ) move( glm::vec3( 0.0f, 0.0f, 0.1f ) );
                else if( event.wheel.y < 0 ) move( glm::vec3( 0.0f, 0.0f, -0.1f ) );
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
        
        // Move the camera
        if( keys[SDL_SCANCODE_RIGHT] ) move( glm::vec3( 1.0f * dt, 0.0f, 0.0f ) );
        if( keys[SDL_SCANCODE_LEFT ] ) move( glm::vec3(-1.0f * dt, 0.0f, 0.0f ) );
        if( keys[SDL_SCANCODE_DOWN ] ) move( glm::vec3( 0.0f,-1.0f * dt, 0.0f ) );
        if( keys[SDL_SCANCODE_UP   ] ) move( glm::vec3( 0.0f, 1.0f * dt, 0.0f ) );

        // Send Matricies
        glUniformMatrix4fv( model_uni, 1, GL_FALSE, glm::value_ptr( model_matix ) );
        glUniformMatrix4fv( view_uni, 1, GL_FALSE, glm::value_ptr( view_matrix ) );
        glUniformMatrix4fv( proj_uni, 1, GL_FALSE, glm::value_ptr( proj_matrix ) );
        
        //if( keys[SDL_SCANCODE_SPACE] )
            update_cursor( vbo[0], (mx / (float)SCREEN_WIDTH) * 2.0f - 1.0f, (my / (float)SCREEN_HEIGHT) * 2.0f - 1.0f );
        glBindVertexArray( vao[1] );
        
        // Render geometry
        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );
        glBindVertexArray( vao[0] );
        glDrawArrays( GL_TRIANGLES, 0, 3 );

        SDL_GL_SwapWindow( window );                               // Show the graphics
    }
    
    // Cleanup
    glDeleteProgram( shaderProgram );
    glDeleteShader( vertexShader );
    glDeleteShader( fragmentShader );

    glDeleteBuffers( 1, &ebo );
    glDeleteBuffers( 2, &vbo[2] );
    glDeleteVertexArrays( 2, &vao[0] );

    SDL_GL_DeleteContext(context);
    SDL_Quit();
    return 0;
}

void update_cursor( GLint vbo, float x, float y )
{
    // input: x and y in range -1:1, origin in the top left
    // invert Y to match openGL
    y = -y;
    // x and y are now in clip space

    glm::mat4 viewProjInv = glm::inverse( proj_matrix * view_matrix );
    
    glm::vec4 point3d( x, y, 0.0f, 1.0f );

    point3d = viewProjInv * point3d;

    point3d.x = point3d.x / point3d.w;
    point3d.y = point3d.y / point3d.w;
    point3d.z = point3d.z / point3d.w;

    //std::cout << "Point: " << point3d.x << " " << point3d.y << " " << point3d.z << " " << point3d.w <<
    //    " \tCam: " << position.x << " " << position.y << " " << position.z << std::endl;

    // experimental?!
    point3d.x = position.x + (point3d.x - position.x) * point3d.w * position.z;
    point3d.y = position.y + (point3d.y - position.y) * point3d.w * position.z;

    x = point3d.x;
    y = point3d.y;
    
    // output: x and y in world coordinates
    GLfloat verts[] = {
        // cursor triangle
        0.0f + x,  0.1f + y, 1.0f, 1.0f, 1.0f, 0.5f, 0.0f, // top
       -0.1f + x, -0.1f + y, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, // bottom left
        0.1f + x, -0.1f + y, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f  // bottom right
    };

    // GET THIS TO DRAW CURSOR
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(verts), verts );
}

void move( glm::vec3 mv )
{

    position += mv;
    if( position.z <  0.2f ) position.z = 0.2f;
    if( position.z > 10.0f ) position.z = 10.0f;

    view_matrix = glm::lookAt(
        position,                                             // Position
        glm::vec3(position.x, position.y, position.z - 1.0f), // Look at
        glm::vec3(0.0f, 1.0f, 0.0f)                           // Up vector
    );
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

#include "ECApp.h"

//-------------------------------------------------------------------------------------------------
/**
 * Shows a prompt error window containing a message.
 */
void
ecsyst::LogError( std::string error_message = "An unknown error occurred" )
{
#ifdef _WINDOWS
    MessageBox( NULL, error_message.c_str( ), "EightChip: ERROR OCCURED", MB_OK | MB_ICONERROR );
#else
    std::cerr << error_message << std::endl;
#endif
}

//-------------------------------------------------------------------------------------------------
/**
 * The "settings.ini" file has on each line an alias for a rom its path and indicates, and the
 * number of opcodes to execute per second while playing it.
 *
 * The file format is as follows:
 * RomName: PATH_TO_ROM_FILE, OPCODES_PER_SECOND!
 **/
bool
ecemulate::LoadSettings( SETTINGS_MAP& settings )
{
    std::string line;

    std::ifstream fileStream;
    fileStream.open( "settings.ini" );

    if ( fileStream.is_open( ) )
    {
        std::string key, value;

        while ( getline( fileStream, line ) )
        {
            auto delimiter_pos = line.find( ":" );
            key = line.substr( 0, delimiter_pos );
            value = line.substr( delimiter_pos + 1 );

            // check for errors
            if ( key.empty( ) || value.empty( ) )
            {
                ecsyst::LogError( ERR08 );
                fileStream.close( );
                return false;
            }

            // add to settings map
            settings.insert( std::make_pair( key, value ) );
        }
    }
    else
    {
        ecsyst::LogError( ERR07 );
        return false;
    }

    fileStream.close( );

    if ( settings.empty( ) )
    {
        ecsyst::LogError( ERR09 );
        return false;
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
bool
ecemulate::LoadRom( EightChipCPU* cpu, const SETTINGS_MAP& settings )
{
    bool res = false;

    SETTINGS_MAP::const_iterator it = settings.find( ROM_NAME );

    // If the rom isn't found, the map's iterator is a the end element.
    if ( settings.end( ) == it )
    {
        ecsyst::LogError( ERR01 );
        return res;
    }

    ecsyst::LogError( "INFO: Loading " + (*it).second + " .. ");

    // Load the rom into memory
    res = cpu->InitRom( ( *it ).second );

    return res;
}
//-------------------------------------------------------------------------------------------------
void
ecgfx::DrawGraphics( EightChipCPU* cpu, SDL_Window* window )
{
    // Create an OpenGL context associated with the window
    SDL_GLContext glcontext = SDL_GL_CreateContext( window );

    // Clear the color and depth buffers to ensure that the previous frame
    // is not drawn on top of the current one
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Load the identity matrix to reset the current matrix
    glLoadIdentity( );

    // Set the raster position to the top-left corner of the window
    glRasterPos2i( -1, 1 );

    // Set the pixel zoom to 1
    glPixelZoom( 1, -1 );

    // Draw the pixel data stored in the m_ScreenPixels member of the EightChipCPU object
    // on the window
    glDrawPixels( WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, cpu->m_ScreenPixels );

    // Swap the window's back buffer with the front buffer to make the
    // drawn graphics visible on the screen
    SDL_GL_SwapWindow( window );

    // Flush the OpenGL pipeline
    glFlush( );

    // Delete the OpenGL context associated with the window to free up
    // any resources associated with the context
    SDL_GL_DeleteContext( glcontext );
}
//-------------------------------------------------------------------------------------------------
void
ecgfx::InitOpenGL( SDL_Window* window )
{
    // Create an OpenGL context associated with the window
    SDL_GLContext glcontext = SDL_GL_CreateContext( window );

    // Set the viewport to the size of the window
    glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );

    // Set the projection matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    glOrtho( 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1.0, 1.0 );

    // Clear the screen to black
    glClearColor( 0, 0, 0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Set the shading model
    glShadeModel( GL_FLAT );

    // Swap buffers to display the scene
    SDL_GL_SwapWindow( window );

    // Enable texturing
    glEnable( GL_TEXTURE_2D );

    // Disable depth testing, culling, dithering, and blending
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_CULL_FACE );
    glDisable( GL_DITHER );
    glDisable( GL_BLEND );

    // Delete the OpenGL context
    SDL_GL_DeleteContext( glcontext );
}

//-------------------------------------------------------------------------------------------------
bool
ecgfx::InitGraphics( SDL_Window* window )
{
    // Initialise the SDL library
    if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        ecsyst::LogError( ERR05 );
        return false;
    }

    // Create a window
    window =  SDL_CreateWindow( WINDOW_CAPTION, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL );
    if ( window == nullptr )
    {
        ecsyst::LogError( ERR06 );
        return false;
    }

    // Initialise OpenGL
    InitOpenGL( window );

    return true;
}
//-------------------------------------------------------------------------------------------------
void
ecemulate::SetupInput( EightChipCPU* cpu, SDL_Event event )
{
    int key = -1;

    if ( event.type == SDL_KEYDOWN )
    {
        switch ( event.key.keysym.sym )
        {
        case SDLK_q:
            key = 0x0;
            break;
        case SDLK_s:
            key = 0x1;
            break;
        case SDLK_d:
            key = 0x2;
            break;
        case SDLK_f:
            key = 0x3;
            break;
        case SDLK_w:
            key = 0x4;
            break;
        case SDLK_x:
            key = 0x5;
            break;
        case SDLK_c:
            key = 0x6;
            break;
        case SDLK_v:
            key = 0x7;
            break;
        case SDLK_a:
            key = 0x8;
            break;
        case SDLK_z:
            key = 0x9;
            break;
        case SDLK_e:
            key = 0xA;
            break;
        case SDLK_r:
            key = 0xB;
            break;
        case SDLK_u:
            key = 0xC;
            break;
        case SDLK_i:
            key = 0xD;
            break;
        case SDLK_o:
            key = 0xE;
            break;
        case SDLK_p:
            key = 0xF;
            break;

        default:
            break;
        }
        if ( key != -1 )
        {
            cpu->KeyDown( key );
        }
    }
    else if ( event.type == SDL_KEYUP )
    {
        key = -1;
        switch ( event.key.keysym.sym )
        {
        case SDLK_q:
            key = 0x0;
            break;
        case SDLK_s:
            key = 0x1;
            break;
        case SDLK_d:
            key = 0x2;
            break;
        case SDLK_f:
            key = 0x3;
            break;
        case SDLK_w:
            key = 0x4;
            break;
        case SDLK_x:
            key = 0x5;
            break;
        case SDLK_c:
            key = 0x6;
            break;
        case SDLK_v:
            key = 0x7;
            break;
        case SDLK_a:
            key = 0x8;
            break;
        case SDLK_z:
            key = 0x9;
            break;
        case SDLK_e:
            key = 0xA;
            break;
        case SDLK_r:
            key = 0xB;
            break;
        case SDLK_u:
            key = 0xC;
            break;
        case SDLK_i:
            key = 0xD;
            break;
        case SDLK_o:
            key = 0xE;
            break;
        case SDLK_p:
            key = 0xF;
            break;

        default:
            break;
        }
        if ( key != -1 )
        {
            cpu->KeyUp( key );
        }
    }
}
//-------------------------------------------------------------------------------------------------
void
ecemulate::EmulateCycle( EightChipCPU* cpu, const SETTINGS_MAP& settings, bool& status, SDL_Window* window )
{
    status = true;

    SETTINGS_MAP::const_iterator it = settings.find( "OpcodesPerSecond" );

    // Check whether the rom settings are indeed in file settings.
    if ( settings.end( ) == it )
    {
        ecsyst::LogError( ERR00 );
        return;
    }

    int frameskip = 60;

    // number of OpCodes to execute per second
    int numopcodes = atoi( ( *it ).second.c_str( ) );

    // number of OpCodes to execute per frame
    int numframe = numopcodes / frameskip;

    SDL_Event event;
    float interval = 1000;
    interval /= frameskip;

    unsigned int time = SDL_GetTicks( );

    while ( status )
    {
        while ( SDL_PollEvent( &event ) )
        {
            ecemulate::SetupInput( cpu, event );

            if ( event.type == SDL_QUIT )
            {
                status = false;
            }
        }

        unsigned int currentTime = SDL_GetTicks( );

        if ( ( time + interval ) < currentTime )
        {
            cpu->DecreaseTimers( );
            for ( int i = 0; i < numframe; i++ )
                cpu->ExecuteNextOpCode( );

            time = currentTime;
            ecgfx::DrawGraphics( cpu, window );
        }
    }
}

//-------------------------------------------------------------------------------------------------

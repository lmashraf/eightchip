#include "stdafx.h"

#include "ECCpu.h"

/*
 *Shows a prompt error window containing a message.
 */
void
ecsyst::LogMessage( std::string message )
{
#ifdef _WINDOWS
    MessageBox( NULL, message.c_str( ), "EightChip: ERROR OCCURED", MB_OK | MB_ICONERROR );
#else
    std::cerr << message << std::endl;
#endif
}
//-------------------------------------------------------------------------------------------------
/**
 * The "settings.ini" file has on each line an alias for a rom its path and indicates, and the
 * number of opcodes to execute per second while playing it.
 *
 * The file format is as follows:
 * RomName: PATH_TO_ROM_FILE, OPCODES_PER_SECOND!
 */
bool
ecemulate::LoadSettings( SETTINGS_MAP& settings )
{
    const BYTE MAXLINE = 0xFF;
    char line[ MAXLINE ];
    std::string settingName, settingValue;

    std::ifstream file;
    file.open( "settings.ini" );

    if ( !file.is_open( ) )
    {
        ecsyst::LogMessage( ERR007 );
        return false;
    }

    while ( !file.eof( ) )
    {
        memset( line, '\0', sizeof( line ) );

        file.getline( line, MAXLINE );

        // get the setting's name
        char* name = 0;
        name = strtok( line, ":" );
        settingName = name;

        // get the setting's value
        char* value = 0;
        value = strtok( NULL, "!" );
        settingValue = value;

        // check for errors
        if ( value == 0 || name == 0 || settingName.empty( ) || settingValue.empty( ) )
        {
            ecsyst::LogMessage( ERR008 );
            file.close( );
            return false;
        }

        // add to settings map
        settings.insert( std::make_pair( settingName, settingValue ) );
    }

    file.close( );

    if ( settings.empty( ) )
    {
        ecsyst::LogMessage( ERR009 );
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
        ecsyst::LogMessage( ERR001 );
        return res;
    }

    // Load the rom into memory
    res = cpu->InitRom( ( *it ).second );

    return res;
}
//-------------------------------------------------------------------------------------------------
void
ecgfx::DrawGraphics( EightChipCPU* cpu )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity( );
    glRasterPos2i( -1, 1 );
    glPixelZoom( 1, -1 );

    glDrawPixels( WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, cpu->m_ScreenPixels );

    SDL_GL_SwapBuffers( );

    glFlush( );
}
//-------------------------------------------------------------------------------------------------
void
ecgfx::SetupOpenGL( )
{
    glViewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    glOrtho( 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1.0, 1.0 );
    glClearColor( 0, 0, 0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glShadeModel( GL_FLAT );

    glEnable( GL_TEXTURE_2D );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_CULL_FACE );
    glDisable( GL_DITHER );
    glDisable( GL_BLEND );
}

//-------------------------------------------------------------------------------------------------
bool
ecgfx::SetupSDL( )
{
    if ( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        ecsyst::LogMessage( ERR005 );
        return false;
    }
    if ( SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT, 8, SDL_OPENGL ) == NULL )
    {
        ecsyst::LogMessage( ERR006 );
        return false;
    }

    // OpenGL
    SetupOpenGL( );

    SDL_WM_SetCaption( WINDOW_CAPTION, NULL );

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
ecemulate::EmulateCycle( EightChipCPU* cpu, const SETTINGS_MAP& settings, bool& status )
{
    status = true;

    SETTINGS_MAP::const_iterator it = settings.find( "OpcodesPerSecond" );

    // Check whether the rom settings are indeed in file settings.
    if ( settings.end( ) == it )
    {
        ecsyst::LogMessage( ERR000 );
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
            ecgfx::DrawGraphics( cpu );
        }
    }
}
//-------------------------------------------------------------------------------------------------

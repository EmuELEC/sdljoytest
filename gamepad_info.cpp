/* gamepad_info, display gamepad SDLGamecontrollerDB mappings and information
 * used in EmuELEC, original taken from:
 * 
 * Test gamepad axis/buttons with SDL2
 * Also tests new SDL2 features: game controller, joystick/gamepad, hotplug and haptics/rumble.
 * 
 * (c) Wintermute0110 <wintermute0110@gmail.com> December 2014
 */
#include <SDL2/SDL.h>

SDL_Joystick *joy = NULL;
// Game controllers do not have hats or balls, only joysticks have.
int SDL_joystick_has_hat = 0;

SDL_GameController *gamepad = NULL;
SDL_JoystickID instanceID = -1; // Joystick instance ID. Changes if there are hotplug events!!!
int device_index_in_use = -1; // This is the devic number in use
int SDL_joystick_is_gamepad = 0; // True if joystick is a SDL2 recognised gamepad

int main(int argn, char** argv)
{
    int numJoysticks, i;
    bool moreinfo = false;

    if (argn > 1 && strncmp(argv[1], "-more", 10) == 0) {
      moreinfo = true;
    }
    
    //
    // Joystick initialisation
    // In SDL2, and contrary to what happens in SDL1, joystick events are
    // received even if video is not initialisated.
    // Very interesting. If video is initialised in SDL2 then only button 
    // UP events work! (Tested with a Logitech F710 wireless)!!!
    //
    // printf( "Sys_InitInput: SDL2 joystick subsystem init\n" );
    if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC)) {
        printf( "Sys_InitInput: SDL_Init() failed: %s\n", SDL_GetError());
        return 0;
    }

    //
    // Load controller mappings
    //
		const char* db_file = SDL_getenv("SDL_GAMECONTROLLERCONFIG_FILE");
		if (db_file) {
			if (moreinfo)
            printf("Sys_InitInput: Loading %s\n", db_file);
            int num_devices = SDL_GameControllerAddMappingsFromFile(db_file);
		}

    //
    // Print joystick information at startup time.
    //
    numJoysticks = SDL_NumJoysticks();
    
if (numJoysticks > 0)
{
    for (i = 0; i < numJoysticks; i++) {
		int gamepad_idx_to_open = i;
		
		// Try to open joystick as controller
		gamepad = SDL_GameControllerOpen( gamepad_idx_to_open );
		if( gamepad == NULL ) {
			SDL_joystick_is_gamepad = 0;
			// If fails open joystick as joystick (old SDL joystick API)
			printf("SDL_GameControllerOpen failed: %s\n", SDL_GetError());
			printf("Opening joystick with old SDL joystick API\n");

			joy = SDL_JoystickOpen( gamepad_idx_to_open );
			if (joy == NULL ) {
				printf( "SDL_JoystickOpen failed: %s\n", SDL_GetError() );
				printf( "Couldn't open joystick %i\n", gamepad_idx_to_open );
				joy = NULL;
				instanceID = -1;
				device_index_in_use = -1;
			} else {
				int num_hats;
				char guid[64];

				instanceID = SDL_JoystickInstanceID(joy);
				device_index_in_use = gamepad_idx_to_open;
				SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, sizeof (guid));
				num_hats = SDL_JoystickNumHats( joy );
            if (moreinfo) 
            {
				printf( "Opened joystick device index %i (%s)\n", 
								                device_index_in_use, SDL_JoystickNameForIndex( device_index_in_use ) );
				printf( "        axes: %d\n", SDL_JoystickNumAxes( joy ) );
				printf( "     buttons: %d\n", SDL_JoystickNumButtons( joy ) );
				printf( "        hats: %d\n", num_hats );
				printf( "       balls: %d\n", SDL_JoystickNumBalls( joy ) );
				printf( " instance id: %d\n", SDL_JoystickInstanceID( joy ) );
				printf( "        guid: %s\n", guid);
            } else { 
            printf("%s%%%s\n", SDL_JoystickName( joy ),guid);
            }
				SDL_joystick_has_hat = 0;
				if( num_hats )
					SDL_joystick_has_hat = 1;
			}
		} else {
			SDL_joystick_is_gamepad = 1;
			joy = SDL_GameControllerGetJoystick( gamepad );
			instanceID = SDL_JoystickInstanceID(joy);
			device_index_in_use = gamepad_idx_to_open;

			// Remember that gamepads do not have hats in SDL2
			int num_axes = 0;
			int num_buttons = 0;
			char guid[64];

			SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joy), guid, sizeof (guid));
            if (moreinfo) 
            {
			printf( "Opened gamepad device index %i (%s)\n", 
							             device_index_in_use, SDL_GameControllerNameForIndex( device_index_in_use ) );
			printf( "        axes: %d\n", SDL_JoystickNumAxes( joy ) );
			printf( "        hats: %d\n", SDL_JoystickNumHats( joy ) );
			printf( "     buttons: %d\n", SDL_JoystickNumButtons( joy ) );
			printf( " instance id: %d\n", SDL_JoystickInstanceID( joy ) );
			printf( "        name: %s\n", SDL_JoystickName( joy ) );
			printf( "        guid: %s\n", guid);
			printf( "     mapping: %s\n", SDL_GameControllerMapping( gamepad ));
                } else { 
                    printf("%s\n", SDL_GameControllerMapping( gamepad ));
            }
		}
	} 
} else {
     printf( "No Joystick/Gamepad detected! \n" );
}
    //
    // Shutdown SDL2
    //
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);

    return 0;
}

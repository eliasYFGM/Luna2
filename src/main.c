#include "core.h"
#include "states/gamestate.h"

int main(int argc, char** argv)
{
  // Default configuration that the game will make use of
  struct Game_Config config =
  {
    // Window title
    "Luna",
    // Display resolution
    640, 480,
    // Framerate (FPS)
    30,
    // Want full-screen?
    TRUE,
    // Want audio module?
    TRUE,
    // Use additional back-buffer? (black bars on full-screen)
    TRUE,
    // Argument list
    { argc, argv }
  };

  if (game_init(&config))
  {
    // Run the game with a default "starting state"
    game_run(GAME_STATE);
  }

  return 0;
}

#include "engine.h"
#include "states/game_state.h"

int main(int argc, char **argv)
{
  // Default configuration that the game will make use of
  struct Game_Config config =
  {
    // Argument list
    argc, argv,
    // Window title
    "Luna 2",
    // Display resolution
    640, 480,
    // Framerate (FPS)
    30,
    // Full-screen?
    FALSE,
    // Enable audio module?
    TRUE,
    // Additional back-buffer? (black bars on full-screen)
    TRUE
  };

  if (game_init(&config))
  {
    // Run the game, with a starting state
    game_run(Game_State());
  }

  return 0;
}

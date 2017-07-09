#include "engine.h"
#include "state_game.h"

int main(int argc, char **argv)
{
  struct Engine_Conf conf =
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
      // Additional back-buffer? (black bars on full-screen)
      TRUE
    };

  if (engine_init(&conf))
    {
      // Run with a starting state
      engine_run(STATE_GAME);
    }

  return 0;
}

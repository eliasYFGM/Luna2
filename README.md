# Luna 2
A simple remake of the creepypasta game "Luna 2" or Luna Game 2, written in pure C (as in the C99 standard). It started as a small tribute to the series after 5 years of its first release in 2011.

Gameplay is 99% identical to Luna 2. It has improved movement (physics) and a few bug-fixes too.

## Compilation

### Linux
This is the default target. You need to install Code::Blocks and Allegro 5 development packages in order to compile and build. You can use MinGW32 or MinGW-w64 to target Windows too, but you'll need to build Allegro for this platform.

### Windows
Be sure to have Code::Blocks installed and change the linker options to fit your current configuration. You will need to build Allegro 5 yourself with CMAKE if you don't have the libraries that correspond with your GCC version.

At least Allegro 5.0 is required, with acodec (+ ogg/vorbis support), audio, image, font and primitives.

## Credits

Music - Super Ponybeat

Luna sprites - Desktop Ponies

Graphics (BG, tileset, etc.) - Anonymous

# hex game prototype
## Testing images
* to load images place a file called 'images.txt' next to the executable
* each line of the should contain the name and path to 1 image, relative to the executable
* images can be BMPs or PNGs with transparency

## Compiling
required SDL2 and SDL2_images
`clang++ *.cpp -l SDL2 -l SDL2_images -std=c++11`


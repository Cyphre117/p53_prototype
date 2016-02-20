# hex game prototypes
## View the doxygen generated code at

[cyphre117.github.io/p53_prototype](cyphre117.github.io/p53_prototype)
[Guide to setting up doxygen with gh-pages](http://rickfoosusa.blogspot.co.uk/2011/10/howto-use-doxygen-with-github.html)
[an issue talking about using doxygen with gh-pages](https://github.com/m-a-d-n-e-s-s/madness/issues/104)

## Testing images
* to load images place a file called 'images.txt' next to the executable
* each line of the should contain the name and path to 1 image, relative to the executable
* images can be BMPs or PNGs with transparency

## Compiling
required SDL2 and SDL2_images
`clang++ *.cpp -l SDL2 -l SDL2_images -std=c++11`


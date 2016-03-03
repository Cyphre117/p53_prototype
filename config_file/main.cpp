#include <iostream>
#include "config_file.h"

int main( int argc, char* argv[] )
{

    ConfigFile file;

    file.load("config.txt");

    int i;    
    if( file.getInt("TomIsCool", &i) )
        std::cout << "Found int: " << i << std::endl;
    
    float f;
    if( file.getFloat("FLOAT", &f) )
        std::cout << "Found float: " << f << std::endl;

    return 0;
}
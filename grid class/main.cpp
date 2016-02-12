#include <iostream>
#include "grid.h"

int main()
{
    Grid grid;

    for( int i = 0; i < 8; i++ )
        grid.newCell( 0, i, nullptr );

    std::cout << "Cells on grid: " << grid.cells.size() << std::endl;

    grid.deleteNode( 0, 2 );
    
    std::cout << "After deleting one: " << grid.cells.size() << std::endl;

    grid.deleteNode( 0, 3 );

    std::cout << "After deleting two: " << grid.cells.size() << std::endl;

    grid.newCell( 0, 3, nullptr );
    
    std::cout << "Creating a cell again: " << grid.cells.size() << std::endl;
    
    return 0;
}

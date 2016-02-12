#include <iostream>
#include "grid.h"
#include "cell.h"

int main()
{
    Grid grid;

    for( int i = 0; i < 8; i++ )
        grid.newCell( 0, i, nullptr );

    std::cout << "Cells on grid: " << grid.numCells() << std::endl;

    grid.setEmpty( 0, 2 );    
    std::cout << "After deleting one: " << grid.numCells() << std::endl;

    grid.setEmpty( 0, 3 );
    std::cout << "After deleting two: " << grid.numCells() << std::endl;

    grid.newCell( 0, 3, nullptr );    
    std::cout << "Created a cell again: " << grid.numCells() << std::endl;
    
    for( int i = 0; i < grid.numCells(); i++ )
        grid.getCell(i)->print();

    grid.newBloodVessel( 3, 5, nullptr );
    std::cout << "Blood vessels: " << grid.numBloodVessels() << std::endl;
    //grid.deleteNode( 3, 5 );
    //std::cout << "Blood vessels: " << grid.numBloodVessels() << std::endl;

    // Print the board to the console
    Node* n;
    for( int y = 0; y < CHUNK_WIDTH; y++ )
    {
        for( int x = 0; x < CHUNK_WIDTH; x++ )
        {
            grid.getNode( x, y, &n );

            if( n == nullptr ) std::cout << ". ";
            else if( n->getType() == NodeType::EMPTY ) std::cout << ". ";
            else if( n->getType() == NodeType::CELL ) std::cout << "c ";
            else std::cout << "O ";

        }
        std::cout << std::endl;
        for( int i = 0; i < y + 1; i++ )
            std::cout << " ";
    }

    return 0;
}

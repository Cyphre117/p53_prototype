#include "grid.h"
#include "cell.h"
#include <iostream>

Grid::Grid()
{
    // Initialise the grid to empty
    for( auto& node : grid_ )
        node = nullptr;

    // Not necessary, but may as well reserve the space ahead of time as an optimization
    cells.reserve( CHUNK_WIDTH * CHUNK_WIDTH );
}

Grid::~Grid()
{
    // Cleanup nodes
    for( auto& node : grid_ )
    {
        delete node;
        node = nullptr;
    }
}

// return false if error, input taken by copy, output is pointers instead of reference (&) or return value
bool Grid::getNode( int row, int col, Node** node )
{
    // If the node does not lie on the board, return error
    if( !nodeExists( row, col ) )
        return false;

    // Get the node at that location
    *node = grid_[ row * CHUNK_WIDTH + col ];

    return true;
}

bool Grid::newCell( int row, int col, Cell** createdCell )
{
    Node* current;
    Cell* newCell;

    if( getNode( row, col, &current ) )
    {

        // If there was a node already there, clean it up
        if( current != nullptr )
            delete current;

        // Initialse a new cell
        newCell = new Cell( row, col );

        // Save the new cell to the grid
        grid_[ row * CHUNK_WIDTH + col ] = newCell;

        // And store if in the vector
        cells.push_back( newCell );

        // If the caller requested the new cell, pass it to them
        if( createdCell != nullptr )
            *createdCell = newCell;
    }
    else
    {
        // The requested coords do not exist
        return false;
    }

    return true;
}

void Grid::deleteNode( int row, int col )
{
    // If the given row/col doesn't exist there is nothing to do, just return
    if( !nodeExists( row, col ) )
        return;

    Node* nodeToDelete = grid_[ row * CHUNK_WIDTH + col ];

    if( nodeToDelete->getType() == NodeType::CELL )
    {
        // Remove the node from the vector
        for( auto it = cells.begin(); it != cells.end(); ++it )
        {
            if( *it == nodeToDelete )
            {
                cells.erase( it );
                break;
            }
        }
    }

    // Delete the node itself
    delete nodeToDelete;
    grid_[ row * CHUNK_WIDTH + col ] = nullptr;
}

bool Grid::nodeExists( int row, int col )
{
    if( row < 0 ) return false;
    if( col < 0 ) return false;
    if( row > CHUNK_WIDTH - 1 ) return false;
    if( col > CHUNK_WIDTH - 1 ) return false;

    return true;
}

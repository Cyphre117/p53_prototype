#include "grid.h"
#include "cell.h"
#include "blood_vessel.h"
#include <iostream>

Grid::Grid()
{
    // Initialise the grid to empty
    for( int col = 0; col < CHUNK_WIDTH; col++ )
    {
        for( int row = 0; row < CHUNK_WIDTH; row++ )
        {
            grid_[row * CHUNK_WIDTH + col] = new Node( row, col, NodeType::EMPTY );
        }
    }
    // Not necessary, but may as well reserve the space ahead of time as an optimization
    cells_.reserve( CHUNK_WIDTH * CHUNK_WIDTH );
}

Grid::~Grid()
{
    
    // first set all the nodes to empty
    for( auto& node : grid_ )
    {
        setEmpty( node->getRow(), node->getColumn() );
    }

    // then delete each node
    for( auto& node : grid_ )
    {
        delete node;
        node = nullptr;
    }

    /*
    // Remove the blood vessels first
    for( auto& bv : bloodVessels_ )
    {
        int row = bv->getRow();
        int col = bv->getColumn();

        // Get the outer parts of the blood vessel and set them to nullptr
        Node* edges[6];
        getNeighbours( row, col, edges );
        for( int i = 0; i < 6; i++ )
        {
            int nr = edges[i]->getRow();
            int nc = edges[i]->getColumn();
            
            deleteNode( nr, nc );    
        }

        delete grid_[ row * CHUNK_WIDTH + col ];
        grid_[ row * CHUNK_WIDTH + col ] = nullptr;
    }
    bloodVessels_.clear();

    std::cout << "nodes " << std::endl;

    //*
    // Cleanup nodes
    for( auto& node : grid_ )
    {
        if( node != nullptr )
        {
            delete node;
            node = nullptr;
        }
    } 

    Node* n;
    for( int y = 0; y < CHUNK_WIDTH; y++ )
    {
        for( int x = 0; x < CHUNK_WIDTH; x++ )
        {
            getNode( x, y, &n );

            if( n == nullptr ) { std::cout << ". "; continue; }
            else if( n->getType() == NodeType::EMPTY ) std::cout << ". ";
            else if( n->getType() == NodeType::CELL ) std::cout << "c ";
            else std::cout << "O ";

            if( y > 3 ) std::cout << x << y << std::endl;
            grid_[ x * CHUNK_WIDTH + y ] = nullptr;
            delete n;

        }
        std::cout << std::endl;
        for( int i = 0; i < y + 1; i++ )
            std::cout << " ";
    }


    std::cout << "destroy" << std::endl;
    */
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

bool Grid::getNeighbours( int row, int col, Node** neighbours )
{
    // If the central node doesn't exist, it cannot have any neighbours
    if( !nodeExists( row, col ) )
        return false;

    int i = 0, nr = row, nc = col - 1;

    if( !nodeExists( nr, nc) ) neighbours[i] = nullptr;
    else neighbours[i] = grid_[ nr * CHUNK_WIDTH + nc ];

    i++; nr = row + 1;

    if( !nodeExists( nr, nc) ) neighbours[i] = nullptr;
    else neighbours[i] = grid_[ nr * CHUNK_WIDTH + nc ];

    i++; nr = row - 1; nc = col;

    if( !nodeExists( nr, nc) ) neighbours[i] = nullptr;
    else neighbours[i] = grid_[ nr * CHUNK_WIDTH + nc ];

    i++; nr = row + 1;

    if( !nodeExists( nr, nc) ) neighbours[i] = nullptr;
    else neighbours[i] = grid_[ nr * CHUNK_WIDTH + nc ];

    i++; nr = row - 1; nc = col + 1;

    if( !nodeExists( nr, nc) ) neighbours[i] = nullptr;
    else neighbours[i] = grid_[ nr * CHUNK_WIDTH + nc ];

    i++; nr = row;

    if( !nodeExists( nr, nc) ) neighbours[i] = nullptr;
    else neighbours[i] = grid_[ nr * CHUNK_WIDTH + nc ];

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
        cells_.push_back( newCell );

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

bool Grid::newBloodVessel( int row, int col, BloodVessel** createdBloodVessel )
{
    Node* current;
    BloodVessel* newBloodVessel;

    if( getNode( row, col, &current ) )
    {
        // If there was a node already there, clean it up
        if( current != nullptr )
            delete current;

        // Initialise the new blood vessel
        newBloodVessel = new BloodVessel( row, col );
        
        // Save the new blood vessel to the grid
        grid_[ row * CHUNK_WIDTH + col ] = newBloodVessel;
        
        // Set the neighbours to also point to the newBloodVessel
        Node* neighbours[6];
        getNeighbours( row, col, neighbours );
        for( int i = 0; i < 6; i++ )
        {
            // getNeighbours returns nullptr to represent nodes that do not exist
            if( neighbours[i] == nullptr ) continue;

            // Delete the old contents of the node
            setEmpty( neighbours[i]->getRow(), neighbours[i]->getColumn() );

            // Set the pointer to point to the newBloodVessel
            grid_[ neighbours[i]->getRow() * CHUNK_WIDTH + neighbours[i]->getColumn() ] = newBloodVessel;
        }

        // and store it in the vector
        bloodVessels_.push_back( newBloodVessel );

        // If the caller requested a ptr to the blood vessel, give it to them
        if( createdBloodVessel != nullptr )
            *createdBloodVessel = newBloodVessel;
    }

    return true;
}

void Grid::setEmpty( int row, int col )
{
    // If the given row/col doesn't exist there is nothing to do, just return
    if( !nodeExists( row, col ) )
        return;

    Node* nodeToDelete = grid_[ row * CHUNK_WIDTH + col ];

    // If it's a cell or a blood vessel remove it from it's vector
    if( nodeToDelete->getType() == NodeType::CELL )
    {
        // Remove the node from the vector
        for( auto it = cells_.begin(); it != cells_.end(); ++it )
        {
            if( *it == nodeToDelete )
            {
                cells_.erase( it );
                break;
            }
        }
    }
    else if( nodeToDelete->getType() == NodeType::BLOOD_VESSEL )
    {
        for( auto it = bloodVessels_.begin(); it != bloodVessels_.end(); ++it )
        {
            if( *it == nodeToDelete )
            {
                bloodVessels_.erase( it );
                break;
            }
        }
    }

    // If the node is a blood vessel, make sure we delete the whole thing
    if( nodeToDelete->getType() == NodeType::BLOOD_VESSEL )
    {
        int row = nodeToDelete->getRow();
        int col = nodeToDelete->getColumn();

        // Get the outer parts of the blood vessel and set them to nullptr
        Node* edges[6];
        getNeighbours( row, col, edges );
        for( int i = 0; i < 6; i++ )
        {
            //TODO: this is wrong when using blood vessels the way I currently have them
            // the node points to the center so when you do get row/col of the cell you want you are
            // actually getting the row col of the centre of the blood vessel!
            int nr = edges[i]->getRow();
            int nc = edges[i]->getColumn();

            // empty the edges
            grid_[ nr * CHUNK_WIDTH + nc ] = nullptr;
        }
        
        // empty the centre
        delete grid_[ row * CHUNK_WIDTH + col ];
        grid_[ row * CHUNK_WIDTH + col ] = nullptr;

    }
    else
    {
        delete nodeToDelete;
        grid_[ row * CHUNK_WIDTH + col ] = new Node( row, col, NodeType::EMPTY );
    }
}

bool Grid::nodeExists( int row, int col )
{
    if( row < 0 ) return false;
    if( col < 0 ) return false;
    if( row > CHUNK_WIDTH - 1 ) return false;
    if( col > CHUNK_WIDTH - 1 ) return false;

    return true;
}

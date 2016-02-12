#ifndef GRID_H
#define GRID_H

#include <array>
#include <vector>

#define CHUNK_WIDTH 16

// Forward declarations
class Node;
class Cell;

class Grid
{
public:
    Grid();
    ~Grid();

    // [in] row
    // [in] column
    // [out] node at given row column if found
    // [ret] true if given row/col lies on board, false otherwise
    bool getNode( int row, int col, Node** node );

    // [in] row to create cell
    // [in] column to create cell
    // [out/nullptr] the newly created cell, nullptr if the caller doesn't care
    // [ret] true if given row/col lies on board & cell created, false otherwise
    bool newCell( int row, int col, Cell** createdCell );

    // [in] row of node to be deleted
    // [in] column of node to be deleted
    void deleteNode( int row, int col );

    std::vector<Cell*> cells;

    // Clears and refills the vector of Cell*
    void rebuildCellsList() {}

private:

    std::array<Node*, CHUNK_WIDTH * CHUNK_WIDTH> grid_;
        
    // Returns true if the given row/col lies on the grid
    bool nodeExists( int row, int col );

};

#endif

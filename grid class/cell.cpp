#include "cell.h"
#include <iostream>

Cell::Cell( int row, int col ) :
Node( row, col, NodeType::CELL )
{
}

Cell::~Cell()
{
}

void Cell::print()
{
    std::cout << "Cell at: " << row_ << " " << col_ << std::endl;
}

#ifndef CELL_H
#define CELL_H

#include "node.h"

class Cell : public Node 
{
public: 
    Cell( int row, int col );
    virtual ~Cell();

    void print();

protected:
};

#endif

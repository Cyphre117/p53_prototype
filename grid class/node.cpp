#include "node.h"

Node::Node( int row, int col, NodeType type ) :
    row_( row ),
    col_( col ),
    type_( type )
{
}

Node::~Node()
{
}

#ifndef NODE_H
#define NODE_H

#include <array>

enum class NodeType { EMPTY, CELL,  BLOOD_VESSEL };

class Node
{
    public:
        Node( int row, int col, NodeType type );
        virtual ~Node();

        // Getters
        int getRow() { return row_; }
        int getColumn() { return col_; }
        NodeType getType() { return type_; }

    protected:

        int row_;
        int col_;
        NodeType type_;
};

#endif

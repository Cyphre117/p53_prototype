#ifndef NODE_H
#define NODE_H

enum class NodeType { CELL,  BLOOD_VESSEL };

class Node
{
    public:
        virtual ~Node();

        NodeType getType() { return type_; }

    protected:
        // Cannot create Nodes directly
        Node( int row, int col, NodeType type );

        int row_;
        int col_;
        NodeType type_;
};

#endif

/**
 *
 * octree.h
 *
 * Octal Tree data structure implementation with C++11
 *
 * Written by Taylor Shin
 * Sep. 16th, 2016
 *
**/

#ifndef __octree_h__
#define __octree_h__

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <istream>
#include <ostream>
#include <memory>

#include "tarray2D.h"
#include "tcoord3D.h"

// TODO: Change Node* types to unique_ptr or shared_ptr

OPEN_NS_OCTREE

/**
    *
    * The octal-tree class!!
    *
**/
template<typename T, int AS = 1>
class OcTree {

protected:
    // OcTree element definitions
    class Node;
    class Branch;
    class Aggregate;
    class Leaf;
    enum NodeType { BranchNode, AggregateNode, LeafNode };

    Node*& root();
    const Node* root() const;
    //std::shared_ptr<Node>& root();
    //std::unique_ptr<Node> root() const;

    static void deleteNode(Node** node);

protected:

    // Internal class definitions

    // Node
    class Node
    {
    public:
        NodeType type() const;

    protected:
        Node(NodeType type);
        ~Node() { ; };

    private:
        NodeType type_ : 2;
    };


    // Branch
    class Branch : public Node
    {
    public:
        Branch();
        Branch(const Branch& b);
        ~Branch();

        const Node* child(uint64_t x, uint64_t y, uint64_t z) const;
        Node*& child(uint64_t x, uint64_t y, uint64_t z);
        const Node* child(uint64_t index) const;
        Node*& child(uint64_t index);

        friend void OcTree<T, AS>::deleteNode(Node** node);
    private:
        Branch& operator= (Branch b);
    private:
        Node* children[2][2][2];
    };

    // Aggregate
    class Aggregate : public Node
    {
    public:
        Aggregate(const T& v);

        const T& value(uint64_t x, uint64_t y, uint64_t z) const;
        T& value(uint64_t x, uint64_t y, uint64_t z);
        void setValue(uint64_t x, uint64_t y, uint64_t z, const T& v);

        const T& value(uint64_t i) const;
        T& value(uint64_t i);
        void setValue(uint64_t i, const T& v);

        friend void OcTree<T, AS>::deleteNode(Node** node);

    private:
        ~Aggregate() { ; };

    private:
        T value_[AS][AS][AS];
    };

    // Leaf
    class Leaf : public Node
    {
    public:
        Leaf(const T& v);

        const T& value() const;
        T& value();
        void setValue(const T& v);

        friend void OcTree<T, AS>::deleteNode(Node** node);

    private:
        ~Leaf() { ; }

    private:
        T value_;
    };

    static const int aggregateSize_ = AS;

private:
    // Recursive helpers
    void eraseRecursive(Node** node, uint64_t size, uint64_t x, uint64_t y, uint64_t z);
    static uint64_t bytesRecursive(const Node* node);
    static uint64_t nodesRecursive(const Node* node);
    static uint64_t nodesAtSizeRecursive(uint64_t targetSize, uint64_t size, Node* node);
    void zSliceRecursive(
        TArray2D<T> slice, const Node* node, uint64_t size,
        uint64_t x, uint64_t y, uint64_t z, uint64_t targetZ) const;
    static void writeBinaryRecursive(std::ostream& out, const Node* node);
    static void readBinaryRecursive(std::istream& in, Node** node);

public:
    // Access methods
    uint64_t size() const;
    const T& emptyValue() const;

    static uint64_t branchBytes();
    static uint64_t aggregateBytes();
    static uint64_t leafBytes();
    uint64_t bytes() const;

    uint64_t nodes() const;
    uint64_t nodesAtSize(uint64_t size) const;

    // Mutators
    void setEmptyValue(const T& emptyValue);

    void swap(OcTree<T, AS>& o);
    OcTree<T, AS>& operator= (OcTree<T, AS> o);

    // Indexing operators
    T& operator() (uint64_t x, uint64_t y, uint64_t z);
    const T& operator() (uint64_t x, uint64_t y, uint64_t z) const;
    const T& at(uint64_t x, uint64_t y, uint64_t z) const;

    void set(uint64_t x, uint64_t y, uint64_t z, const T& value);
    void erase(uint64_t x, uint64_t y, uint64_t z);

    TArray2D<T> zSlice(uint64_t z) const;

    // I/O functions
    void writeBinary(std::ostream& out) const;
    void readBinary(std::istream& in);

    /**
        *
        * Constructors and Destructors
        *
    **/
    OcTree(uint64_t size, T emptyValue, bool force_type);
    OcTree(uint64_t size, const T& emptyValue = T(0));
    OcTree(const OcTree<T, AS>& o);
    virtual ~OcTree();

private:
    // Data storages
    Node* root_;
    T emptyValue_;
    uint64_t size_;

}; /* class OcTree */

CLOSE_NS_OCTREE

#include "octree.tcc"

#endif /* Inlclude guard */

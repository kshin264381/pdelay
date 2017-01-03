/**
 *
 * octree.tcc
 *
 * Octal Tree data structure implementation with C++11
 * (Method implementation part)
 *
 * Written by Taylor Shin
 * Sep. 16th, 2016
 *
**/

//#include "octree.h"

OPEN_NS_OCTREE

/**
 *
 * Generating a octal-tree.
 * OcTree must be size of power of 2.
 * i.e. if size 256, the cube will be divided into 256 x 256 x 256 nodes.
 *
 * emptyValue will be returned if nothing has been assigned.
 *
**/
template<typename T, int AS>
OcTree<T, AS>::OcTree(uint64_t size, const T& emptyValue) : \
root_(nullptr),
emptyValue_(emptyValue),
size_(size)
{
    // Making sure size is 2^n
    assert(((size - 1) & size) == 0);
    assert(((AS - 1) & AS) == 0);
}

/**
 *
 * Another interface for octal-tree generator.
 * Same idea with above but accepts datatype of pointer
 *
**/
template<typename T, int AS>
OcTree<T, AS>::OcTree(uint64_t size, T emptyValue, bool force_type) : \
root_(nullptr),
emptyValue_(emptyValue),
size_(size)
{
    // Making sure size is 2^n
    assert(((size - 1) & size) == 0);
    assert(((AS - 1) & AS) == 0);
}


/**
 *
 * Generating an octal tree from another octal tree.
 *
 * Basically, we'll place another octal tree into memory rather than
 * returning a pointer.
 *
**/
template<typename T, int AS>
OcTree<T, AS>::OcTree(const OcTree<T, AS>& o) : \
emptyValue_(o.emptyValue_),
size_(o.size_)
{
    if (!o.root_) {
        root_ = nullptr;
    }
    else {
        switch (o.root_->type()) {
        case BranchNode:
            root_ = new Branch(*reinterpret_cast<Branch*>(o.root_));
            break;
        case LeafNode:
            root_ = new Leaf(*reinterpret_cast<Leaf*>(o.root_));
            break;
        case AggregateNode:
            root_ = new Aggregate(*reinterpret_cast<Aggregate*>(o.root_));
            break;
        }
    }
}

/**
 *
 * Destructor, Delete everything recursively.
 *
**/
template<typename T, int AS>
OcTree<T, AS>::~OcTree()
{
    deleteNode(&root_);
}




/**
 *
 * OcTree methods
 *
**/

/**
 * Swaps the octree's contents with another's. Only swaps root pointer...
 * not the entire stuff
**/
template<typename T, int AS>
void OcTree<T, AS>::swap(OcTree<T, AS>& o)
{
    std::swap(emptyValue_, o.emptyValue_); // This might throw some error

    // They won't throw any error... I hope...
    std::swap(root_, o.root_);
    std::swap(size_, o.size_);
}

/**
 * Assignment operator
**/
template<typename T, int AS>
OcTree<T, AS>& OcTree<T, AS>::operator= (
    OcTree<T, AS> o)
{
    swap(o);
    return *this;
}

/**
 * Returns size of octree.
**/
template<typename T, int AS>
uint64_t OcTree<T, AS>::size() const
{
    return size_;
}

/**
 * returns value of emptyValue... obviously...
**/
template<typename T, int AS>
const T& OcTree<T, AS>::emptyValue() const
{
    return emptyValue_;
}

/**
 * re-initializes the value of emptyValue... obviously...
**/
template<typename T, int AS>
void OcTree<T, AS>::setEmptyValue(const T& emptyValue)
{
    emptyValue_ = emptyValue;
}

/**
 * Deletes all nodes.. and connecting ones... 
 *
 * Exterminatus!!
**/
template<typename T, int AS>
void OcTree<T, AS>::deleteNode(Node** node)
{
    assert(node);

    if (*node) {
        if ((*node)->type() == BranchNode) {
            delete reinterpret_cast<Branch*>(*node);
        }
        else if ((*node)->type() == AggregateNode) {
            delete reinterpret_cast<Aggregate*>(*node);
        }
        else {
            assert((*node)->type() == LeafNode);
            delete reinterpret_cast<Leaf*>(*node);
        }
        *node = nullptr;
    }
}

/**
 * Returns root node pointer
**/
template<typename T, int AS>
typename OcTree<T, AS>::Node*& OcTree<T, AS>::root()
{
    return root_;
}

/**
 * Const version...
**/
template<typename T, int AS>
const typename OcTree<T, AS>::Node* OcTree<T, AS>::root() const
{
    return root_;
}

/**
 * Returns value at index (x, y, z). If nothing can be found at the
 * Coordinate, returns emptyValue().
 * 
 * Note that memory access for x is the fastest and z is the slowest. 
 * So, when you make a search nested loop, make sure you loop with
 * z -> y -> x order.
**/
template<typename T, int AS>
const T& OcTree<T, AS>::at(uint64_t x, uint64_t y, uint64_t z) const
{
    assert(x >= 0 && x < size_);
    assert(y >= 0 && y < size_);
    assert(z >= 0 && z < size_);

    Node* n = root_;
    uint64_t size = size_;

    while (size != aggregateSize_) {
        if (!n) {
            return emptyValue_;
        }
        else if (n->type() == BranchNode) {
            size /= 2;
            n = reinterpret_cast<Branch*>(n)->child(
                !!(x&size), !!(y&size), !!(z&size) );
        }
        else {
            assert(n->type() == LeafNode);
            return reinterpret_cast<Leaf*>(n)->value();
        }
    }

    if (!n) {
        return emptyValue_;
    }

    --size;

    return reinterpret_cast<Aggregate*>(n)->value(
        x&size, y&size, z&size);

}

/**
 * same as at() but easier to use
**/
template<typename T, int AS>
const T& OcTree<T, AS>::operator() (uint64_t x, uint64_t y, uint64_t z) const
{
    return at(x, y, z);
}

/**
 * returns reference to value at (x,y,z). If nothing exists there,
 * a new node is generated and initialized by emptyValue() and 
 * returns it.
 *
 * It's better use at() instead of relying this crap. We better conserve
 * the original data, right?
**/
template<typename T, int AS>
T& OcTree<T, AS>::operator() (uint64_t x, uint64_t y, uint64_t z)
{
    assert(x >= 0 && x < size_);
    assert(y >= 0 && y < size_);
    assert(z >= 0 && z < size_);

    Node** n = &root_;
    uint64_t size = size_;

    while (size != aggregateSize_) {
        if (!*n) {
            *n = new Branch;
        }
        else if ((*n)->type() == BranchNode) {
            size /= 2;
            n = &reinterpret_cast<Branch*>(*n)->child(
                !!(x&size), !!(y&size), !!(z&size));
        }
        else {
            return reinterpret_cast<Leaf*>(*n)->value();
        }
    }

    if (!*n) {
        *n = new Aggregate(emptyValue_);
    }

    --size;
    return reinterpret_cast<Aggregate*>(*n)->value(x&size, y&size, z&size);
}

/**
 * Assignes a value at node (x, y, z). If an emptyValue() is given, 
 * erase the node. If the node (x, y, z) doesn't exist, creates it
 * and assignes the given value.
**/
template<typename T, int AS>
void OcTree<T, AS>::set(
    uint64_t x, uint64_t y, uint64_t z, const T& value)
{
    if (value != emptyValue()) {
        (*this)(x, y, z) = value;
    }
    else {
        erase(x, y, z);
    }
}

/**
 * Erases node at (x, y, z). If (x, y, z) doesn't exist, returns 
 * emptyValue().
 *
 * Also tries to free up memory as much as possible. i.e. if a note at
 * a single node branch is deleted, it will also delete the branch
 * node as well.
**/
template<typename T, int AS>
void OcTree<T, AS>::erase(uint64_t x, uint64_t y, uint64_t z)
{
    assert(x >= 0 && x < size_);
    assert(y >= 0 && y < size_);
    assert(z >= 0 && z < size_);

    eraseRecursive(&root_, size_, x, y, z);
}


/**
 * The recursive part for erase()
 *
**/
template<typename T, int AS>
void OcTree<T, AS>::eraseRecursive(
    Node** node, uint64_t size,
    uint64_t x, uint64_t y, uint64_t z)
{
    assert(node);

    if (!*node) {
        return;
    }

    if (size != aggregateSize_) {
        if ((*node)->type() == BranchNode) {
            size /= 2;
            Branch* b = reinterpret_cast<Branch*>(*node);
            eraseRecursive(
                &b->child(!!(x&size), !!(y&size), !!(z&size)),
                size, x, y, z);

            for (auto i = 0; i < 8; ++i) {
                if (b->child(i)) {
                    return;
                }
            }
            deleteNode(node);
        } /* if ((*node)->type() == BranchNode) */
        else if (reinterpret_cast<Leaf*>(*node)->value() == emptyValue_) {
            deleteNode(node);
        } /* if ((*node)->type() == BranchNode) */
        else {
            Branch* b = new Branch;
            size /= 2;
            auto childIndex = (x&size ? 1 : 0) \
                | (y&size ? 2 : 0) \
                | (z&size ? 4 : 0);
            const T& value = reinterpret_cast<Leaf*>(*node)->value();

            try {
                for (auto i = 0; i < 8; ++i) {
                    if (i == childIndex) {
                        continue;
                    }
                    if (size == aggregateSize_) {
                        b->child(i) = new Leaf(value);
                    }
                    else {
                        b->child(i) = new Aggregate(value);
                    }
                }
            }
            catch (...) {
                Node* bb = b;
                deleteNode(&bb);
                throw;
            }

            deleteNode(node);
            *node = b;
            node - &b->child(childIndex);

        } /* if ((*node)->type() == BranchNode) */

    } /* if (size != aggregateSize_) */
    else {
        --size;
        Aggregate* a = reinterpret_cast<Aggregate*>(*node);
        a->setValue(x&size, y&size, z&size, emptyValue_);

        for (auto i = 0; i < AS*AS*AS; ++i) {
            if (a->value(i) != emptyValue_) {
                return;
            }
        }
        deleteNode(node);
    } /* if (size != aggregateSize_) */
}

/**
 * Returns number of bytes of current branh node
**/
template<typename T, int AS>
uint64_t OcTree<T, AS>::branchBytes()
{
    return sizeof(Branch);
}

/**
 * Returns number of bytes of current aggregate node
**/
template<typename T, int AS>
uint64_t OcTree<T, AS>::aggregateBytes()
{
    return sizeof(Aggregate);
}

/**
 * Returns number of bytes of current leaf node
**/
template<typename T, int AS>
uint64_t OcTree<T, AS>::leafBytes()
{
    return sizeof(Leaf);
}

/**
 * Returns total number of nodes in the octree
**/
template<typename T, int AS>
uint64_t OcTree<T, AS>::nodes() const
{
    return nodesRecursive(root_);
}

/**
 * the actual implementation for nodes() method
**/
template<typename T, int AS>
uint64_t OcTree<T, AS>::nodesRecursive(
    const Node* node)
{
    if (!node) {
        return 0;
    }

    uint64_t n = 1;
    if (node->type() == BranchNode) {
        for (auto i = 0; i < 8; ++i) {
            n += nodesRecursive(
                reinterpret_cast<const Branch*>(node)->child(i));
        }
    }

    return n;
}


/**
 * Returns total number of bytes assigned to the octree.
 *
 * --> this code isn't free from memory fragmentation.
**/
template<typename T, int AS>
uint64_t OcTree<T, AS>::bytes() const
{
    return bytesRecursive(root_) + sizeof(*this);
}

/**
 * The actual implementation of the bytes() method.
**/
template<typename T, int AS>
uint64_t OcTree<T, AS>::bytesRecursive(
    const Node* node)
{
    if (!node) {
        return 0;
    }

    uint64_t b = 0;
    switch (node->type()) {
    case BranchNode:
        b = sizeof(Branch);
        for (auto i = 0; i < 8; ++i) {
            b += bytesRecursive(
                reinterpret_cast<const Branch*>(node)->child(i));
        }
        break;
    case LeafNode:
        b = sizeof(Leaf);
        break;
    case AggregateNode:
        b = sizeof(Aggregate);
        break;
    }

    return b;
}

/**
 * Returns number of nodes of at size. In detail, let's assume a root is 
 * a single node of size 1. At size n there maybe a maximum of 2 nodes.
 *
 * For sizes lower than aggregate size, the return will be zero.
**/
template<typename T, int AS>
uint64_t OcTree<T, AS>::nodesAtSize(uint64_t size) const
{
    return nodesAtSizeRecursive(size, size_, root_);
}

/**
 * The actual implementation of nodesAtSize
**/
template<typename T, int AS>
uint64_t OcTree<T, AS>::nodesAtSizeRecursive(
    uint64_t targetSize, uint64_t size, Node* node)
{
    if (node) {
        if (size == targetSize) {
            return 1;
        }

        if (node->type() == BranchNode) {
            auto sum = 0;
            for (auto i = 0; i < 2; ++i) {
                for (auto j = 0; j < 2; ++j) {
                    for (auto k = 0; k < 2; ++k) {
                        sum += nodesAtSizeRecursive(targetSize, size / 2,
                            reinterpret_cast<Branch*>(node)->child(k, j, i));
                    }
                }
            }
            return sum;
        }
    }

    return 0;
}

template<typename T, int AS>
OcTree<T, AS>::Node::Node(NodeType type)
    : type_(type)
{
}

template<typename T, int AS>
typename OcTree<T, AS>::NodeType 
OcTree<T, AS>::Node::type() const
{
    return type_;
}

template<typename T, int AS>
OcTree<T, AS>::Branch::Branch()
    :Node(BranchNode)
{
    memset(children, 0, sizeof(children));
}

template<typename T, int AS>
OcTree<T, AS>::Branch::Branch(const Branch& b)
    : Node(BranchNode)
{
    for (auto i = 0; i < 8; ++i) {
        if (b.child(i)) {
            switch (b.child(i)->type()) {
            case BranchNode:
                child(i) = new Branch(
                    *reinterpret_cast<const Branch*>(b.child(i)));
                break;
            case LeafNode:
                child(i) = new Leaf(
                    *reinterpret_cast<const Leaf*>(b.child(i)));
                break;
            case AggregateNode:
                child(i) = new Aggregate(
                    *reinterpret_cast<const Aggregate*>(b.child(i)));
                break;
            }
        }
        else {
            child(i) = 0;
        }
    }
}

template<typename T, int AS>
OcTree<T, AS>::Branch::~Branch()
{
    for (auto i = 0; i < 2; ++i) {
        for (auto j = 0; j < 2; ++j) {
            for (auto k = 0; k < 2; ++k) {
                assert(children[i][j][k] != this);
                deleteNode(&children[i][j][k]);
            }
        }
    }
}

template<typename T, int AS>
typename OcTree<T, AS>::Branch& 
OcTree<T, AS>::Branch::operator= (
    typename OcTree<T, AS>::Branch b)
{
    assert(b);
    return (*this);
}

template<typename T, int AS>
const typename OcTree<T, AS>::Node* 
OcTree<T, AS>::Branch::child(
    uint64_t x, uint64_t y, uint64_t z) const
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    return children[z][y][x];
}

template<typename T, int AS>
typename OcTree<T, AS>::Node*& 
OcTree<T, AS>::Branch::child(
    uint64_t x, uint64_t y, uint64_t z)
{
    assert(x == 0 || x == 1);
    assert(y == 0 || y == 1);
    assert(z == 0 || z == 1);
    return children[z][y][x];
}

template<typename T, int AS>
const typename OcTree<T, AS>::Node* 
OcTree<T, AS>::Branch::child(uint64_t index)
const
{
    assert(index >= 0 && index < 8);
    return *(&children[0][0][0] + index);
}

template<typename T, int AS>
typename OcTree<T, AS>::Node*& 
OcTree<T, AS>::Branch::child(uint64_t index)
{
    assert(index >= 0 && index < 8);
    return *(&children[0][0][0] + index);
}

template<typename T, int AS>
OcTree<T, AS>::Aggregate::Aggregate(const T& v)
    : Node(AggregateNode)
{
    for (auto i = 0; i < AS; ++i) {
        for (auto j = 0; j < AS; ++j) {
            for (auto k = 0; k < AS; ++k) {
                value_[i][j][k] = v;
            }
        }
    }
}

template<typename T, int AS>
const T& OcTree<T, AS>::Aggregate::value(
    uint64_t x, uint64_t y, uint64_t z) const
{
    assert(x >= 0 && x < AS);
    assert(y >= 0 && y < AS);
    assert(z >= 0 && z < AS);

    return value_[z][y][x];
}

template<typename T, int AS>
T& OcTree<T, AS>::Aggregate::value(
    uint64_t x, uint64_t y, uint64_t z)
{
    assert(x >= 0 && x < AS);
    assert(y >= 0 && y < AS);
    assert(z >= 0 && z < AS);

    return value_[z][y][x];
}

template<typename T, int AS>
void OcTree<T, AS>::Aggregate::setValue(
    uint64_t x, uint64_t y, uint64_t z, const T& v)
{
    assert(x >= 0 && x < AS);
    assert(y >= 0 && y < AS);
    assert(z >= 0 && z < AS);

    value_[z][y][x] = v;
}

template<typename T, int AS>
const T& OcTree<T, AS>::Aggregate::value(uint64_t i) const
{
    assert(i >= 0 && i < AS*AS*AS);

    return *(&value_[0][0][0] + i);
}

template<typename T, int AS>
T& OcTree<T, AS>::Aggregate::value(uint64_t i)
{
    assert(i >= 0 && i < AS*AS*AS);

    return *(&value_[0][0][0] + i);
}

template<typename T, int AS>
void OcTree<T, AS>::Aggregate::setValue(uint64_t i, const T& v)
{
    assert(i >= 0 && i < AS*AS*AS);

    *(&value_[0][0][0] + i) = v;
}

template<typename T, int AS>
OcTree<T, AS>::Leaf::Leaf(const T& v)
    : Node(LeafNode)
    , value_(v)
{
}

template<typename T, int AS>
const T& OcTree<T, AS>::Leaf::value() const
{
    return value_;
}

template<typename T, int AS>
T& OcTree<T, AS>::Leaf::value()
{
    return value_;
}

template<typename T, int AS>
void OcTree<T, AS>::Leaf::setValue(const T& v)
{
    value_ = v;
}

/**
 * Returns a perpendicular to Z axis slice. So, the input parameter
 * is Z and returns contents on all given z coordinate. As usual, if
 * node doesn't exist, returns emptyValue().
 *
 * Just like at(), we'll iterate z -> y -> x order.
**/
template<typename T, int AS>
TArray2D<T> OcTree<T, AS>::zSlice(uint64_t z) const
{
    assert(z >= 0 && z < size_);

    TArray2D<T> slice(size_, size_);
    zSliceRecursive(slice, root_, size_, 0, 0, 0, z);

    return slice;
}

/**
 * Recursive implementation of zSlice
**/
template<typename T, int AS>
void OcTree<T, AS>::zSliceRecursive(
    TArray2D<T> slice, const Node* node, uint64_t size,
    uint64_t x, uint64_t y, uint64_t z, uint64_t targetZ) const
{
    if (!node) {
        for (auto i = 0; i < slice.M(); ++i) {
            for (auto j = 0; j < slice.N(); ++j) {
                slice(i, j) = emptyValue_;
            }
        }
    }
    else if (node->type() == BranchNode) {
        size /= 2;
        for (auto i = 0; i < 2; ++i) {
            for (auto j = 0; j < 2; ++j) {
                zSliceRecursive(slice.subarray(
                    i*size, j*size, (i + 1)*size, (j + 1)*size),
                    reinterpret_cast<const Branch*>(node)->child(
                        j, i, !!(targetZ&size)), size, x, y, z, targetZ);
            }
        }
    }
    else if (node->type() == AggregateNode) {
        for (auto i = 0; i < slice.M(); ++i) {
            for (auto j = 0; j < slice.N(); ++j) {
                slice(i, j) = \
                    reinterpret_cast<const Aggregate*>(node)->value(
                    j, i, targetZ - z & (size - 1));
            }
        }
    }
}

/**
 * Writes the octree in binary form. Simple and fast method to
 * generate output.
**/
template<typename T, int AS>
void OcTree<T, AS>::writeBinary(std::ostream& out) const
{
    if (!root_) {
        static const char zero = 0;
        out.write(&zero, 1);
    }
    else {
        static const char one = 1;
        out.write(&one, 1);

        writeBinaryRecursive(out, root());
    }

    out.write(reinterpret_cast<const char*>(&emptyValue_), sizeof(T));
    out.write(reinterpret_cast<const char*>(&size_), sizeof(uint64_t));
}

/**
 * The recursive implementation of writeBinary
**/
template<typename T, int AS>
void OcTree<T, AS>::writeBinaryRecursive(
    std::ostream& out, const Node* node)
{
    assert(node);

    if (!out.good()) {
        return;
    }

    char type = node->type();
    out.write(&type, 1);

    switch (type) {
    case BranchNode:
        const Branch* b = reinterpret_cast<const Branch*>(node);

        char children = 0;
        for (auto i = 0; i < 8; ++i) {
            children |= (b->child(i) != 0) << i;
        }
        out.write(&children, 1);

        for (auto i = 0; i < 8; ++i) {
            if (b->child(i)) {
                writeBinaryRecursive(out, b->child(i));
            }
        }
        break;

    case AggregateNode:
        out.write(reinterpret_cast<const char*>(
            &reinterpret_cast<const Aggregate*>(node)->value(0, 0, 0)),
            AS*AS*AS * sizeof(T));
        break;

    case LeafNode:
        out.write(reinterpret_cast<const char*>(
            &reinterpret_cast<const Leaf*>(node)->value()),
            sizeof(T));
        break;

    } /* switch (type) */
}


/**
 * Reads in octree from a binary file written with the writeBinary()
**/
template<typename T, int AS>
void OcTree<T, AS>::readBinary(std::istream& in)
{
    OcTree<T, AS> tmp(0);

    char root;
    in.read(&root, 1);
    if (root) {
        readBinaryRecursive(in, &tmp.root_);
    }

    in.read(reinterpret_cast<char*>(&tmp.emptyValue_), sizeof(T));
    in.read(reinterpret_cast<char*>(&tmp.size_), sizeof(uint64_t));

    if (in.good()) {
        swap(tmp);
    }
}

/**
 * The recursive part of readBinary
**/
template<typename T, int AS>
void OcTree<T, AS>::readBinaryRecursive(
    std::istream& in, Node** node)
{
    assert(node);

    if (!in.good()) {
        return;
    }

    char type;
    in.read(&type, 1);

    switch (type) {
    case BranchNode:
        Branch* b = new Branch;
        *node = b;

        char children;
        in.read(&children, 1);

        for (auto i = 0; i < 8; ++i) {
            if (children & (1 << i)) {
                readBinaryRecursive(in, &b->child(i));
            }
        }
        break;

    case AggregateNode:
        Aggregate* a = new Aggregate(T(0));
        *node = a;
        in.read(reinterpret_cast<char*>(&a->value(0, 0, 0)), 
            AS*AS*AS * sizeof(T));
        break;

    case LeafNode:
        Leaf* l = new Leaf(T(0));
        *node = 1;
        in.read(reinterpret_cast<char*>(&l->value()), sizeof(T));
        break;

    } /* switch(type) */
}

CLOSE_NS_OCTREE
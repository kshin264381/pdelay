/**
 * pd_tcad_node.cc
 *
 *  Implementation for pd_tcad_node class
 *  defined at pd_tcad_node.h...
 *
 *  Created on: May 4, 2016
 *      Author: Taylor Shin
**/

#include <cstdlib>

#include "pd_tcad_node.h"

using namespace pdTCAD;

/*******************************
 * Private stuff               *
 *******************************/
/**
 * Utilities
 *
**/
template <typename FDATA, typename INDEX>
bool Node<FDATA,INDEX>::__comp_ind_vec(
    std::vector<INDEX>& IndexA, std::vector<INDEX>& IndexB)
{
    auto lenA = IndexA.size();
    auto lenB = IndexB.size();

    // if length of indices are different --> this must not happen!!
    if (lenA != lenB) { return false; }

    // ok, let's run iteration
    auto n = lenA;
    while (--n > 0 && IndexA[n] == IndexB[n]);
    // while loop broken before scanning entire vectors
    if (n) { return false; }
    else return true;
}

/*******************************
 * Public stuff                *
 *******************************/
/**
 * Adjacement node list manipulation
 *
**/
// Forcefully add give node to this->AdjNodeList
template <typename FDATA, typename INDEX>
bool Node<FDATA,INDEX>::AddNode(
    std::unique_ptr<Node<FDATA,INDEX>> new_node)
{
    // First, search duplicates, do nothing and return false
    if (FindNode(new_node))
    { return false; }

    // If given node is the same node... ignore it.
    if (__comp_ind_vec(new_node->GetIndex(), this->GetIndex()))
    { return false; }

    // Actually add the node!!
    this->AdjNodeList.push_back(new_node);
}

// Attach given node pointer (must be given as a unique pointer)
// to this->AdjNodeList
template <typename FDATA, typename INDEX>
bool Node<FDATA,INDEX>::Attach(
    std::unique_ptr<Node<FDATA,INDEX>> new_node)
{
    // Ok, actually attach the new node to this->AdjNodeList
    AddNode(new_node);
    // Also update the given adjacement node
    if (!new_node->FindNode(
        std::make_unique<Node<FDATA,INDEX>>(this)))
        new_node->AddNode(
            std::make_unique<Node<FDATA,INDEX>>(this));
}

// Remove node from this->AdjNodeList
template <typename FDATA, typename INDEX>
bool Node<FDATA,INDEX>::RemoveNode(
    std::unique_ptr<Node<FDATA,INDEX>> a_node)
{
    // If given node is not actually in here...
    if (!FindNode(a_node)) { return false; }

    // Ok, find index of the given node.
    auto this_node_list_len = this->AdjNodeList.size();
    auto i = 0;
    for (i=0; i<this_node_list_len; i++) {
        if (__comp_ind_vec(
            a_node->GetIndex(),
            this->AdjNodeList[i]->GetIndex())) break;
    }
    // Then remove it
    this->AdjNodeList.erase(this->AdjNodeList.begin()+i);

    return true;
}

// Detach current node from given node.
template <typename FDATA, typename INDEX>
bool Node<FDATA,INDEX>::Detach(
    std::unique_ptr<Node<FDATA,INDEX>> a_node)
{
    RemoveNode(a_node);
    a_node->RemoveNode(
        std::make_unique<Node<FDATA,INDEX>>(this));
}

template <typename FDATA, typename INDEX>
bool Node<FDATA,INDEX>::FindNode(
    std::unique_ptr<Node<FDATA,INDEX>> a_node)
{
    std::vector<INDEX> a_node_index = a_node->GetIndex();
    auto this_node_list_len = this->AdjNodeList.size();
    auto i = 0;
    std::vector<INDEX> tmp_node_index;

    // considered openmp here but decided not to use it.
    for (i=0; i<this_node_list_len; i++) {
        tmp_node_index = this->AjdNodeList[i]->GetIndex();
        if (__comp_ind_vec(a_node_index, tmp_node_index)) {
            return true;
        }
    }
    return false;
}

/**
 * Utilities
 *
**/
template <typename FDATA, typename INDEX>
bool Node<FDATA,INDEX>::SetDim()
{
    auto loc_dim     = this->Location.size();
    auto pot_dim     = this->Potential.size();
    auto field_dim   = this->EField.size();
    auto current_dim = this->Current.size();

    auto dim_list    = \
        { loc_dim, pot_dim, field_dim, current_dim };
    auto n = sizeof(dim_list)/sizeof(loc_dim);

    while(--n > 0 && dim_list[n] == dim_list[0]);
    if (n) {
        printf("Node: Data dimension mismatch!!\n");
        exit(-1);
    }

    return dim_list[n];
}
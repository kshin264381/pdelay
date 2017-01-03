/**
 *
 * pd_tcad_node.h
 *
 *  Defines a node element for pd_tcad_mesh and solver.
 *
 *  Created on: May 4, 2016
 *      Author: Taylor Shin
 *
**/

#ifndef __pd_tcad_node_h__
#define __pd_tcad_node_h__

#include <vector>
#include <deque>

#include "unique_ptr.h"
#include "datatype_hash.h"

using namespace DataType;

namespace pdTCAD
{

/**
 * The Node class
 *
 * Solver for Poisson and Continuous equation.
 * 
 * Must be defined with two datatypes:
 *   Floating point data, and Index data
 *
**/
// Defining a dummy multiple template class at first...
template <typename...> class Node;

template <typename FDATA, typename INDEX>
class Node<FDATA,INDEX> {
private:
    std::vector<FDATA> Location;   // Location from zero (in um)
    std::vector<FDATA> Potential;  // Potential (vector in V)
    std::vector<FDATA> EField;     // Electric Field (vector in MV/cm)
    std::vector<FDATA> Current;    // Current (vector in A/um)

    FDATA nDopConc;      // n-type doping concentration (in #/cm3)
    FDATA pDopConc;      // p-type doping concentration (in #/cm3)

    FDATA nNCarr;        // number of n-type carriers (in #/cm3)
    FDATA nPCarr;        // number of p-type carriers (in #/cm3)
    FDATA gNCarr;        // n-type carrier generation rate (in #/cm3/sec.)
    FDATA gPCarr;        // p-type carrier generation rate (in #/cm3/sec.)
    FDATA rNCarr;        // n-type carrier recombination rate (in #/cm3/sec.)
    FDATA rPCarr;        // p-type carrier recombination rate (in #/cm3/sec.)

    INDEX IndexX;        // X index number of current node.
    INDEX IndexY;        // Y index number of current node.

    Dict<std::string, FDATA> Material; // Material information

    short Dimension;     // System Dimension
    // Dealing with connections
    std::vector<std::unique_ptr<Node<FDATA,INDEX>>> AdjNodeList;

    // Compare vectors
    bool __comp_ind_vec(
        std::vector<INDEX>& IndexA,
        std::vector<INDEX>& IndexB);

public:
    /**
     * Some stuff that could be useful... or not.
     *
    **/
    // Data extraction stuff
    std::vector<FDATA> GetLocation()    { return this->Location; }
    std::vector<FDATA> GetPotential()   { return this->Potential; }
    std::vector<FDATA> GetEField()      { return this->EField; }
    std::vector<FDATA> GetCurrent()     { return this->Current; }
    FDATA              GetNDopConc()    { return this->nDopConc; }
    FDATA              GetPDopConc()    { return this->pDopConc; }
    FDATA              GetNCarr()       { return this->nNCarr; }
    FDATA              GetPCarr()       { return this->nPCarr; }
    FDATA              GetGenNCarr()    { return this->gNCarr; }
    FDATA              GetGenPCarr()    { return this->gPCarr; }
    FDATA              GetRecNCarr()    { return this->rNCarr; }
    FDATA              GetRecPCarr()    { return this->rPCarr; }
    std::vector<INDEX> GetIndex()
    { return std::vector<INDEX>(this->IndexX, this->IndexY); }
    Dict<std::string, FDATA> GetMaterial()
    { return this->Material; }
    std::vector<std::unique_ptr<Node<FDATA,INDEX>>> GetAdjNodeList()
    { return this->AdjNodeList; }

    // Data set up stuff
    void SetLocation(std::vector<FDATA> vec_loc_data)
    { this->Location = vec_loc_data; }
    void SetPotential(std::vector<FDATA> vec_pot_data)
    { this->Potential = vec_pot_data; }
    void SetEField(std::vector<FDATA> vec_field_data)
    { this->EField = vec_field_data; }
    void SetCurrent(std::vector<FDATA> vec_current_data)
    { this->Current = vec_current_data; }
    void SetNDopConc(FDATA n_dop_conc)
    { this->nDopConc = n_dop_conc; }
    void SetPDopConc(FDATA p_dop_conc)
    { this->pDopConc = p_dop_conc; }
    void SetNCarr(FDATA n_conc)
    { this->nNCarr = n_conc; }
    void SetPCarr(FDATA p_conc)
    { this->nPCarr = p_conc; }
    void SetGenNCarr(FDATA n_carr_gen)
    { this->gNCarr = n_carr_gen; }
    void SetGenPCarr(FDATA p_carr_gen)
    { this->gPCarr = p_carr_gen; }
    void SetRecNCarr(FDATA n_carr_rec)
    { this->rNCarr = n_carr_rec; }
    void SetRecPCarr(FDATA p_carr_rec)
    { this->rPCarr = p_carr_rec; }
    void SetIndex(std::vector<INDEX> new_index)
    { this->IndexX = new_index[0]; this->IndexY = new_index[1]; }
    void SetMaterial(Dict<std::string, FDATA> new_mat)
    { this->Material = new_mat; }
    void SetAdjNodeList(
        std::vector<std::unique_ptr<Node<FDATA,INDEX>>> new_adj_node_list)
    { this->AdjNodeList = new_adj_node_list; }

    // Adjacent node list manipulation
    bool AddNode(std::unique_ptr<Node<FDATA,INDEX>> new_node);
    bool Attach(std::unique_ptr<Node<FDATA,INDEX>> new_node);
    bool RemoveNode(std::unique_ptr<Node<FDATA,INDEX>> a_node);
    bool Detach(std::unique_ptr<Node<FDATA,INDEX>> a_node);
    bool FindNode(std::unique_ptr<Node<FDATA,INDEX>> a_node);

    // Setup Dimension
    bool SetDim();

    /**
     * Constructors and Destructors
     *
    **/
    // Construct an isolated single node.
    Node() : 
        Location(std::vector<FDATA>({0.0,0.0})),
        Potential(std::vector<FDATA>({0.0,0.0})),
        EField(std::vector<FDATA>({0.0,0.0})),
        Current(std::vector<FDATA>({0.0,0.0})),
        nDopConc(0.0), pDopConc(0.0),
        nNCarr(0.0), nPCarr(0.0),
        gNCarr(0.0), gPCarr(0.0),
        rNCarr(0.0), rPCarr(0.0),
        IndexX(0), IndexY(0),
        AdjNodeList(std::vector<std::unique_ptr<Node<FDATA,INDEX>>>())
    {
        // Then set up dimension variable
        SetDim();
    }

    // Construct a node with a set of scalar data... (2D initialization)
    Node(
        FDATA LocationX, FDATA LocationY,
        FDATA PotentialX, FDATA PotentialY,
        FDATA EFieldX, FDATA EFieldY,
        FDATA CurrX, FDATA CurrY,
        FDATA dop_conc_n, FDATA dop_conc_p,
        FDATA num_NCarr, FDATA num_PCarr,
        FDATA gen_NCarr, FDATA gen_PCarr,
        FDATA rec_NCarr, FDATA rec_PCarr,
        INDEX IndX, INDEX IndY) : 
        Location(std::vector<FDATA>({LocationX, LocationY})),
        Potential(std::vector<FDATA>({PotentialX, PotentialY})),
        EField(std::vector<FDATA>({EFieldX, EFieldY})),
        Current(std::vector<FDATA>({CurrX, CurrY})),
        nDopConc(dop_conc_n), pDopConc(dop_conc_p),
        nNCarr(num_NCarr), nPCarr(num_PCarr),
        gNCarr(gen_NCarr), gPCarr(gen_PCarr),
        rNCarr(rec_NCarr), rPCarr(rec_PCarr),
        IndexX(IndX), IndexY(IndY),
        AdjNodeList(std::vector<std::unique_ptr<Node<FDATA,INDEX>>>())
    {
        // Obviously, it's a 2D initializer!!
        this->Dimension = 2;
    }

    // Construct a node with vectors.
    // Now we can make even 5D node... errr, joke..
    Node(
        std::vector<FDATA> vec_loc_data,
        std::vector<FDATA> vec_pot_data,
        std::vector<FDATA> vec_field_data,
        std::vector<FDATA> vec_current_data,
        FDATA dop_conc_n, FDATA dop_conc_p,
        FDATA num_NCarr, FDATA num_PCarr,
        FDATA gen_NCarr, FDATA gen_PCarr,
        FDATA rec_NCarr, FDATA rec_PCarr,
        INDEX IndX, INDEX IndY) : 
        Location(vec_loc_data),
        Potential(vec_pot_data),
        EField(vec_field_data),
        Current(vec_current_data),
        nDopConc(dop_conc_n), pDopConc(dop_conc_p),
        nNCarr(num_NCarr), nPCarr(num_PCarr),
        gNCarr(gen_NCarr), gPCarr(gen_PCarr),
        rNCarr(rec_NCarr), rPCarr(rec_PCarr),
        IndexX(IndX), IndexY(IndY),
        AdjNodeList(std::vector<std::unique_ptr<Node<FDATA,INDEX>>>())
    {
        SetDim();
    }

    // Destroy this node!!
    virtual ~Node() {;}
};

} // namespace pdTCAD

#endif /* Include guard */
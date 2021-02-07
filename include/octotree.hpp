#ifndef OCTOTREE_HPP
#define OCTOTREE_HPP

#include <iostream>
#include <cassert>
#include <vector>
#include <include/datatypes.hpp>

using namespace std;

const int EMPTY = 0;
const int BOUND = 1;
const int FILLED = 2;

vector<vec> steps = {vec(0, 0, 1), vec(0, 0, -1), vec(1, 0, 0), vec(-1, 0, 0), vec(0, 1, 0), vec(0, -1, 0)};

class Octo
{
public:
    Octo* parent = nullptr;
    vector<Octo> children;
    Voxel voxel;

    int type = EMPTY;

    Octo()
    {

    }

    Octo(vec pos, int size)
    {
        voxel.setPosition(pos);
        voxel.setSize(size);
    }

    void buildChildren()
    {
        int halfSize = voxel.size / 2;
        children.reserve(8);

        for (int i = 0; i < 8; i++)
        {
            int sx = (i % 2);
            int sy = ((i / 2) % 2);
            int sz = ((i / 4) % 2);

            children.push_back(Octo(vec(voxel.pos.x + sx * halfSize, voxel.pos.y + sy * halfSize, voxel.pos.z + sz * halfSize), halfSize));
            children[i].parent = this;
        }
    }

    bool exist(vec pos)
    {
        if (voxel.size == 1)
            return type != EMPTY;

        if (children.size() == 0)
            return false;

        for (auto& child : children)
            if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
                return child.exist(pos);
        
        assert(("Impossible situation, problems with accessing position in octotree", 1));
        return false;
    }

    void put(vec pos)
    {
        if (voxel.size == 1)
        {
            type = BOUND;
            return;
        }

        if (children.size() == 0)
            buildChildren();
        
        for (auto& child : children)
        {
            if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
            {
                type = BOUND;
                child.put(pos);
                return;
            }
        }
    }

    void updateType(vec pos, int type)
    {
        if (voxel.size == 1)
        {
            this->type = type;
            return;
        }

        if (children.size() == 0)
            return;

        for (auto& child : children)
        {
            if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
            {
                child.updateType(pos, type);
                return;
            }
        }
    }
};

void updateFillingOnce(Octo& tree, vec pos)
{
    for (vec& d : steps)
        if (!tree.exist(pos + d))
            return;

    tree.updateType(pos, FILLED);
}

void put(Octo& tree, vec pos)
{
    tree.put(pos);
    updateFillingOnce(tree, pos);

    for (vec& d : steps)
        updateFillingOnce(tree, pos + d);
}

void buildOcto(Octo& tree, int depth)
{
    if (depth <= 0)
    {
        tree.type = BOUND;
        return;
    }

    tree.buildChildren();

    for (Octo& leaf : tree.children)
        buildOcto(leaf, depth - 1);
}

void parseOcto(Octo& tree, vector<Voxel>& result, int depth = -1)
{
    if (tree.children.size() == 0 || depth == 0)
    {
        result.push_back(tree.voxel);
        return;
    }

    for (Octo& leaf : tree.children)
        parseOcto(leaf, result, depth - 1);
}

#endif
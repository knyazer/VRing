#ifndef OCTOTREE_HPP
#define OCTOTREE_HPP

#include <iostream>
#include <cassert>
#include <vector>
#include <set>
#include <include/datatypes.hpp>

using namespace std;

vector<vec> steps = {vec(1, 0, 0), vec(-1, 0, 0), vec(0, 1, 0), vec(0, -1, 0), vec(0, 0, 1), vec(0, 0, -1)};

class OctoNode
{
public:
    OctoNode* parent = nullptr;
    vector<OctoNode> children;
    Voxel voxel;

    bool isExist = false;

    OctoNode()
    {

    }

    OctoNode(vec pos, int size)
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

            children.push_back(OctoNode(vec(voxel.pos.x + sx * halfSize, voxel.pos.y + sy * halfSize, voxel.pos.z + sz * halfSize), halfSize));
            children[i].parent = this;
        }
    }

    bool exist(vec pos)
    {
        if (voxel.size == 1)
            return isExist;

        if (children.size() == 0)
            return false;

        for (auto& child : children)
            if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
                return child.exist(pos);
        
        assert(("Problems with access to position in octree, children are not filled", 1));
        return false;
    }

    void put(vec pos)
    {
        if (voxel.size == 1)
        {
            isExist = 1;
            return;
        }

        if (children.size() == 0)
            buildChildren();
        
        for (auto& child : children)
        {
            if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
            {
                isExist = 1;
                child.put(pos);
                return;
            }
        }
    }

    void updateConnection(vec pos, connection_t connection)
    {
        if (voxel.size == 1)
        {
            this->voxel.connection = connection;
            return;
        }

        if (children.size() == 0)
            return;

        for (auto& child : children)
        {
            if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
            {
                child.updateConnection(pos, connection);
                return;
            }
        }
    }

    OctoNode* at(vec pos)
    {
        if (voxel.size == 1)
            return this;
        
        if (children.size() == 0)
        {
            assert(("Attempt to access uninitialized memory", 1));
            return nullptr;
        }

        for (auto& child : children)
            if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
                return child.at(pos);
        
        assert(("Problems with access to position in octree, children are not filled", 1));
        return nullptr;
    }
};

class Octree
{
public:
    OctoNode root;

    Octree()
    {

    }

    Octree(vec pos, int size)
    {
        root = OctoNode(pos, size);
    }

    void updateConnectionAt(OctoNode* base)
    {
        if (base == nullptr)
            return;

        vector<vec> lsteps = steps;
        int used = 0;

        connection_t connection = 0;

        OctoNode* currentNode = base->parent;
        while (used != lsteps.size())
        {
            if (currentNode == nullptr)
                break;
            
            for (uint8_t i = 0; i < lsteps.size(); i++)
            {
                auto& step = lsteps[i];
                if (step == vec(0, 0, 0))
                    continue;
                
                vec pos = base->voxel.pos + step;
                for (auto& child : currentNode->children)
                {
                    if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
                    {
                        if (child.exist(pos))
                            connection += (1 << i);

                        step = vec(0, 0, 0);
                        used++;
                        break;
                    }
                }
            }

            currentNode = currentNode->parent;
        }

        base->voxel.connection = connection;

        //cout << connection << endl;
    }

    void put(vec pos)
    {
        root.put(pos);
    }

    void updateConnections()
    {
        updateConnections(&root);
    }

private:
    void updateConnections(OctoNode* node)
    {
        if (node->voxel.size == 1)
        {
            updateConnectionAt(node);
            return;
        }

        if (node->children.size() == 0)
            return;
        
        for (auto& child : node->children)
            updateConnections(&child);
    }
};

#endif
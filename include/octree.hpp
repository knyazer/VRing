#ifndef OCTOTREE_HPP
#define OCTOTREE_HPP

#include <iostream>
#include <cassert>
#include <vector>
#include <queue>
#include <include/datatypes.hpp>

using namespace std;

vector<vec> steps = {vec(1, 0, 0), vec(-1, 0, 0), vec(0, 1, 0), vec(0, -1, 0), vec(0, 0, 1), vec(0, 0, -1)};

class OctoNode
{
public:
    OctoNode* parent = nullptr;
    vector<OctoNode> children;
    Voxel voxel;

    int type = EMPTY;

    OctoNode()
    {

    }

    OctoNode(vec& pos, int size)
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
            int sx = (i & 1);
            int sy = ((i >> 1) & 1);
            int sz = ((i >> 2) & 1);

            children.push_back(OctoNode(vec(voxel.pos.x + sx * halfSize, voxel.pos.y + sy * halfSize, voxel.pos.z + sz * halfSize), halfSize));
            children[i].parent = this;
        }
    }

    int filling(vec& pos, int size = 1)
    {
        if (voxel.size == size)
            return type;

        if (voxel.size < size)
            return parent->filling(pos, size);

        if (children.size() == 0)
        {
            if (type == PARTIALLY_FILLED)
                return EMPTY;

            return type;
        }

        for (auto& child : children)
            if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
            {
                if (child.type == EMPTY)
                    return EMPTY;
                else if (child.type == FILLED)
                    return FILLED;
                else
                    return child.filling(pos);
            }
        
        assert(("Problems with access to position in octree, children are not filled", 1));
        return EMPTY;
    }

    void put(vec& pos)
    {
        type = PARTIALLY_FILLED;

        if (voxel.size == 1)
        {
            type = FILLED;
            return;
        }

        if (children.size() == 0)
            buildChildren();
        
        for (auto& child : children)
        {
            if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
            {
                child.put(pos);
                return;
            }
        }
    }

    void updateConnection(vec& pos, connection_t connection)
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

    OctoNode* at(vec& pos)
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

    Octree(vec& pos, int size)
    {
        size = 1 << size;
        root = OctoNode(pos - vec(size / 2, size / 2, size / 2), size);
    }

    void updateConnectionAt(OctoNode* base)
    {
        if (base == nullptr)
            return;

        vector<vec> lsteps = steps;
        int used = 0;
        int size = base->voxel.size;

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
                
                vec& pos = base->voxel.pos + step * size;
                for (auto& child : currentNode->children)
                {
                    if (pos >= child.voxel.pos && pos < (child.voxel.pos + child.voxel.size))
                    {
                        if (child.filling(pos, size) == FILLED)
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
    }

    void put(vec& pos)
    {
        root.put(pos);
    }

    void updateConnections()
    {
        updateConnections(&root);
    }

    OctoNode* at(vec& pos)
    {
        return root.at(pos);
    }

private:
    void updateConnections(OctoNode* node)
    {
        if (node->type == EMPTY)
            return;

        updateConnectionAt(node);

        if (node->voxel.size == 1)
            return;

        if (node->children.size() == 0)
            return;
        
        for (auto& child : node->children)
            updateConnections(&child);
    }
};

const double hsqrt3 = sqrt(3) / 2;

class Sphere
{
public:
    int r;
    vec pos;

    Sphere(vec& pos, int r)
    {
        this->pos = pos;
        this->r = r;
    }

    bool in(vec& a)
    {
        return (a - pos).sqlength() < sq(r);
    }

    int checkCube(vec& origin, float size)
    {
        vec center = origin + vec(size / 2, size / 2, size / 2);
        if ((center - pos).sqlength() > sq(hsqrt3 * size + r))
            return EMPTY;
        
        for (int i = 0; i < 8; i++)
        {
            int xm = i & 1, ym = (i >> 1) & 1, zm = (i >> 2) & 1;

            if (!in(origin + vec(xm, ym, zm) * size))
                return PARTIALLY_FILLED;
        }


        return FILLED;
    }


    void updateConnectionAt(OctoNode* node)
    {
        if (node == nullptr)
            return;
        connection_t connection = NO_CONNECTIONS;
        for (int i = 0; i < steps.size(); i++)
            if ((checkCube(node->voxel.pos + (steps[i] * node->voxel.size), node->voxel.size) == FILLED && node->voxel.size != 1) || 
                (checkCube(node->voxel.pos + (steps[i] * node->voxel.size), node->voxel.size) != EMPTY && node->voxel.size == 1))
                connection += (1 << i);
        node->voxel.connection = connection;
    }

    void put(Octree& oct)
    {
        oct.root.type = PARTIALLY_FILLED;

        queue<OctoNode*> q;
        q.push(&oct.root);
        while (!q.empty())
        {
            OctoNode* node = q.front();
            q.pop();

            if (node->voxel.size == 1)
            {
                node->type = FILLED;
                continue;
            }

            if (node->children.size() == 0)
                node->buildChildren();

            for (OctoNode& child : node->children)
            {
                int res = checkCube(child.voxel.pos, child.voxel.size);

                if (res == EMPTY)
                    continue;
                
                updateConnectionAt(&child);
                
                child.type = res;

                if (child.voxel.connection != FULLY_CONNECTED)
                    q.push(&child);
            }
        }
    }
};

#endif
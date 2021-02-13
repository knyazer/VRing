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

    int filling(vec pos)
    {
        if (voxel.size == 1)
            return type;

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

    void put(vec pos)
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
        size = 1 << size;
        root = OctoNode(pos - vec(size / 2, size / 2, size / 2), size);
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
                        if (child.filling(pos) == FILLED)
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

class Sphere
{
public:
    int r;
    vec pos;

    Sphere(vec pos, int r)
    {
        this->pos = pos;
        this->r = r;
    }

    bool in(vec a)
    {
        a = a - pos;
        return a.sqlength() < isq(r);
    }

    int checkCube(vec origin, int size)
    {
        if ((origin + vec(size / 2, size / 2, size / 2) - pos).sqlength() > isq(size + r))
            return 0;
        
        bool ans = true;
        for (int i = 0; i < 8; i++)
        {
            int xm = i & 1, ym = (i >> 1) & 1, zm = (i >> 2) & 1;
            vec p = origin + vec(size * xm, size * ym, size * zm);
            ans &= in(p);
        }

        return ans ? 2 : 1;
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

            for (auto& child : node->children)
            {
                int res = checkCube(child.voxel.pos, child.voxel.size);

                if (res == 0)
                    continue;

                if (child.type == EMPTY)
                    child.type = PARTIALLY_FILLED;

                if (res == 1)
                    q.push(&child);
                
                if (res == 2)
                    child.type = FILLED;
            }
        }
    }
};

#endif
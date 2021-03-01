#ifndef VLIST_HPP
#define VLIST_HPP

#include <include/datatypes.hpp>

#define LEN 100000000LL
#define DATA_SIZE 4
int32_t g_data[LEN];

class Cell
{
public:
    Cell(){}
    
    void put(vec &v, int conn)
    {
        g_data[ptr + 0] = v.x;
        g_data[ptr + 1] = v.y;
        g_data[ptr + 2] = v.z;
        g_data[ptr + 3] = conn;
    }
    
    int ptr;

    Cell* prev;
    Cell* next;
};

class VList
{
public:
    VList(){}

    void remove(Cell* cell)
    {
        size--;

        if (cell == end)
        {
            end = end->prev;
            delete cell;
            return;
        }

        cell->next->prev = end; // replace cell by end as parent
        cell->prev->next = end; // replace cell by end as kid

        end->prev->next = nullptr; // clear memory of the end
        end = end->prev; // replace the end buy its parent

        delete cell;
    }

    void add(Cell* cell)
    {
        size++;

        if (begin == nullptr)
        {
            begin = cell;
            end = cell;
            cell->ptr = 0;
            return;
        }

        cell->ptr = end->ptr + DATA_SIZE;

        end->next = cell; // replace the end
        cell->prev = end;

        end = end->next;

    }

    Cell* begin = nullptr;
    Cell* end = nullptr;

    int size = 0;
};

VList g_vlist;

#endif
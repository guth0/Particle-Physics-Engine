#include <iostream>

struct CollisionCell
{
    static constexpr uint8_t cell_capacity = 4;
    static constexpr uint8_t max_cell_idx = cell_capacity - 1;

    uint32_t objects_count = 0;
    uint32_t objects[cell_capacity] = {};

    CollisionCell() = default;

    void addObject(uint32_t index)
    {

        objects[objects_count] = index;
        objects_count += objects_count <= max_cell_idx;
    }

    void clear()
    {
        objects_count = 0u;
    }

    void remove(uint32_t id)
    {
        for (uint32_t i{0}; i < objects_count; ++i)
        {
            if (objects[i] == id)
            {
                // Swap pop
                objects[i] = objects[objects_count - 1];
                --objects_count;
                return;
            }
        }
    }
};

struct CollisionGrid
{
    std::vector<std::vector<CollisionCell>> data;
    uint32_t height;
    uint32_t width;
    uint16_t cell_size;

    CollisionGrid() = default;

    CollisionGrid(uint32_t width_, uint16_t height_, uint16_t cell_size_)
    {
        data.resize(width_, std::vector<CollisionCell>(height_));
        height = height_;
        width = width_;
        cell_size = cell_size_;
    }

    void setSize(uint32_t width_, uint32_t height_, uint16_t cell_size_)
    {
        data.resize(width_, std::vector<CollisionCell>(height_));
        height = height_;
        width = width_;
        cell_size = cell_size_;
    }

    bool addObject(uint32_t x, uint32_t y, uint32_t index)
    {
        const uint16_t cellX = floor(x / cell_size);
        const uint16_t cellY = floor(y / cell_size);

        // Add to grid

        // std::cout << cellX << ',' << cellY << std::endl;

        data[cellX][cellY].addObject(index);
        return true;
    }

    void clear()
    {
        for (auto &c : data)
        {
            for (auto &r : c)
                r.clear();
        }
    }
};
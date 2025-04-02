#ifndef UTILS_QUADTREE_QUADTREE_NODE_HPP
#define UTILS_QUADTREE_QUADTREE_NODE_HPP

#include <vector>

struct Point {
    double x, y;
};

// Bounding box for quadtree nodes
struct AABB {
    double x, y, width, height;

    bool contains(const Point &p) const;
};

class QuadTreeNode {
private:
    static constexpr int CAPACITY = 4;
    AABB boundary;
    std::vector<Point> points;
    bool divided = false;

    QuadTreeNode *northwest = nullptr;
    QuadTreeNode *northeast = nullptr;
    QuadTreeNode *southwest = nullptr;
    QuadTreeNode *southeast = nullptr;

public:
    QuadTreeNode(AABB boundary);
    ~QuadTreeNode();
    void subdivide();
    bool insert(Point p);
};

#endif
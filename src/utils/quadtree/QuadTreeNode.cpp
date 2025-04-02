#include "QuadTreeNode.hpp"
#include <iostream>

bool AABB::contains(const Point &p) const {
    return (p.x >= x - width && p.x <= x + width &&
            p.y >= y - height && p.y <= y + height);
}

QuadTreeNode::QuadTreeNode(AABB boundary) : boundary(boundary) {}

QuadTreeNode::~QuadTreeNode() {
    delete northwest;
    delete northeast;
    delete southwest;
    delete southeast;
}

void QuadTreeNode::subdivide() {
    double x = boundary.x;
    double y = boundary.y;
    double w = boundary.width / 2;
    double h = boundary.height / 2;

    northwest = new QuadTreeNode({x - w, y - h, w, h});
    northeast = new QuadTreeNode({x + w, y - h, w, h});
    southwest = new QuadTreeNode({x - w, y + h, w, h});
    southeast = new QuadTreeNode({x + w, y + h, w, h});
    divided = true;
}

bool QuadTreeNode::insert(Point p) {
    if (!boundary.contains(p)) {
        return false;
    }

    if (points.size() < CAPACITY) {
        points.push_back(p);
        return true;
    }

    if (!divided) {
        subdivide();
    }

    if (northwest->insert(p) || northeast->insert(p) ||
        southwest->insert(p) || southeast->insert(p)) {
        return true;
    }

    return false;
}
#pragma once
#include "Core/Core.h"

#include <any>

namespace Rocket
{
    // TODO : use different namespace in different module
    // namespace Geometry

    struct SpatialRect
    {
    };

    class QuadTreeNode
    {
    public:
        void AddParent(const Ref<QuadTreeNode>& parent) { parent_ = parent; }
        void AddChild(const Ref<QuadTreeNode>& child, int index)
        {
            RK_CORE_ASSERT(index < 4, "quad tree child index error"); 
            children_[index] = child;
        }
        Ref<QuadTreeNode> GetParent() const { return parent_; }
        Ref<QuadTreeNode> GetChild(int index) const 
        { 
            RK_CORE_ASSERT(index < 4, "quad tree child index error"); 
            return children_[index]; 
        }
    private:
        Ref<QuadTreeNode> parent_;
        // NE NW SW SE
        Array<Ref<QuadTreeNode>, 4> children_;
        Vec<Ref<SpatialRect>> objects_;
        Vec<Ref<SpatialRect>> found_objects_;
        std::any data_;
    };

    class QuadTree
    {
    public:
    private:
        Ref<QuadTreeNode> root_;
    };
}
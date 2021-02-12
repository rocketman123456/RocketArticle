#pragma once
#include "Core/Core.h"
#include "Module/MemoryManager.h"

//#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Eigen>
#include <unordered_set>
#include <utility>

namespace Rocket
{
    using Vector2f = Eigen::Vector2f;
    using Vector3f = Eigen::Vector3f;
    using Vector4f = Eigen::Vector4f;
    using Matrix2f = Eigen::Matrix2f;
    using Matrix3f = Eigen::Matrix3f;
    using Matrix4f = Eigen::Matrix4f;

    using Vector2d = Eigen::Vector2d;
    using Vector3d = Eigen::Vector3d;
    using Vector4d = Eigen::Vector4d;
    using Matrix2d = Eigen::Matrix2d;
    using Matrix3d = Eigen::Matrix3d;
    using Matrix4d = Eigen::Matrix4d;

    using Quaternionf = Eigen::Quaternionf;
    using Quaterniond = Eigen::Quaterniond;

    using AngleAxisf = Eigen::AngleAxisf;
    using AngleAxisd = Eigen::AngleAxisd;

    using Point2D = Eigen::Vector2f;
    using Point2DPtr = Ref<Point2D>;
    using Point2DList = Vec<Point2DPtr>;
    using Point2DSet = USet<Point2DPtr>;

    using Point3D = Eigen::Vector3f;
    using Point3DPtr = Ref<Point3D>;
    using Point3DList = Vec<Point3DPtr>;
    using Point3DSet = USet<Point3DPtr>;

    using Edge = std::pair<Point3DPtr, Point3DPtr>;
    using EdgePtr = Ref<Edge>;
    using EdgeList = Vec<EdgePtr>;
    using EdgeSet = USet<EdgePtr>;

    struct Face
    {
        EdgeList    Edges;
        Vector3f    Normal;
        Point3DList GetVertices() const 
        {
            Point3DList vertices;
            for (auto edge : Edges)
            {
                vertices.push_back(edge->first);
            }
            return vertices;
        }
    };

    using FacePtr = Ref<Face>;
    using FaceList = Vec<FacePtr>;
    using FaceSet = USet<FacePtr>;
}

#pragma once
#include <Eigen/Eigen>
#include <memory>
#include <vector>
#include <unordered_set>
#include <utility>

namespace Rocket
{
    using Vector2f = Eigen::Vector2f;
    using Vector3f = Eigen::Vector3f;
    using Vector4f = Eigen::Vector4f;
    using Matrix2f = Eigen::Matrix2d;
    using Matrix3f = Eigen::Matrix3d;
    using Matrix4f = Eigen::Matrix4d;

    using Point2D = Eigen::Vector2f;
    using Point2DPtr = std::shared_ptr<Point2D>;
    using Point2DList = std::vector<Point2DPtr>;
    using Point2DSet = std::unordered_set<Point2DPtr>;

    using Point3D = Eigen::Vector3f;
    using Point3DPtr = std::shared_ptr<Point3D>;
    using Point3DList = std::vector<Point3DPtr>;
    using Point3DSet = std::unordered_set<Point3DPtr>;

    using Edge = std::pair<Point3DPtr, Point3DPtr>;
    using EdgePtr = std::shared_ptr<Edge>;
    using EdgeList = std::vector<EdgePtr>;
    using EdgeSet = std::unordered_set<EdgePtr>;

    struct Face {
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
    using FacePtr = std::shared_ptr<Face>;
    using FaceList = std::vector<FacePtr>;
    using FaceSet = std::unordered_set<FacePtr>;
}
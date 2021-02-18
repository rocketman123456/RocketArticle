#pragma once
#include "Core/Core.h"

//#define EIGEN_DONT_ALIGN_STATICALLY
#include <Eigen/Eigen>
#include <unordered_set>
#include <utility>

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
using Point2DPtr = Rocket::Ref<Point2D>;
using Point2DList = Rocket::Vec<Point2DPtr>;
using Point2DSet = Rocket::USet<Point2DPtr>;

using Point3D = Eigen::Vector3f;
using Point3DPtr = Rocket::Ref<Point3D>;
using Point3DList = Rocket::Vec<Point3DPtr>;
using Point3DSet = Rocket::USet<Point3DPtr>;

using Edge = std::pair<Point3DPtr, Point3DPtr>;
using EdgePtr = Rocket::Ref<Edge>;
using EdgeList = Rocket::Vec<EdgePtr>;
using EdgeSet = Rocket::USet<EdgePtr>;

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

using FacePtr = Rocket::Ref<Face>;
using FaceList = Rocket::Vec<FacePtr>;
using FaceSet = Rocket::USet<FacePtr>;

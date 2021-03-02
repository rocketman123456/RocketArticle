//
// Created by goksu on 4/6/19.
//

#pragma once
#include "Core/Core.h"
#include "Common/GeomMath.h"
#include "Triangle.h"
#include <algorithm>
#include <Eigen/Eigen>
using namespace Eigen;

namespace rst {
    enum class Buffers
    {
        Color = 1,
        Depth = 2
    };

    inline Buffers operator|(Buffers a, Buffers b)
    {
        return Buffers((int)a | (int)b);
    }

    inline Buffers operator&(Buffers a, Buffers b)
    {
        return Buffers((int)a & (int)b);
    }

    enum class Primitive
    {
        Line,
        Triangle
    };

    /*
     * For the curious : The draw function takes two buffer id's as its arguments.
     * These two structs make sure that if you mix up with their orders, the
     * compiler won't compile it. Aka : Type safety
     * */
    struct pos_buf_id
    {
        int pos_id = 0;
    };

    struct ind_buf_id
    {
        int ind_id = 0;
    };

    class Rasterizer
    {
    public:
        Rasterizer(int w, int h);
        pos_buf_id load_positions(const std::vector<Vector3f>& positions);
        ind_buf_id load_indices(const std::vector<Eigen::Vector3i>& indices);

        void set_model(const Matrix4f& m);
        void set_view(const Matrix4f& v);
        void set_projection(const Matrix4f& p);

        void set_pixel(const Vector3f& point, const Vector3f& color);

        void clear(Buffers buff);

        void draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, Primitive type);

        std::vector<Vector3f>& frame_buffer() { return frame_buf; }

    private:
        void draw_line(Vector3f begin, Vector3f end);
        void rasterize_wireframe(const Triangle& t);

    private:
        Matrix4f model;
        Matrix4f view;
        Matrix4f projection;

        std::map<int, std::vector<Vector3f>> pos_buf;
        std::map<int, std::vector<Eigen::Vector3i>> ind_buf;

        std::vector<Vector3f> frame_buf;
        std::vector<float> depth_buf;
        int get_index(int x, int y);

        int width, height;

        int next_id = 0;
        int get_next_id() { return next_id++; }
    };
} // namespace rst

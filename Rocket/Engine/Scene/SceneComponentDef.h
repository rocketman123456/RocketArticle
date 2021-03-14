#pragma once
#include "Core/Core.h"
#include "Utils/Hashing.h"

namespace Rocket
{
    ENUM(SceneComponentType)
    {
        kSceneComponentTypeMesh = "MESH"_i32,
        kSceneComponentTypeMaterial = "MATL"_i32,
        kSceneComponentTypeTexture = "TXTU"_i32,
        kSceneComponentTypeLightOmni = "LGHO"_i32,
        kSceneComponentTypeLightInfi = "LGHI"_i32,
        kSceneComponentTypeLightSpot = "LGHS"_i32,
        kSceneComponentTypeLightArea = "LGHA"_i32,
        kSceneComponentTypeCamera = "CAMR"_i32,
        kSceneComponentTypeAnimationClip = "ANIM"_i32,
        kSceneComponentTypeClip = "CLIP"_i32,
        kSceneComponentTypeVertexArray = "VARR"_i32,
        kSceneComponentTypeIndexArray = "VARR"_i32,
        kSceneComponentTypeGeometry = "GEOM"_i32,
        kSceneComponentTypeTransform = "TRFM"_i32,
        kSceneComponentTypeTranslate = "TSLT"_i32,
        kSceneComponentTypeRotate = "ROTA"_i32,
        kSceneComponentTypeScale = "SCAL"_i32,
        kSceneComponentTypeTrack = "TRAC"_i32,
        kSceneComponentTypeSkyBox = "SKYB"_i32,
        kSceneComponentTypeTerrain = "TERN"_i32
    };

    ENUM(SceneComponentCollisionType)
    {
        kSceneComponentCollisionTypeNone = "CNON"_i32,
        kSceneComponentCollisionTypeSphere = "CSPH"_i32,
        kSceneComponentCollisionTypeBox = "CBOX"_i32,
        kSceneComponentCollisionTypeCylinder = "CCYL"_i32,
        kSceneComponentCollisionTypeCapsule = "CCAP"_i32,
        kSceneComponentCollisionTypeCone = "CCON"_i32,
        kSceneComponentCollisionTypeMultiSphere = "CMUL"_i32,
        kSceneComponentCollisionTypeConvexHull = "CCVH"_i32,
        kSceneComponentCollisionTypeConvexMesh = "CCVM"_i32,
        kSceneComponentCollisionTypeBvhMesh = "CBVM"_i32,
        kSceneComponentCollisionTypeHeightfield = "CHIG"_i32,
        kSceneComponentCollisionTypePlane = "CPLN"_i32,
    };

    ENUM(IndexDataType)
    {
        kIndexDataTypeInt8 = "I8  "_i32,
        kIndexDataTypeInt16 = "I16 "_i32,
        kIndexDataTypeInt32 = "I32 "_i32,
        kIndexDataTypeInt64 = "I64 "_i32,
    };

    ENUM(VertexDataType)
    {
        kVertexDataTypeFloat1 = "FLT1"_i32,  kVertexDataTypeFloat2 = "FLT2"_i32,
        kVertexDataTypeFloat3 = "FLT3"_i32,  kVertexDataTypeFloat4 = "FLT4"_i32,
        kVertexDataTypeDouble1 = "DUB1"_i32, kVertexDataTypeDouble2 = "DUB2"_i32,
        kVertexDataTypeDouble3 = "DUB3"_i32, kVertexDataTypeDouble4 = "DUB4"_i32
    };

    ENUM(PrimitiveType)
    {
        kPrimitiveTypeNone = "NONE"_i32,  ///< No particular primitive type.
        kPrimitiveTypePointList = "PLST"_i32,  ///< For N>=0, vertex N renders a point.
        kPrimitiveTypeLineList = "LLST"_i32,  ///< For N>=0, vertices [N*2+0, N*2+1] render a line.
        kPrimitiveTypeLineStrip = "LSTR"_i32,  ///< For N>=0, vertices [N, N+1] render a line.
        kPrimitiveTypeTriList = "TLST"_i32,  ///< For N>=0, vertices [N*3+0, N*3+1,
                                             ///< N*3+2] render a triangle.
        kPrimitiveTypeTriFan = "TFAN"_i32,  ///< For N>=0, vertices [0, (N+1)%M, (N+2)%M] render a
                                            ///< triangle, where M is the vertex count.
        kPrimitiveTypeTriStrip = "TSTR"_i32,  ///< For N>=0, vertices [N*2+0, N*2+1, N*2+2] and [N*2+2,
                                              ///< N*2+1, N*2+3] render triangles.
        kPrimitiveTypePatch = "PACH"_i32,  ///< Used for tessellation.
        kPrimitiveTypeLineListAdjacency = "LLSA"_i32,  ///< For N>=0, vertices [N*4..N*4+3] render a line from [1,
                                                       ///< 2]. Lines [0, 1] and [2, 3] are adjacent to the
                                                       ///< rendered line.
        kPrimitiveTypeLineStripAdjacency = "LSTA"_i32,  ///< For N>=0, vertices [N+1, N+2] render a line. Lines [N,
                                                        ///< N+1] and [N+2, N+3] are adjacent to the rendered line.
        kPrimitiveTypeTriListAdjacency = "TLSA"_i32,  ///< For N>=0, vertices [N*6..N*6+5] render a triangle from
                                                      ///< [0, 2, 4]. Triangles [0, 1, 2] [4, 2, 3] and [5, 0, 4]
                                                      ///< are adjacent to the rendered triangle.
        kPrimitiveTypeTriStripAdjacency = "TSTA"_i32,  ///< For N>=0, vertices [N*4..N*4+6] render a triangle from
                                                       ///< [0, 2, 4] and [4, 2, 6]. Odd vertices Nodd form
                                                       ///< adjacent triangles with indices min(Nodd+1,Nlast) and
                                                       ///< max(Nodd-3,Nfirst).
        kPrimitiveTypeRectList = "RLST"_i32,  ///< For N>=0, vertices [N*3+0, N*3+1, N*3+2] render a
                                              ///< screen-aligned rectangle. 0 is upper-left, 1 is
                                              ///< upper-right, and 2 is the lower-left corner.
        kPrimitiveTypeLineLoop = "LLOP"_i32,  ///< Like <c>kPrimitiveTypeLineStrip</c>, but the first and
                                              ///< last vertices also render a line.
        kPrimitiveTypeQuadList = "QLST"_i32,  ///< For N>=0, vertices [N*4+0, N*4+1, N*4+2] and [N*4+0,
                                              ///< N*4+2, N*4+3] render triangles.
        kPrimitiveTypeQuadStrip = "QSTR"_i32,  ///< For N>=0, vertices [N*2+0, N*2+1, N*2+3] and [N*2+0,
                                               ///< N*2+3, N*2+2] render triangles.
        kPrimitiveTypePolygon = "POLY"_i32,  ///< For N>=0, vertices [0, N+1, N+2] render a triangle.
    };

    std::ostream& operator<<(std::ostream& out, SceneComponentType type);
    std::ostream& operator<<(std::ostream& out, IndexDataType type);
    std::ostream& operator<<(std::ostream& out, VertexDataType type);
    std::ostream& operator<<(std::ostream& out, PrimitiveType type);
}
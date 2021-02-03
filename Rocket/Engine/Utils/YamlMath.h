#pragma once
#include "Common/GeomMath.h"

#include <yaml-cpp/yaml.h>

namespace YAML {
    template<>
    struct convert<Rocket::Vector2f>
    {
        static Node encode(const Rocket::Vector2f& rhs)
        {
            Node node;
            node.push_back(rhs[0]);
            node.push_back(rhs[1]);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, Rocket::Vector2f& rhs)
        {
            if(!node.IsSequence() || node.size() != 2)
            {
                return false;
            }

            rhs[0] = node[0].as<float>();
            rhs[1] = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Rocket::Vector3f>
    {
        static Node encode(const Rocket::Vector3f& rhs)
        {
            Node node;
            node.push_back(rhs[0]);
            node.push_back(rhs[1]);
            node.push_back(rhs[2]);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, Rocket::Vector3f& rhs)
        {
            if(!node.IsSequence() || node.size() != 3)
            {
                return false;
            }

            rhs[0] = node[0].as<float>();
            rhs[1] = node[1].as<float>();
            rhs[2] = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<Rocket::Vector4f>
    {
        static Node encode(const Rocket::Vector4f& rhs)
        {
            Node node;
            node.push_back(rhs[0]);
            node.push_back(rhs[1]);
            node.push_back(rhs[2]);
            node.push_back(rhs[3]);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, Rocket::Vector4f& rhs)
        {
            if(!node.IsSequence() || node.size() != 4)
            {
                return false;
            }

            rhs[0] = node[0].as<float>();
            rhs[1] = node[1].as<float>();
            rhs[2] = node[2].as<float>();
            rhs[3] = node[3].as<float>();
            return true;
        }
    };
}
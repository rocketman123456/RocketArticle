#pragma once
#include "Scene/SceneComponent.h"

namespace Rocket
{
    class TagName : implements SceneComponent
    {
    public:
        COMPONENT(TagName);
    public:
        TagName() = default;
        TagName(const TagName&) = default;
        TagName(const std::string & tag) : TagStr(tag) {}
        std::string TagStr;
    };
}
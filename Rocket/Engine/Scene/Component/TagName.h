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
        TagName(const String& tag) : tag_str(tag) {}
        virtual ~TagName() = default;

        String tag_str;
    };
}
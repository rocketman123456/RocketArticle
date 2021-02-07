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
        TagName(const String& tag) : TagStr(tag) {}
        virtual ~TagName() = default;

        String TagStr;
    };
}
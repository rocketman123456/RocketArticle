#include "Utils/GenerateName.h"

#include <crossguid/guid.hpp>

using namespace xg;
using namespace Rocket;

String GenerateName::Generate()
{
    auto id = newGuid();
	return id.str();
}
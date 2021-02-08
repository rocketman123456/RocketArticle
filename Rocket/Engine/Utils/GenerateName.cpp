#include "Utils/GenerateName.h"

#include <crossguid/guid.hpp>

using namespace xg;
using namespace Rocket;

String Rocket::GenerateName()
{
    auto id = newGuid();
	return id.str();
}
#pragma once
#include "Core/Core.h"
#include "Utils/Variant.h"

// TransferFunction uint64_t(const Vec<Variant>&, const uint64_t) // Action, State
// ActionFunction bool(const Vec<Variant>&, const Vec<Variant>&) // Input data, Target data

// Robot State Transfer function
uint64_t update_along_mat(const Rocket::Vec<Rocket::Variant>&, const uint64_t);

// Robot Action function list
bool action_on_edge(const Rocket::Vec<Rocket::Variant>&, const Rocket::Vec<Rocket::Variant>&);

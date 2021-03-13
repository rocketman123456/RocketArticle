#pragma once
#include "Core/Core.h"
#include "Utils/Variant.h"

// TransferFunction uint64_t(const Vec<Variant>&, const uint64_t) // Action, State
// ActionFunction bool(const Vec<Variant>&, const Vec<Variant>&) // Input data, Target data

// Robot State Transfer function list
uint64_t init_2_other(const Rocket::Vec<Rocket::Variant>&, const uint64_t);

uint64_t rot_00_2_other(const Rocket::Vec<Rocket::Variant>&, const uint64_t);
uint64_t rot_01_2_other(const Rocket::Vec<Rocket::Variant>&, const uint64_t);
uint64_t rot_02_2_other(const Rocket::Vec<Rocket::Variant>&, const uint64_t);

uint64_t move_00_2_other(const Rocket::Vec<Rocket::Variant>&, const uint64_t);
uint64_t move_01_2_other(const Rocket::Vec<Rocket::Variant>&, const uint64_t);
uint64_t move_02_2_other(const Rocket::Vec<Rocket::Variant>&, const uint64_t);
uint64_t move_03_2_other(const Rocket::Vec<Rocket::Variant>&, const uint64_t);
uint64_t move_04_2_other(const Rocket::Vec<Rocket::Variant>&, const uint64_t);
uint64_t move_05_2_other(const Rocket::Vec<Rocket::Variant>&, const uint64_t);

// Robot Action function list

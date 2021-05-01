//
// Created by Kirill Golubev on 01.05.2021.
//
#include "tile.h"

bool roguelike::tile::empty() const noexcept{ return not resident.has_value(); }

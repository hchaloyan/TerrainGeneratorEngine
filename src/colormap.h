#pragma once
#include <cstdint>

struct ColorStop { double t; uint8_t red, green, blue; };

static const ColorStop terrainStops[] = {
    { 0.00,   0,  50, 120 },  // deep blue
    { 0.05, 210, 185, 130 },  // sandy beige
    { 0.30,  50, 120,  50 },  // forest green
    { 0.60, 120,  90,  50 },  // brown
    { 1.00, 255, 255, 255 },  // white
};

static const int numTerrainStops = sizeof(terrainStops) / sizeof(terrainStops[0]);
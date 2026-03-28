#pragma once
#include <string.h>
#include "parser.h"



// outPath should be wherever you want the program to output the new png
// Function returns true on success, false on failure
bool renderGrayscale(const ElevationTile& tile, const std::string& outPath, std::string& err);
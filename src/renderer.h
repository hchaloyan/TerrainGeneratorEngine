#pragma once
#include <string>
#include "parser.h"
#include "colormap.h"
#include "hillshade.h"

// ---------------------------------------------------------------------------------


// outPath should be wherever you want the program to output the new png
// Functions return true on success, false on failure

bool renderColormap(const ElevationTile& tile, const std::string& outPath, std::string& err);

bool renderGrayscale(const ElevationTile& tile, const std::string& outPath, std::string& err);
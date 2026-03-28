#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third_party/stbImageWrite.h"
#include "renderer.h"
#include <algorithm>

bool renderGrayscale(
    const ElevationTile& tile, 
    const std::string& outPath,
    std::string& err
) {


    const auto& elevations = tile.elevations;
    const auto& header = tile.header;

    // Find min/max and ignore noData
    int16_t minimum = INT16_MAX;
    int16_t maximum = INT16_MIN;
    
    for (int16_t element : elevations) {
        
        // Skip noData cells
        if (element == header.noData) continue;
        
        
        minimum = std::min(minimum, element);
        maximum = std::max(maximum, element);
    }

    // Check whether there are elevation differences
    if (minimum == maximum) {
        err = "Elevation range is flat — cannot normalize";
        return false;
    }

    // Buffer
    std::vector<uint8_t> pixels(elevations.size());


    // Iterate through elevations vector
    for (size_t i = 0; i < elevations.size(); i++) {
        
        
        // Set noData to zero
        if (elevations[i] == header.noData) {
            pixels[i] = 0;
        } 
        // Otherwise, apply: 255 * (value - min) / (max - min)
        else {

            pixels[i] = static_cast<uint8_t>(
                255.0 * (elevations[i] - minimum) / (maximum - minimum)
            );
        
        }
    }

    // stbi_write_png returns 0 on failure
    // Write PNG using stb, and error check
    if (stbi_write_png(outPath.c_str(), header.nCols, header.nRows,1, pixels.data(), header.nCols) == 0){
        err = "Failed to write PNG: " + outPath;
        return false;
    }

    return true;

}
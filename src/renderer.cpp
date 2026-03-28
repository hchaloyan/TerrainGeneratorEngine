#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../third_party/stbImageWrite.h"
#include "renderer.h"
#include "colormap.h"
#include <algorithm>

bool renderGrayscale(
    const ElevationTile& tile, 
    const std::string& outPath,
    std::string& err
) {


    const auto& elevations = tile.elevations;
    const auto& header = tile.header;


    int16_t minimum = INT16_MAX;
    int16_t maximum = INT16_MIN;

    // Find min/max and ignore noData
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
    if (stbi_write_png(outPath.c_str(), header.nCols, header.nRows, 1, pixels.data(), header.nCols) == 0){
        err = "Failed to write PNG: " + outPath;
        return false;
    }

    return true;

}

// ---------------------------------------------------------------------------------

bool renderColormap(
    const ElevationTile& tile, 
    const std::string& outPath,
    std::string& err
) {


    const auto& elevations = tile.elevations;
    const auto& header = tile.header;

    
    int16_t minimum = INT16_MAX;
    int16_t maximum = INT16_MIN;

    // Find min/max and ignore noData
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

    // Buffer, multiply size by three for RGB representation
    std::vector<uint8_t> pixels(elevations.size() * 3);

    double t;

    // Iterate through elevations vector
    for (size_t i = 0; i < elevations.size(); i++) {
        
        
        // Set noData to zero
        if (elevations[i] == header.noData) {
            t = 0.0;
        } 
        // Otherwise, apply: (value - min) / (max - min)
        else {
            t = (double) (elevations[i] - minimum) / (maximum - minimum);
        
        }

        int s = 0;

        while(s < numTerrainStops - 2 && t > terrainStops[s + 1].t){
            s++;
        }

        double f = (t - terrainStops[s].t) / (terrainStops[s+1].t - terrainStops[s].t);
        pixels[i*3 + 0] = (uint8_t)(terrainStops[s].red + f * (terrainStops[s+1].red - terrainStops[s].red));
        pixels[i*3 + 1] = (uint8_t)(terrainStops[s].green + f * (terrainStops[s+1].green - terrainStops[s].green));
        pixels[i*3 + 2] = (uint8_t)(terrainStops[s].blue + f * (terrainStops[s+1].blue - terrainStops[s].blue));
    }

    // stbi_write_png returns 0 on failure
    // Write PNG using stb, and error check
    if (stbi_write_png(outPath.c_str(), header.nCols, header.nRows, 3, pixels.data(), header.nCols * 3) == 0){
        err = "Failed to write PNG: " + outPath;
        return false;
    }

    return true;

}
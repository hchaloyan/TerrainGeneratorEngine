#include <iostream>
#include "parser.h"
#include "renderer.h"

int main(int argc, char* argv[]) {
    
    // Arg count check
    if (argc < 2) {
        std::cerr << "Improper arg count:\nUsage: terrain <path/to/file> [--colormap grayscale|color]\n";
        return 1;
    }

    BILParser parser;
    ElevationTile tile;
    std::string err;

    // Parse and error check
    if (!parser.parse(argv[1], tile, err)) {
        std::cerr << "Parse error: " << err << "\n";
        return 1;
    }

    const auto& h = tile.header;

    std::cout << "Grid:     " << h.nCols << " x " << h.nRows << "\n";
    std::cout << "Cell:     " << h.xDim << "° x " << h.yDim << "°\n";
    std::cout << "UL:       " << h.upperLeftLongitude << "°E, "
                              << h.upperLeftLatitude  << "°N\n";
    std::cout << "Endian:   " << (h.bigEndian ? "big" : "little") << "\n";
    std::cout << "No-data:  " << h.noData << "\n\n";

    // Sample a 3x3 grid of points spread across the tile
    for (int r : { 0, h.nRows/2, h.nRows-1 }) {
        for (int c : { 0, h.nCols/2, h.nCols-1 }) {
            int16_t elev = tile.at(r, c);
            if (elev == h.noData)
                std::cout << "(" << r << "," << c << ") noData\n";
            else
                std::cout << "(" << r << "," << c << ") = " << elev << " m\n";
        }
    }

    std::string outputPath = "output/render.png";

    // Default to color
    std::string mode = "color";

    // Check for --colormap flag
    for (int i = 2; i < argc; i++) {
        if (std::string(argv[i]) == "--colormap" && i + 1 < argc) {
            mode = argv[i + 1];
        }
    }

    if (mode == "grayscale") {
        if (!renderGrayscale(tile, outputPath, err)) {
            std::cerr << "Render error: " << err << "\n";
            return 1;
        }
    } else if (mode == "color") {
        if (!renderColormap(tile, outputPath, err)) {
            std::cerr << "Render error: " << err << "\n";
            return 1;
        }
    } else {
        std::cerr << "Unknown colormap: " << mode << "\n";
        std::cerr << "Usage: --colormap [grayscale|color]\n";
        return 1;
    }

    std::cout << "Saved output: " << outputPath << "\n";

    return 0;
}
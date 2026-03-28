#include <iostream>
#include "parser.h"
#include "renderer.h"

int main(int argc, char* argv[]) {
    
    // Arg check
    if (argc < 2) {
        std::cerr << "Improper arg count,\nUsage: terrain <path/to/file.bil>\n";
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


    // Greyscale Render -------------------------------------------------------

    std::string outputPath = "output/testRender.png";

    // Render test:
    if(!renderGrayscale(tile, outputPath, err)){
        std::cerr << "Render error: " << err << "\n";
        return 1;
    }

    std::cout << "Saved output: " << outputPath << ".\n";



    // Colormap Render --------------------------------------------------------

    std::string colorOutputPath = "output/colorRender.png";

    // Render test:
    if(!renderColormap(tile, colorOutputPath, err)){
        std::cerr << "Render error: " << err << "\n";
        return 1;
    }

    std::cout << "Saved output: " << colorOutputPath << ".\n";

    return 0;
}
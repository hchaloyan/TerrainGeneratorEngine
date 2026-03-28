#include <iostream>
#include "parser.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: terrain <path/to/file.bil>\n";
        return 1;
    }

    BILParser parser;
    ElevationTile tile;
    std::string err;

    if (!parser.parse(argv[1], tile, err)) {
        std::cerr << "Parse error: " << err << "\n";
        return 1;
    }

    std::cout << "Loaded " << tile.header.ncols << "x" << tile.header.nrows
              << " tile  (cellsize=" << tile.header.cellsize << "°)\n";
    std::cout << "UL corner: " << tile.header.ulxmap << "°E, "
              << tile.header.ulymap << "°N\n";

    // Quick sanity: print center elevation
    int cr = tile.header.nrows / 2, cc = tile.header.ncols / 2;
    std::cout << "Center elevation: " << tile.at(cr, cc) << " m\n";

    return 0;
}
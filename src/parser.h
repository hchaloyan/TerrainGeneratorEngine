#pragma once
#include <string>
#include <vector>
#include <cstdint>

// Metadata from BIL
struct BILHeader {
    int    nRows              = 0;     
    int    nCols              = 0;
    int    nBits              = 16;
    double xDim               = 0.0;
    double yDim               = 0.0;
    double upperLeftLongitude = 0.0;    // upper-left longitude
    double upperLeftLatitude  = 0.0;    // upper-left latitude
    int    noData             = -32767;
    bool bigEndian            = false;
};

// A parsed elevation tile: header metadata + flat row-major elevation grid
struct ElevationTile {
    BILHeader header;
    // Row-major: index = row * header.nCols + col
    // Values are metres above sea level; noData cells == header.noData
    std::vector<int16_t> elevations;

    // Convenience accessor
    int16_t at(int row, int col) const {
        return elevations[row * header.nCols + col];
    }
};

class BILParser {
public:
    // Parse a .hdr/.bil pair.  Pass either filename; the parser swaps extensions.
    // Returns false and populates error_msg on failure.
    bool parse(const std::string& filepath, ElevationTile& out, std::string& error_msg);

private:
    bool parseHeader(const std::string& hdr_path, BILHeader& hdr, std::string& err);
    bool parseData  (const std::string& bil_path, ElevationTile& tile, std::string& err);
};
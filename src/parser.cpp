#include "parser.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>

// Helper Functions ----------------------------------------------------------------

static std::string toLower(std::string s) {
    std::transform(
        s.begin(),
        s.end(),
        s.begin(),
        [](unsigned char c){ return std::tolower(c); }
    );


    return s;
}

// Replace extension of path with newExt (include the dot, e.g. ".hdr")
static std::string swapExtension(const std::string& path, const std::string& newExt) {
    auto dot = path.rfind('.');
    return (dot == std::string::npos ? path : path.substr(0, dot)) + newExt;
}

// Swap bytes of a 16-bit integer: Big-Endian to Little-Endian
static int16_t swapBytes(int16_t v) {
    return static_cast<int16_t>(((v & 0xFF) << 8) | ((v >> 8) & 0xFF));
}

// Parser --------------------------------------------------------------------------

bool BILParser::parse(const std::string& filepath,
                      ElevationTile& out,
                      std::string& error_msg)
{
    std::string hdr_path = swapExtension(filepath, ".hdr");
    std::string bil_path = swapExtension(filepath, ".bil");

    if (!parseHeader(hdr_path, out.header, error_msg)) return false;
    if (!parseData  (bil_path, out,         error_msg)) return false;
    return true;
}

bool BILParser::parseHeader(
    const std::string& hdr_path,
    BILHeader& hdr,
    std::string& err
){
    
    // Open header path
    std::ifstream f(hdr_path);
    
    // Error Check
    if (!f.is_open()) {
        err = "Cannot open header: " + hdr_path;
        return false;
    }



    std::string line;
    
    // Parse line by line
    while (std::getline(f, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (!(iss >> key >> value)) continue;

        std::string k = toLower(key);


        if      (k == "nrows")     hdr.nRows                = std::stoi(value);
        else if (k == "ncols")     hdr.nCols                = std::stoi(value);
        
        else if (k == "nbits")     hdr.nBits                = std::stoi(value);
        
        else if (k == "xdim")      hdr.xDim                 = std::stod(value);
        else if (k == "ydim")      hdr.yDim                 = std::stod(value);
        
        else if (k == "ulxmap")    hdr.upperLeftLongitude   = std::stod(value);
        else if (k == "ulymap")    hdr.upperLeftLatitude    = std::stod(value);
        
        
        else if (k == "nodata")    hdr.nodata               = std::stoi(value);
        
        
        else if (k == "byteorder") hdr.bigEndian            = (value == "M" 
                                                                || value == "m");
    }

    // Error Checking
    if (hdr.nRows == 0 || hdr.nCols == 0) {
        err = "Header missing nrows/ncols: " + hdr_path;
        return false;
    }
    if (hdr.nbits != 16) {
        err = "Only 16-bit BIL is supported (got " + std::to_string(hdr.nBits) + ")";
        return false;
    }


    return true;
}

bool BILParser::parseData(
    const std::string& bil_path,
    ElevationTile& tile,
    std::string& err
) {
    
    // Error check BIL path
    std::ifstream f(bil_path, std::ios::binary);
    if (!f.is_open()) {
        err = "Cannot open data file: " + bil_path;
        return false;
    }

    const int total = tile.header.nRows * tile.header.nCols;
    tile.elevations.resize(total);

    // Read all raw bytes
    f.read(reinterpret_cast<char*>(tile.elevations.data()),
           total * sizeof(int16_t));

    if (f.gcount() != static_cast<std::streamsize>(total * sizeof(int16_t))) {
        err = "Unexpected EOF in " + bil_path +
              " (read " + std::to_string(f.gcount()) + " bytes)";
        return false;
    }

    // Swap if Big-Endian
    if (hdr.header.bigEndian){
        for (auto& v : tile.elevations) v = swapBytes(v);
    }
    return true;
}
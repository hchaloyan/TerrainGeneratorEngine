#include "hillshade.h"
#include <cmath>

std::vector<double> computeHillshade(const ElevationTile& tile) {

    const auto& elevations = tile.elevations;
    const auto& header = tile.header;

    // Sun direction: northwest at 45 degrees elevation
    const double angularDistance   = 315.0 * M_PI / 180.0;  // northwest
    const double altitude  = 45.0  * M_PI / 180.0;  // sun angle above horizon

    // Sun vector in 3D
    const double sunX = cos(altitude) * cos(angularDistance);
    const double sunY = cos(altitude) * sin(angularDistance);
    const double sunZ = sin(altitude);

    // Cell size in meters (approximate — 1 degree latitude ~ 111km)
    const double cellSizeMeters = header.xDim * 111000.0;

    std::vector<double> shade(elevations.size(), 0.0);

    for (int row = 1; row < header.nRows - 1; row++) {
        for (int col = 1; col < header.nCols - 1; col++) {

            // Get elevations of neighbours
            double left  = elevations[row * header.nCols + (col - 1)];
            double right = elevations[row * header.nCols + (col + 1)];
            double up    = elevations[(row - 1) * header.nCols + col];
            double down  = elevations[(row + 1) * header.nCols + col];

            // Skip if any neighbour is noData
            if (left  == header.noData || right == header.noData ||
                up    == header.noData || down  == header.noData) {
                shade[row * header.nCols + col] = 0.0;
                continue;
            }

            // Central difference gradient
            double dzdx = (right - left) / (2.0 * cellSizeMeters);
            double dzdy = (down  - up)   / (2.0 * cellSizeMeters);

            // Surface normal (unnormalized)
            double nx = -dzdx;
            double ny = -dzdy;
            double nz = 1.0;

            // Normalize
            double len = sqrt(nx*nx + ny*ny + nz*nz);
            nx /= len;
            ny /= len;
            nz /= len;

            // Dot product with sun vector — clamp to 0 on dark side
            double dot = nx*sunX + ny*sunY + nz*sunZ;
            shade[row * header.nCols + col] = std::max(0.0, dot);
        }
    }

    return shade;
}
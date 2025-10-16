// include/Histogram1D.h
#ifndef HISTOGRAM1D_H
#define HISTOGRAM1D_H

#include <vector>
#include <string>
#include <mutex>
#include <fstream>
#include <iomanip>
#include <iostream>

class Histogram1D {
private:
    std::vector<double> sumValues;        // Suma de parámetros en cada bin
    std::vector<double> sumSquaredValues; // Suma de cuadrados para la std
    std::vector<int> counts;              // Conteo de muestras en cada bin
    double minDistance, maxDistance;
    int numBins;
    double binWidth;
    std::mutex mutex;

public:
    Histogram1D(double minDist, double maxDist, int bins);
    void addDataPoint(double distance, double parameter);
    std::pair<std::vector<double>, std::vector<double>> getAverageValues() const;
    void saveToFile(const std::string& filename) const;
    void printStatistics() const;
};

#endif

// src/Histogram1D.cpp
#include "Histogram1D.h"
#include <cmath>

Histogram1D::Histogram1D(double minDist, double maxDist, int bins)
    : minDistance(minDist), maxDistance(maxDist),
      numBins(bins), binWidth((maxDist - minDist) / bins) {
    sumValues.resize(numBins, 0.0);
    sumSquaredValues.resize(numBins, 0.0);
    counts.resize(numBins, 0);
}

void Histogram1D::addDataPoint(double distance, double parameter) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (distance >= minDistance && distance <= maxDistance) {
        int bin = static_cast<int>((distance - minDistance) / binWidth);
        
        // Asegurarse de que no nos salimos de los límites
        bin = std::min(std::max(bin, 0), numBins - 1);
        
        sumValues[bin] += parameter;
        sumSquaredValues[bin] += parameter * parameter;
        counts[bin]++;
    }
}

std::pair<std::vector<double>, std::vector<double>> Histogram1D::getAverageValues() const {
    std::vector<double> averages(numBins, 0.0);
    std::vector<double> stdDevs(numBins, 0.0);
    
    for (int i = 0; i < numBins; ++i) {
        if (counts[i] > 0) {
            averages[i] = sumValues[i] / counts[i];
        }
        if (counts[i] > 1) {
            double variance = (sumSquaredValues[i] - counts[i] * averages[i] * averages[i]) / (counts[i] - 1);
            stdDevs[i] = std::sqrt(std::max(variance, 0.0));
        }
    }
    
    return {averages, stdDevs};
}

void Histogram1D::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo: " << filename << std::endl;
        return;
    }
    
    // Escribir encabezado
    file << "# r avg_BOP std_BOP count\n";
    
    auto [averages, stdDevs] = getAverageValues();
    
    for (int i = 0; i < numBins; ++i) {
        double distance = minDistance + (i + 0.5) * binWidth;
        file << std::fixed << std::setprecision(3) 
             << distance << " " 
             << std::setprecision(6) << averages[i] << " "
             << stdDevs[i] << " "
             << counts[i] << "\n";
    }
    
    file.close();
}

void Histogram1D::printStatistics() const {
    auto averages = getAverageValues();
    int totalSamples = 0;
    
    for (int count : counts) {
        totalSamples += count;
    }
    
    std::cout << "Estadísticas del histograma:\n";
    std::cout << "Total de muestras: " << totalSamples << "\n";
    std::cout << "Rango de distancia: [" << minDistance << ", " << maxDistance << "]\n";
    std::cout << "Número de bins: " << numBins << "\n";
    
    // Encontrar bins no vacíos
    int nonEmptyBins = 0;
    for (int count : counts) {
        if (count > 0) nonEmptyBins++;
    }
    std::cout << "Bins no vacíos: " << nonEmptyBins << "/" << numBins << "\n";
}

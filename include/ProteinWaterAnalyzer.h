// include/ProteinWaterAnalyzer.h
#ifndef PROTEINWATERANALYZER_H
#define PROTEINWATERANALYZER_H

#include "ThreadPool.h"
#include "Histogram1D.h"
#include "Types.h"
#include <string>
#include <atomic>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>
#include <iostream>
#include <map>
#include <filesystem>  // Para explorar el directorio
#include <algorithm>   // Para ordenar

class ProteinWaterAnalyzer {
private:
    ThreadPool pool;
    Histogram1D histogram;
    std::atomic<int> processedFrames;
    int totalFrames;
    
    double calculateMinDistance(const WaterMolecule& water, 
                               const std::vector<ProteinAtom>& proteinAtoms,
                               double Lx, double Ly, double Lz);
    
    FrameData readProteinFile(const std::string& filename, int frameNumber);
    void readWaterFile(const std::string& filename, FrameData& frame);
    
    // Métodos para explorar el directorio
    std::vector<std::pair<int, std::string>> findProteinFiles(const std::string& directory);
    std::vector<std::pair<int, std::string>> findWaterFiles(const std::string& directory);
    std::vector<std::pair<int, std::pair<std::string, std::string>>> findFilePairs(const std::string& directory);
    
public:
    ProteinWaterAnalyzer(size_t numThreads, double minDist, double maxDist, int bins);
    
    void processFrame(int frameNumber, const std::string& proteinFile, 
                     const std::string& waterFile, int parameterIndex);
    
    // Método actualizado para procesar desde directorio
    void processDirectory(const std::string& directory, int parameterIndex);

    // Espera a que terminen todas las tareas en el thread pool
    void wait();
    
    void saveHistogram(const std::string& filename);
    void printStatistics();
};

#endif

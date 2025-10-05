// src/ProteinWaterAnalyzer.cpp
#include "ProteinWaterAnalyzer.h"

ProteinWaterAnalyzer::ProteinWaterAnalyzer(size_t numThreads, 
                        double minDist, double maxDist, 
                        int bins)
    : pool(numThreads), 
      histogram(minDist, maxDist, bins),
      processedFrames(0), totalFrames(0) {}

double ProteinWaterAnalyzer::calculateMinDistance(const WaterMolecule& water, 
                               const std::vector<ProteinAtom>& proteinAtoms,
                               double Lx, double Ly, double Lz) {
    double minDist = std::numeric_limits<double>::max();
    
    for (const auto& atom : proteinAtoms) {
        double dx = water.x - atom.x;
        double dy = water.y - atom.y;
        double dz = water.z - atom.z;
        
        // Aplicar condiciones periódicas de contorno
        dx -= Lx * std::round(dx / Lx);
        dy -= Ly * std::round(dy / Ly);
        dz -= Lz * std::round(dz / Lz);
        
        double dist = std::sqrt(dx*dx + dy*dy + dz*dz);
        minDist = std::min(minDist, dist);
    }
    
    return minDist;
}

FrameData ProteinWaterAnalyzer::readProteinFile(const std::string& filename, int frameNumber) {
    FrameData frame(frameNumber);
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo: " + filename);
    }
    
    int numAtoms;
    file >> numAtoms;
    
    std::string line;
    std::getline(file, line); // Leer el resto de la primera línea
    std::getline(file, line); // Leer línea de dimensiones
    
    // Parsear dimensiones
    std::istringstream dimStream(line);
    std::string token;
    while (dimStream >> token) {
        if (token.find("Lx=") != std::string::npos) {
            frame.Lx = std::stod(token.substr(3));
        } else if (token.find("Ly=") != std::string::npos) {
            frame.Ly = std::stod(token.substr(3));
        } else if (token.find("Lz=") != std::string::npos) {
            frame.Lz = std::stod(token.substr(3));
        }
    }
    
    // Leer átomos
    for (int i = 0; i < numAtoms; ++i) {
        std::string element;
        double x, y, z;
        file >> element >> x >> y >> z;
        frame.proteinAtoms.emplace_back(element, x, y, z);
    }
    
    file.close();
    return frame;
}

void ProteinWaterAnalyzer::readWaterFile(const std::string& filename, FrameData& frame) {
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo: " + filename);
    }
    
    int numMolecules;
    file >> numMolecules;
    
    std::string line;
    std::getline(file, line); // Leer el resto de la primera línea
    
    // Leer moléculas de agua
    for (int i = 0; i < numMolecules; ++i) {
        std::string element;
        double x, y, z, q4, q6, w4, w6;
        file >> element >> x >> y >> z >> q4 >> q6 >> w4 >> w6;
        frame.waterMolecules.emplace_back(element, x, y, z, q4, q6, w4, w6);
    }
    
    file.close();
}

// Encontrar archivos de proteína (frame_XXX.xyz)
std::vector<std::pair<int, std::string>> ProteinWaterAnalyzer::findProteinFiles(const std::string& directory) {
    std::vector<std::pair<int, std::string>> proteinFiles;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // Buscar archivos que coincidan con el patrón frame_XXX.xyz
                if (filename.find("frame_") == 0 && filename.find("_bop") == std::string::npos) {
                    // Extraer el número del frame
                    std::string numberStr = filename.substr(6, filename.find(".xyz") - 6);
                    try {
                        int frameNumber = std::stoi(numberStr);
                        proteinFiles.emplace_back(frameNumber, entry.path().string());
                    } catch (const std::exception& e) {
                        std::cerr << "Error extrayendo número del frame de: " << filename << std::endl;
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Error accediendo al directorio: " + std::string(e.what()));
    }
    
    // Ordenar por número de frame
    std::sort(proteinFiles.begin(), proteinFiles.end(), 
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    return proteinFiles;
}

// Encontrar archivos de agua (frame_XXX_bop.xyz)
std::vector<std::pair<int, std::string>> ProteinWaterAnalyzer::findWaterFiles(const std::string& directory) {
    std::vector<std::pair<int, std::string>> waterFiles;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                
                // Buscar archivos que coincidan con el patrón frame_XXX_bop.xyz
                if (filename.find("frame_") == 0 && filename.find("_bop.xyz") != std::string::npos) {
                    // Extraer el número del frame
                    std::string numberStr = filename.substr(6, filename.find("_bop.xyz") - 6);
                    try {
                        int frameNumber = std::stoi(numberStr);
                        waterFiles.emplace_back(frameNumber, entry.path().string());
                    } catch (const std::exception& e) {
                        std::cerr << "Error extrayendo número del frame de: " << filename << std::endl;
                    }
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Error accediendo al directorio: " + std::string(e.what()));
    }
    
    // Ordenar por número de frame
    std::sort(waterFiles.begin(), waterFiles.end(), 
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    return waterFiles;
}

// Encontrar pares de archivos que coincidan
std::vector<std::pair<int, std::pair<std::string, std::string>>> ProteinWaterAnalyzer::findFilePairs(const std::string& directory) {
    auto proteinFiles = findProteinFiles(directory);
    auto waterFiles = findWaterFiles(directory);
    
    std::vector<std::pair<int, std::pair<std::string, std::string>>> filePairs;
    
    // Crear mapas para búsqueda rápida
    std::map<int, std::string> proteinMap(proteinFiles.begin(), proteinFiles.end());
    std::map<int, std::string> waterMap(waterFiles.begin(), waterFiles.end());
    
    // Encontrar frames que tengan ambos archivos
    for (const auto& [frame, proteinFile] : proteinMap) {
        auto waterIt = waterMap.find(frame);
        if (waterIt != waterMap.end()) {
            filePairs.emplace_back(frame, std::make_pair(proteinFile, waterIt->second));
        } else {
            std::cerr << "Advertencia: No se encontró archivo de agua para el frame " << frame << std::endl;
        }
    }
    
    // También verificar frames de agua sin proteína
    for (const auto& [frame, waterFile] : waterMap) {
        if (proteinMap.find(frame) == proteinMap.end()) {
            std::cerr << "Advertencia: No se encontró archivo de proteína para el frame " << frame << std::endl;
        }
    }
    
    std::cout << "Encontrados " << filePairs.size() << " pares de archivos válidos" << std::endl;
    return filePairs;
}

void ProteinWaterAnalyzer::processFrame(int frameNumber, const std::string& proteinFile, 
                     const std::string& waterFile, int parameterIndex) {
    auto future = pool.enqueue([this, frameNumber, proteinFile, waterFile, parameterIndex]() {
        try {
            // Leer datos
            FrameData frame = readProteinFile(proteinFile, frameNumber);
            readWaterFile(waterFile, frame);
            
            // Procesar cada molécula de agua
            for (const auto& water : frame.waterMolecules) {
                if (water.element == "O") { // Solo átomos de oxígeno
                    double minDist = calculateMinDistance(water, frame.proteinAtoms, 
                                                        frame.Lx, frame.Ly, frame.Lz);
                    
                    double parameter;
                    switch (parameterIndex) {
                        case 1: parameter = water.Q4; break;
                        case 2: parameter = water.Q6; break;
                        case 3: parameter = water.W4; break;
                        case 4: parameter = water.W6; break;
                        default: parameter = water.Q6; break;
                    }
                    
                    histogram.addDataPoint(minDist, parameter);
                }
            }
            
            processedFrames++;
            std::cout << "Procesado frame " << frameNumber 
                     << " (" << processedFrames << "/" << totalFrames << ")" << std::endl;
                    
        } catch (const std::exception& e) {
            std::cerr << "Error procesando frame " << frameNumber << ": " << e.what() << std::endl;
        }
    });
}

// Procesar desde directorio
void ProteinWaterAnalyzer::processDirectory(const std::string& directory, int parameterIndex) {
    auto filePairs = findFilePairs(directory);
    totalFrames = filePairs.size();
    processedFrames = 0;
    
    if (totalFrames == 0) {
        std::cerr << "Error: No se encontraron pares de archivos válidos en el directorio: " << directory << std::endl;
        return;
    }
    
    std::cout << "Procesando " << totalFrames << " frames desde el directorio: " << directory << std::endl;
    
    for (const auto& [frameNumber, files] : filePairs) {
        processFrame(frameNumber, files.first, files.second, parameterIndex);
    }
}



void ProteinWaterAnalyzer::wait() {
    // Delegar al thread pool
    pool.wait();
}
void ProteinWaterAnalyzer::saveHistogram(const std::string& filename) {
    histogram.saveToFile(filename);
}

void ProteinWaterAnalyzer::printStatistics() {
    histogram.printStatistics();
}

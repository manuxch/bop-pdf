// main.cpp
#include "ProteinWaterAnalyzer.h"
#include <iostream>
#include <thread>
#include <chrono>

void showUsage(const char* programName) {
    std::cout << "Uso: " << programName << " <directorio> [opciones]" << std::endl;
    std::cout << "Opciones:" << std::endl;
    std::cout << "  -p <número>    Parámetro de orden a usar (1-4:Q4,Q6,W4,W6, por defecto: 2-Q6)" << std::endl;
    std::cout << "  -t <número>    Número de hilos (por defecto: CPUs disponibles)" << std::endl;
    std::cout << "  -o <archivo>   Archivo de salida (por defecto: histograma.dat)" << std::endl;
    std::cout << "  -min <valor>   Distancia mínima (por defecto: 0.0)" << std::endl;
    std::cout << "  -max <valor>   Distancia máxima (por defecto: 20.0)" << std::endl;
    std::cout << "  -bins <número> Número de bins (por defecto: 100)" << std::endl;
    std::cout << std::endl;
    std::cout << "Ejemplo:" << std::endl;
    std::cout << "  " << programName << " /ruta/a/mis/datos -p 2 -t 8 -o resultado.csv" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        showUsage(argv[0]);
        return 1;
    }
    
    try {
        // Valores por defecto
        std::string directory = argv[1];
        int parameterIndex = 1;
        int numThreads = std::thread::hardware_concurrency();
        std::string outputFile = "histograma.dat";
        double minDistance = 0.0;
        double maxDistance = 20.0;
        int distanceBins = 100;
        
        // Parsear argumentos
        for (int i = 2; i < argc; i++) {
            std::string arg = argv[i];
            if (arg == "-p" && i + 1 < argc) {
                parameterIndex = std::stoi(argv[++i]);
            } else if (arg == "-t" && i + 1 < argc) {
                numThreads = std::stoi(argv[++i]);
            } else if (arg == "-o" && i + 1 < argc) {
                outputFile = argv[++i];
            } else if (arg == "-min" && i + 1 < argc) {
                minDistance = std::stod(argv[++i]);
            } else if (arg == "-max" && i + 1 < argc) {
                maxDistance = std::stod(argv[++i]);
            } else if (arg == "-bins" && i + 1 < argc) {
                distanceBins = std::stoi(argv[++i]);
            } else if (arg == "-h" || arg == "--help") {
                showUsage(argv[0]);
                return 0;
            }
        }
        
        // Validar parámetros
        if (parameterIndex < 1 || parameterIndex > 4) {
            std::cerr << "Error: El parámetro de orden debe estar entre 1 y 4" << std::endl;
            return 1;
        }
        
        if (numThreads < 1) {
            std::cerr << "Error: El número de hilos debe ser al menos 1" << std::endl;
            return 1;
        }
        
        std::string p2bop[] {"Q4", "Q6", "W4", "W6"};
        std::cout << "Iniciando análisis con los siguientes parámetros:" << std::endl;
        std::cout << "  Directorio: " << directory << std::endl;
        std::cout << "  Hilos: " << numThreads << std::endl;
        std::cout << "  Parámetro de orden: " << p2bop[parameterIndex - 1] << std::endl;
        std::cout << "  Rango de distancia: [" << minDistance << ", " << maxDistance << "]" << std::endl;
        std::cout << "  Bins: " << distanceBins << std::endl;
        std::cout << "  Archivo de salida: " << outputFile << std::endl;
        std::cout << std::endl;
        
        // Crear analizador
        ProteinWaterAnalyzer analyzer(numThreads, minDistance, maxDistance, distanceBins);
        
        // Procesar directorio
        analyzer.processDirectory(directory, parameterIndex);
        
        // Esperar a que terminen todos los trabajos
        std::cout << "Esperando a que terminen todos los trabajos..." << std::endl;
        analyzer.wait(); // sincronización de tareas

        std::cout << "Guardando histograma..." << std::endl;
        analyzer.saveHistogram(outputFile);
        
        // Mostrar estadísticas
        analyzer.printStatistics();
        
        std::cout << "Análisis completado exitosamente!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

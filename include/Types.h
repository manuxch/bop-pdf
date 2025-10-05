// include/Types.h
#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

// Clase para representar un átomo de la proteína
class ProteinAtom {
public:
    std::string element;
    double x, y, z;
    
    ProteinAtom(const std::string& elem, double x_, double y_, double z_)
        : element(elem), x(x_), y(y_), z(z_) {}
};

// Clase para representar una molécula de agua
class WaterMolecule {
public:
    std::string element;
    double x, y, z;
    double Q4, Q6, W4, W6;
    
    WaterMolecule(const std::string& elem, double x_, double y_, double z_,
                  double q4_, double q6_, double w4_, double w6_)
        : element(elem), x(x_), y(y_), z(z_), Q4(q4_), Q6(q6_), W4(w4_), W6(w6_) {}
};

// Clase para representar un frame completo
class FrameData {
public:
    int frameNumber;
    std::vector<ProteinAtom> proteinAtoms;
    std::vector<WaterMolecule> waterMolecules;
    double Lx, Ly, Lz;
    
    FrameData(int frame) : frameNumber(frame), Lx(0), Ly(0), Lz(0) {}
};

#endif

# bop-pdf

Calcula la distribución de valores de los BOP del agua en función de la distancia a la proteína.

## Compilación:

    ./build.sh [opciones]

### Opciones:

```
    -d, --debug          Compilar en modo Debug (por defecto: Release)
    -c, --clean          Limpiar directorio de build antes de compilar
    -j, --jobs N         Usar N jobs paralelos (por defecto: todos los cores)
    -v, --verbose        Mostrar output detallado
    -h, --help           Mostrar este mensaje de ayuda
```

### Ejemplos:
```
    ./build.sh                    # Compilación normal Release
    ./build.sh --debug            # Compilación Debug
    ./build.sh --clean --debug    # Limpiar y compilar en Debug
    ./build.sh --jobs 4           # Usar 4 cores para compilar
    ./build.sh --verbose          # Output detallado
```

### Descripción:
    Este script automatiza la compilación del programa que calcula la 
    función de distribución radial (RDF) de un parámetro de orden 
    (bond order parameter - bop) de las aguas que rodean una proteína.
    mediante el patrón de diseño de analizadores base.

## Uso del programa después de compilar:
   
    ./bop-rdf <directorio> [opciones]

### Opciones:
```
  -p <número>    Parámetro de orden a usar (1-4:Q4,Q6,W4,W6, por defecto: 2-Q6)
  -t <número>    Número de hilos (por defecto: CPUs disponibles)
  -o <archivo>   Archivo de salida (por defecto: histograma.dat)
  -min <valor>   Distancia mínima (por defecto: 0.0)
  -max <valor>   Distancia máxima (por defecto: 20.0)
  -bins <número> Número de bins (por defecto: 100)
```

Ejemplo:

    ./bop-rdf /ruta/a/mis/datos -p 2 -t 8 -o resultado.csv


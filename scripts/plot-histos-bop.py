#!/usr/bin/env python3

import glob
import numpy as np 
import matplotlib.pyplot as plt 
import argparse
import os

plt.rcParams.update({
    'text.usetex': True,
    'font.family': 'serif',
    'font.serif': ['Linux Libertine O'],
    'font.size': 14,
    'axes.titlesize': 14,
    'axes.labelsize': 14,
    'legend.fontsize': 14,
    'xtick.labelsize': 14,
    'ytick.labelsize': 14
})

def main():
    # Crear el parser de argumentos
    parser = argparse.ArgumentParser(
        description='Generador de gráficos que ilustran el valor del parámetro en función de la distancia a una proteína',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='''
Ejemplos de uso:
  python script.py --directorio datos/ --minimo 0.5 --maximo 15.0 --parametro Q4
  python script.py -d resultados/ -min 1.0 -max 10.0 -p W6
  python script.py  # Usa todos los valores por defecto
        '''
    )
    
    # Definir los argumentos
    parser.add_argument(
        '-d', '--directorio',
        type=str,
        default='.',
        help='Directorio de trabajo (por defecto: directorio actual)'
    )
    
    parser.add_argument(
        '-min', '--minimo',
        type=float,
        default=0.0,
        help='Valor mínimo para el parámetro (por defecto: 0.0)'
    )
    
    parser.add_argument(
        '-max', '--maximo',
        type=float,
        default=25.0,
        help='Valor máximo para el parámetro (por defecto: 25.0)'
    )
    
    parser.add_argument(
        '-p', '--parametro',
        type=str,
        choices=['Q4', 'Q6', 'W4', 'W6'],
        default='Q6',
        help='Parámetro de orden: Q4, Q6, W4, W6 (por defecto: Q6)'
    )
    
    # Parsear los argumentos
    args = parser.parse_args()
    
    # Validar que el directorio existe
    if not os.path.exists(args.directorio):
        print(f"Error: El directorio '{args.directorio}' no existe.")
        return
    
    # Validar que mínimo < máximo
    if args.minimo >= args.maximo:
        print("Error: El valor mínimo debe ser menor que el valor máximo.")
        return
    
    # Mostrar los parámetros recibidos
    print("Parámetros de configuración:")
    print(f"  Directorio de trabajo: {args.directorio}")
    print(f"  Valor mínimo: {args.minimo}")
    print(f"  Valor máximo: {args.maximo}")
    print(f"  Parámetro de orden: {args.parametro}")
    
    # Aquí iría el código para generar los gráficos
    print(f"\nGenerando gráficos para el parámetro {args.parametro}...")
    print(f"Rango: [{args.minimo}, {args.maximo}]")
    print(f"Directorio de salida: {args.directorio}")
    

    files = glob.glob(args.directorio + '/histo-*.dat')
    files.sort()
    print(files)
    for file in files:
        lbl = file.split('-')[1][:-4]
        print(lbl)
        r, h, std = np.loadtxt(file, usecols=(0,1,2), unpack=True)
        plt.errorbar(r, h, yerr=std, alpha=0.9, label=lbl)
        # plt.plot(r, h, '-', alpha=0.6, label=lbl)
    plt.legend()
    plt.xlim([args.minimo, args.maximo])
    plt.xlabel(r'$r$ [A]')
    plt.ylabel(f'Valor medio {args.parametro}')
    plt.tight_layout()
    plt.savefig('comp-histos.pdf')



if __name__ == "__main__":
    main()


/*
 * Autor: Yair Alejandro Erreguin Franco
 * Descripción: Implementación de orquestador de Esquema Compartido de Shamir para números de 512 bits
 * Fecha: 22/01/2026
 * Dependencias: sss.h, sssio.h
 * Uso: Para generar shares a partir de un secreto, el secreto debe llegar desde STDIN; para recuperar un 
 *      secreto se deben de recibir archivos .sss mediante CLI para obtener cada share y recuperar el secreto.
 * .:ORQUESTADOR:.
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //Se hace uso de getopt, estandart de POSIX
#include "sss.h"
#include "sssio.h"

/*
 * Función que muestra una breve descripción sobre lo que hace el binario, es decir,
 * explica como usarlo y algunos ejemplos.
 * */
void imprimir_ayuda(char *prog_name)
{
  fprintf(stderr, "Uso: %s [OPCIONES] [ARCHIVOS...]\n", prog_name);
  fprintf(stderr, "Opciones: \n");
  fprintf(stderr, "\t-s -n <N> -k <K>     :(Shamir) Divide un secreto leído desde STDIN en N shares diferentes con un umbral K.\n");
  fprintf(stderr, "\t-r <archivos...>     :(Recuperar) Recupera secreto mediante K shares leidas desde archivos tido .sss\n");
  fprintf(stderr, "\t-h                   :(Ayuda) Muestra esta ayuda.\n");
  fprintf(stderr, "\nEjemplos: \n");
  fprintf(stderr, "\tGenerar              : echo 'AABBCC...' | %s -s -n 5 -k 3\n",prog_name);
  fprintf(stderr, "\tRecuperar            : %s -j share_1.sss share_3.sss share_4.sss > secreto_recuperado\n", prog_name);
}

/*
 * Función que n y k. Estos parametros sirven para dividir un secreto en n shares asignando
 * a dicho secreto un umbral k, es decir, se generan n shares pero solo se necesitan k*/
void modo_generar_shares(int n, int k, BN_CTX *ctx)
{
  //n,k>=2 y k>n
  if(n<2 || k<2 || k>n)
  {
    fprintf(stderr, "Error: Parametros invalidos (modo_generar_shares())\n");
    exit(EXIT_FAILURE);
  }

  BIGNUM *secreto = BN_new();
  leer_secreto_stdin(secreto);  //Se estan usando pipes por SEGURIDAAD

  //Definición de estructuras necesarias
  polinomio P;
  shares Ss;

  //Inicialización de estructuras
  inicializar_polinomio(&P, k);
  inicializar_shares(&Ss, n, k);

  /* Esta validación es necesaria porque si el secreto no pertenece al campo de galois generado entonces
   * este secreto se mapeara, por la naturaleza del modulo, a otro número, lo cual va a causar que no se 
   * pueda recuperar el secreto.
   * */
  if(BN_cmp(secreto, Ss.PRIME_NUMBER) >= 0)
  {
    fprintf(stderr, "Error: El secreto no pertence a GF(P) (modo_generar_shares())\n");
    exit(EXIT_FAILURE);
  }

  //Generar shares
  construir_polinomio(&P, secreto, Ss.PRIME_NUMBER, ctx);
  generar_shares(&Ss, &P, ctx);

  //Escritura
  for(int i=0; i<n; i++)
    escribir_archivo_share(Ss.S[i].x, &Ss.S[i]);

  //Liberación de recursos utilizados
  BN_clear_free(secreto);
  destruir_polinomio(&P);
  destruir_shares(&Ss);

}

/*
 * Función que recibe n archivos para interpolar un polinomio y tratar de recuperar el secreto
 * en cual esta determinado por P(0)*/
void modo_recuperar_secreto(int argc, char **argv, int optind, BN_CTX *ctx)
{
  int archivos_total = argc - optind;
  if(archivos_total < 2)
  {
    fprintf(stderr, "Error: Se necesitan al menos 2 shares para interpolar el polinomio (modo_recuperar_secreto())\n");
    exit(EXIT_FAILURE);
  }

  //Inicialización de estructura que almacenara los shares leidos desde share_i.sss
  shares Ss;
  /*Es importante aclarar que shamir funciona siempre y la cantidad de puntos para interpolar sea mayor o igual
   * a k original.*/
  inicializar_shares(&Ss, archivos_total, archivos_total); //Se tomara k=cantidad de archivos

  int shares_validos = 0;
  for(int i=0; i<archivos_total; i++)
  {
    if(leer_archivo_share(argv[optind+i], &Ss.S[i]))
      shares_validos++;
    else
      fprintf(stderr, "Error: Archivo [%s] invalido (modo_recuperar_secreto())\n", argv[optind + i]);
  }

  if(shares_validos < 2)
  {
    fprintf(stderr, "Error: No hay suficientes shares válidos para interpolar el polinomio P (modo_recuperar_secreto())\n");
    exit(EXIT_FAILURE);
  }

  Ss.k = shares_validos;

  //Interpolación
  BIGNUM *recuperado = BN_new();
  interpolar_shares(recuperado, &Ss, ctx);

  //Salida a STDOUT
  char *hex = BN_bn2hex(recuperado);
  printf("%s", hex);

  //Liberación de recursos utilizados
  OPENSSL_free(hex);
  BN_clear_free(recuperado);
  destruir_shares(&Ss);
}

int main(int argc, char **argv)
{
  int opt;
  int mode = 0;
  int n = 0;
  int k = 0;

  //Iniciando contexto de OPENSSL
  BN_CTX *ctx = BN_CTX_new();
  if(!ctx)
  {
    fprintf(stderr, "Error: Error al iniciar OPENSSL (main())\n");
    exit(EXIT_FAILURE);
  }

  //Parseo de flags
  while((opt = getopt(argc, argv, "hsrn:k:")) != -1)
  {
    switch (opt) 
    {
      case 'h':
        imprimir_ayuda(*(argv+0));
        return 0;
      case 's':
        mode = 1;
        break;
      case 'r':
        mode = 2;
        break;
      case 'n':
        n = atoi(optarg);
        break;
      case 'k':
        k = atoi(optarg);
        break;
      default:
        imprimir_ayuda(*(argv+0));
        exit(EXIT_FAILURE);
    }
  }

  //ORQUESTADO
  if(mode == 1)
  {
    if(n == 0 || k == 0)
    {
      fprintf(stderr, "Error: El esquema de shamir requiere -n y -k\n");
      exit(EXIT_FAILURE);
    }
    modo_generar_shares(n, k, ctx);
  }
  else if(mode == 2)
  {
    modo_recuperar_secreto(argc, argv, optind, ctx);
  }
  else 
  {
    imprimir_ayuda(*(argv+0));
    exit(EXIT_FAILURE);
  }
  BN_CTX_free(ctx);
  return 0;
}


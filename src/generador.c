/*
 * Autor: Yair Alejandro Erreguin Franco
 * Descripción: Generador de números aleaotorios de alta entropia
 * Fecha: 22/01/2026
 * .:GENERADOR:.
 * */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/bn.h>
#include <openssl/rand.h>


void imprimir_ayuda(char *program_name)
{
  fprintf(stderr, "Uso: %s [OPCIONES]\n",program_name);
  fprintf(stderr, "Modos de operación:\n");
  fprintf(stderr, "\t-n <BITS>     : (Número) Genera un número aleaotorio de BITS bits.\n");
  fprintf(stderr, "\t-p <BITS>     : (Primo)  Genera un número primo aleatorio de alta entropia de BITS bits.\n");
  fprintf(stderr, "\th            : (Help)   Mostrar esta ayuda.\n");
  fprintf(stderr, "\nEjemplos:\n");
  fprintf(stderr, "\tNúmero aleaotorio de 32 bits  : %s -n 32\n", program_name);
  fprintf(stderr, "\tNúmero aleaotorio primo de 256 bits : %s -p 256\n", program_name);
}


int generar_numero(int bits, int opt)
{
  if(bits <= 0)
  {
    fprintf(stderr, "Error: La cantidad de bits debe ser positiva. :P\n");
    return 1;
  }
  BIGNUM *rnd = BN_new();
  int exito = 0;
  if(opt == 'p') //PRIMO
  {
    if(!BN_generate_prime_ex(rnd, bits, 1, NULL, NULL, NULL))
    {
      fprintf(stderr, "Erro al generar número primo :()\n");
      exito = 0;
    }
    else
      exito = 1;
  }
  else 
  {
    if(!BN_rand(rnd, bits, -1, 0))
    {
      fprintf(stderr, "Error al generar numero aleaotorio :/\n");
      exito = 0;
    }
    else
      exito = 1;
  }
  if(exito)
  {
    char *hex = BN_bn2hex(rnd);
    printf("%s\n", hex);
    OPENSSL_free(hex);
  }
  
  BN_clear_free(rnd);
  return !exito;
}

void checkCLI(int argc, char *program_name)
{
  if(argc < 2)
  {
    imprimir_ayuda(program_name);
    exit(EXIT_FAILURE);
  }
}
int main(int argc, char **argv)
{
  checkCLI(argc, *(argv));
  int opt;
  int bits = 0;
  
  while ((opt = getopt(argc, argv, "p:n:h")) != -1)
  {
    switch(opt) 
    {
      case 'h':
        imprimir_ayuda(*(argv));
        return 0;
      case 'n':
        bits = atoi(optarg);
        generar_numero(bits, 'n');
        break;
      case 'p':
        bits = atoi(optarg);
        generar_numero(bits, 'p');
        break;
      default:
        imprimir_ayuda(*(argv));
        exit(EXIT_FAILURE);
    }
  }
  return 0;
}

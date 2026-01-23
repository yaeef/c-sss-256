
/*
 * Autor: Yair Alejandro Erreguin Franco
 * Descripción: Script para testing de las bibliotecas implementadas.
 * Fecha: 22/01/2026
 * .:TESTING:.
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sss.h"

void checkCLI(int argc)
{
  if(argc < 4)
  {
    fprintf(stderr, "Error en argumentos CLI\n");
    exit(EXIT_FAILURE);
  }
}
              

int main(int argc, char **argv)
{
  checkCLI(argc);

  //INICIANDO CONTEXTO DE OPENSSL
  BN_CTX *ctx = BN_CTX_new();
  if(!ctx)
  {
    fprintf(stderr, "Error: Creando contexto OPENSSL :()\n");
    exit(EXIT_FAILURE);
  }

  //PARSEO DE SECRETO (HEX->BIGNUM)
  BIGNUM *secreto = BN_new();
  if(BN_hex2bn(&secreto, *(argv+1)) == 0)
  {
    fprintf(stderr, "Error: El secreto no esta en formato Hexadecimal :()\n");
    exit(EXIT_FAILURE);
  }

  //PARAMETROS
  int n = atoi(*(argv+2));
  int k = atoi(*(argv+3));

  if(k>=n)
  {
    fprintf(stderr, "Error: Se debe cumplir K<N\n");
    exit(EXIT_FAILURE);
  }

  printf("=== INICIO SSS 512 ===\n");
  printf("Configuración: n=[%d], k=[%d]", n, k);
  printf("Bits del secreto: [%d]\n", BN_num_bits(secreto));

  //ESTRUCTURAS
  polinomio P;
  shares Ss;
  BIGNUM *secreto_recuperado = BN_new(); //AQUI SE ALMACENARA EL RESULTADO DE LA INTERPOLACION P(0)

  //INICIALIZACIÓN DE ESTRUCTURAS
  inicializar_polinomio(&P, k);
  inicializar_shares(&Ss, n, k);

  printf("+++ Obteniendo el Campo de Galois GF(P)...\n");
  cargar_primo(Ss.PRIME_NUMBER);

  //VALIDACIÓN S<P
  if(BN_cmp(secreto, Ss.PRIME_NUMBER) >= 0)
  {
    fprintf(stderr, "Error: El secreto es no pertenece al Campo de Galois GF(P) :()\n");
    exit(EXIT_FAILURE);
    goto cleanup;
  }

  //GENERANDO SHARES
  printf("+++ Generando Shares...\n");
  construir_polinomio(&P, secreto, Ss.PRIME_NUMBER, ctx);
  generar_shares(&Ss, &P, ctx);

  printf("\n--- Shares generados ---\n");
  for(int i=0; i<Ss.n; i++)
    imprimir_share(&Ss.S[i]);
  printf("------------------------------\n\n");

  //INTERPOLACION
  printf("+++ Interpolando secreto ...\n");
  interpolar_shares(secreto_recuperado, &Ss, ctx);

  //BIGNUM->HEX
  char *hex_rec = BN_bn2hex(secreto_recuperado);
  char *hex_org = BN_bn2hex(secreto);

  printf("Secreto original: [%s]\n",hex_org);
  printf("Secreto recuperado: [%s]\n",hex_rec);
  if(strcmp(hex_rec,hex_org) == 0)
    printf("\n>>> Exito:El secreto se recupero correctamente :)<<<\n");
  else
    printf("\n>>> Fallo: Los secretos no coinciden :( <<<)\n");

  OPENSSL_free(hex_org);
  OPENSSL_free(hex_rec);

  //LIBERACIÓN DE RECURSOS
  cleanup:
    destruir_shares(&Ss);
    destruir_polinomio(&P);
    BN_clear_free(secreto);
    BN_clear_free(secreto_recuperado);
    BN_CTX_free(ctx);

  return 0;
}

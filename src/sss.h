/*
 * Autor: Yair Alejandro Erreguin Franco
 * Descripción: Implementación de Biblioteca de Esquema de Secreto Compartido de Shamir (512 bits) | SPLIT|JOIN
 * Fecha: 29/12/2025
 * .:PROTOTIPOS:.
 * */

#ifndef SSS_H
#define SSS_H

#include <openssl/bn.h>
#include <openssl/rand.h>


  /*ESTRUCTURAS*/

/*
 * Estructura que contendra los coeficientes del polinomio de Shamir.
 * Este polinomio se usara para generar los pares (x,y) que formaran
 * un share.
 * */
typedef struct 
{
  BIGNUM **coeficientes;
  int k;
}polinomio;

/*
 * Estructura que contendra un par generado por el polinomio de Shamir.
 * Este par (x,y) es lo que se consedira un Share.
 * */
typedef struct
{
  int x;
  BIGNUM *y;
}share;

/*
 * Estructura que contendrá el número n el cual indica la cantidad total
 * shares existentes, el número k que indica el umbral de shares y el número
 * primo usado para generar el Campo Primo.*/
typedef struct 
{
  share *S;
  int n;
  int k;
  BIGNUM *PRIME_NUMBER;
}shares;


  /*PROTOTIPOS*/

/*
 * Ambas funciones se encargan de gestionar los recursos utilizados por un polinomio
 * */
void inicializar_polinomio(polinomio*, int);
void destruir_polinomio(polinomio*);

/*
 * Ambas funcones se encargan de gestionar los recursos utilizados por la estructura que
 * almacena el conjunto de shares*/
void inicializar_shares(shares*, int, int);
void destruir_shares(shares*);

/*
 * Se construye el polinomio interpolante de Shamir y se generar los Shares
 * (x,y) evaluando en el polinomio. */
void construir_polinomio(polinomio*, BIGNUM*, BIGNUM*, BN_CTX*);
void generar_shares(shares*, polinomio*, BN_CTX*);

/*
 * Dadas k shares, se interpola el polinomio en P(0) = S
 * para recuperar el secreto*/
void interpolar_shares(BIGNUM*, shares*, BN_CTX*);

/*
 * La funcion imprimir_shares se usó para hacer pruebas de escritorio
 * y debuguear el código.
 * La función cargar_primo() toma un número primo seguro y lo convierte 
 * a BIGNUM. Este número fue generado mediante OpenSSL, se creó un script
 * que genera números primos aleatorios de alta entropia de 512 bits.*/
void cargar_primo(BIGNUM *p);
void imprimir_share(share*);


#endif

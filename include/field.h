
/*
 * Autor: Yair Alejandro Erreguin Franco
 * Descripción: Implementación de Biblioteca de Esquema de Secreto Compartido de Shamir (512 bits) | Abstracción de Campo Finito
 * GF(P) = {X; X~(amodP); a,x numeros enteros}
 * Fecha: 29/12/2025
 * .:PROTOTIPOS:.
 * */

#ifndef GF_H
#define GF_H

#include <openssl/bn.h>

/* OPERACIONES DE CAMPO*/

int gf_add(BIGNUM*, const BIGNUM*, const BIGNUM*, const BIGNUM*, BN_CTX*); //Suma modular      : r === a+b(modP)
int gf_sub(BIGNUM*, const BIGNUM*, const BIGNUM*, const BIGNUM*, BN_CTX*); //Resta modular     : r === a+(-b)(modP)
int gf_mul(BIGNUM*, const BIGNUM*, const BIGNUM*, const BIGNUM*, BN_CTX*); //Producto modular  : r === a*b(modP)
int gf_inv(BIGNUM*, const BIGNUM*, const BIGNUM*, BN_CTX*);   //Inverso multiplicativo modular : r === a⁻¹(modP)
int gf_div(BIGNUM*, const BIGNUM*, const BIGNUM*, const BIGNUM*, BN_CTX*); //Cociennte modular : r === a*(a⁻¹)(modP)
                                                                           //
#endif

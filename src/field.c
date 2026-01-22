
/*
 * Autor: Yair Alejandro Erreguin Franco
 * Descripción: Implementación de Biblioteca de Esquema de Secreto Compartido de Shamir (512 bits) | Abstracción de Campo Finito
 * Dependencias: field.h (definiciones)
 * GF(P) = {X; X~(amodP); a,x numeros enteros}
 * Fecha: 29/12/2025
 * .:DEFINICIONES:.
 * */

#include "field.h"
/* OPERACIONES DE CAMPO*/

int gf_add(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, const BIGNUM *p, BN_CTX *ctx) //Suma modular      : r === a+b(modP)
{
  return BN_mod_add(r, a, b, p, ctx);
}

int gf_sub(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, const BIGNUM *p, BN_CTX *ctx) //Resta modular     : r === a+(-b)(modP)
{
  return BN_mod_sub(r, a, b, p, ctx);
}

int gf_mul(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, const BIGNUM *p, BN_CTX *ctx) //Producto modular  : r === a*b(modP)
{
  return BN_mod_mul(r, a, b, p, ctx);
}


int gf_div(BIGNUM *r, const BIGNUM *a, const BIGNUM *b, const BIGNUM *p, BN_CTX *ctx) //Cociennte modular : r === a*(a⁻¹)(modP)
{
  BN_CTX_start(ctx);
  BIGNUM *b_i = BN_CTX_get(ctx);
  if(!b_i)
  {
    BN_CTX_end(ctx);
    return 0;
  }
  if(!gf_inv(b_i, b, p, ctx))
  {
    BN_CTX_end(ctx);
    return 0;
  }
  int flag_r = gf_mul(r, a, b_i, p, ctx);
  BN_CTX_end(ctx);
  return flag_r;
}

int gf_inv(BIGNUM *r, const BIGNUM *a, const BIGNUM *p, BN_CTX *ctx)   //Inverso multiplicativo modular : r === a⁻¹(modP)
{
  if(BN_mod_inverse(r, a, p, ctx) == NULL)
    return 0;
  return 1;
}



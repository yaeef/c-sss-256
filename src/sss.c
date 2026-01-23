/*
 * Autor: Yair Alejandro Erreguin Franco
 * Descripción: Implementación del Esquema de Shamir con OpenSSL y BigNums (512 bits)
 * Dependencias: field.h (Aritmética de Campos), sss.h (Estructuras)
 * Fecha: 29/12/2025
 * .:DEFINICIONES:.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sss.h"
#include "field.h"

const char *SAFE_PRIME_512 = 
    "D4387CF0B5BAD332AC44F396684AF92C9E792A5C932B313E056C0C4AA318AD31"
    "A5D06D26A1F283485572764F8ADDB550D2FC99BC6B6E3B097B246350A01F279F";

  /* GESTIÓN DE RECURSOS */

void inicializar_polinomio(polinomio *P, int k)
{
    P->k = k;
    P->coeficientes = (BIGNUM **)malloc(k * sizeof(BIGNUM *));
    if (P->coeficientes == NULL)
    {
        perror("Error : MEMORY NOT FOUND (inicializar_polinomio())");
        exit(1);
    }

    for(int i = 0; i < k; i++)
    {
        P->coeficientes[i] = BN_new();
        if(P->coeficientes[i] == NULL) exit(1);
        BN_zero(P->coeficientes[i]);
    }
}

void destruir_polinomio(polinomio *P)
{
    if (P == NULL || P->coeficientes == NULL) return;

    //Limpieza segura de cada coeficiente (sobrescribe con ceros)
    for(int i = 0; i < P->k; i++)
    {
        BN_clear_free(P->coeficientes[i]);
    }
    //Se libera memoria usada y se hace un reset de la estructura del polinomio
    free(P->coeficientes);
    P->coeficientes = NULL;
    P->k = 0;
}

void inicializar_shares(shares *Ss, int n, int k)
{
    Ss->n = n;
    Ss->k = k;
    
    Ss->PRIME_NUMBER = BN_new();

    Ss->S = (share *)malloc(n * sizeof(share));
    if (Ss->S == NULL)
    {
        perror("Error : MEMORY NOT FOUND (inicializar_shares())");
        exit(1);
    }

    //Inicialización de y_i para cada share
    for(int i = 0; i < n; i++)
    {
        Ss->S[i].x = 0;
        Ss->S[i].y = BN_new();
        BN_zero(Ss->S[i].y);
    }
}

void destruir_shares(shares *Ss)
{
    if (Ss == NULL) return;

    //Limpieza segura de BIGNUMS
    if (Ss->S != NULL)
    {
        for(int i = 0; i < Ss->n; i++)
        {
            BN_clear_free(Ss->S[i].y);
        }
        free(Ss->S);
        Ss->S = NULL;
    }

    //Liberar el primo
    if (Ss->PRIME_NUMBER)
    {
        BN_clear_free(Ss->PRIME_NUMBER);
        Ss->PRIME_NUMBER = NULL;
    }
}
 
  /*LÓGICA MATEMÁTICA*/

/* * Construye el polinomio f(x) = S + a1*x + ... + a(k-1)*x^(k-1) 
 * S: Secreto (BigNum)
 * P_mod: Primo del campo (BigNum)
 */

void construir_polinomio(polinomio *P, BIGNUM *secreto, BIGNUM *P_mod, BN_CTX *ctx)
{
    if (BN_copy(P->coeficientes[0], secreto) == NULL)
    {
        fprintf(stderr, "Error: S = A[0] (construir_polinomio())\n");
        exit(1);
    }

    //Generar coeficientes a_1 ... a_{k-1} aleatoriamente
    for(int i = 1; i < P->k; i++)
    {
        if (!BN_rand_range(P->coeficientes[i], P_mod))
        {
            fprintf(stderr, "Error: generando entropía OpenSSL construir_polinomio()\n");
            exit(1);
        }
    }
}

/* * Evalúa el polinomio en un punto x usando el Método de Horner 
 * Algoritmo: r = (...(a_k*x + a_{k-1})*x + ...)*x + a_0
 */
void evaluar_polinomio(BIGNUM *resultado, polinomio *P, int x_val, BIGNUM *mod_p, BN_CTX *ctx)
{
    BN_CTX_start(ctx);
    BIGNUM *bn_x = BN_CTX_get(ctx); 
    BN_set_word(bn_x, x_val);

    BN_copy(resultado, P->coeficientes[P->k - 1]);

    for(int i = P->k - 2; i >= 0; i--)
    {
        //r = (r * x) mod P
        gf_mul(resultado, resultado, bn_x, mod_p, ctx);
        
        //r = (r + a_i) mod P
        gf_add(resultado, resultado, P->coeficientes[i], mod_p, ctx);
    }
    BN_CTX_end(ctx);
}

void generar_shares(shares *Ss, polinomio *P, BN_CTX *ctx)
{   
    for(int i = 0; i < Ss->n; i++) 
    {
        Ss->S[i].x = i + 1;
        evaluar_polinomio(Ss->S[i].y, P, Ss->S[i].x, Ss->PRIME_NUMBER, ctx);
    }
}

/*Interpolación de Lagrange para recuperar el secreto f(0) */
void interpolar_shares(BIGNUM *recuperado, shares *Ss, BN_CTX *ctx)
{
    BN_CTX_start(ctx);

    BIGNUM *num = BN_CTX_get(ctx);
    BIGNUM *den = BN_CTX_get(ctx);
    BIGNUM *term = BN_CTX_get(ctx);
    BIGNUM *bn_xi = BN_CTX_get(ctx);
    BIGNUM *bn_xj = BN_CTX_get(ctx);
    BIGNUM *temp_sub = BN_CTX_get(ctx);
    BIGNUM *cero = BN_CTX_get(ctx);
    
    BN_zero(recuperado);

    for(int i = 0; i < Ss->k; i++) 
    {    
        BN_set_word(bn_xi, Ss->S[i].x);
        
        BN_one(num);
        BN_one(den);

        for(int j = 0; j < Ss->k; j++) 
        {
            if(i == j) continue;

            BN_set_word(bn_xj, Ss->S[j].x);

            //NUMERADOR: (0 - x_j) 
            BN_zero(cero);
            gf_sub(temp_sub, cero, bn_xj, Ss->PRIME_NUMBER, ctx); //temp = -x_j
            gf_mul(num, num, temp_sub, Ss->PRIME_NUMBER, ctx);    //num *= temp

            //DENOMINADOR: (x_i - x_j)
            gf_sub(temp_sub, bn_xi, bn_xj, Ss->PRIME_NUMBER, ctx); //temp = xi - xj
            gf_mul(den, den, temp_sub, Ss->PRIME_NUMBER, ctx);     //den *= temp
        }

        //CÁLCULO DEL TERMINO
        gf_div(term, num, den, Ss->PRIME_NUMBER, ctx);
        gf_mul(term, term, Ss->S[i].y, Ss->PRIME_NUMBER, ctx); //term *= y_i

        gf_add(recuperado, recuperado, term, Ss->PRIME_NUMBER, ctx);
    }
    BN_CTX_end(ctx);
}

  /*UTILERÍAS*/

void imprimir_share(share *s)
{
    char *hex_y = BN_bn2hex(s->y);
    printf("ID (x): %d\n", s->x);
    printf("VAL(y): %s\n", hex_y);
    OPENSSL_free(hex_y);
}

void cargar_primo(BIGNUM *p)
{
    //Se carga el número primo seguro y es convierte de HEX a BIGNUM
    if(BN_hex2bn(&p, SAFE_PRIME_512) == 0) 
    {
        fprintf(stderr, "Error fatal: No se pudo cargar el primo constante :(.\n");
        exit(1);
    }
}

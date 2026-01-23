
/*
 * Autor: Yair Alejandro Erreguin Franco
 * Descripción: Implementación de funciones encargadas de la entrada del secreto y salida de los SHARES. Biblioteca enfocada en la entrada/salida del esquema de Shamir.
 * Fecha: 22/01/2026
 * .:PROTOTIPOS:.
 * */

#ifndef SSS_IO
#define SSS_IO

#include <string.h>
#include <openssl/bn.h>
#include "sss.h"
/*
 * Definicio del header y footer de los archivos shares_i.sss
 * */
#define HEADER ".:SSS-KEY:."
#define FOOTER ".:END-SSS-KEY:."

/*PROTOTIPOS*/

void leer_secreto_stdin(BIGNUM*);                     //input
void escribir_archivo_share(int ,share*);             //output
int leer_archivo_share(char*, share*);                //input



#endif

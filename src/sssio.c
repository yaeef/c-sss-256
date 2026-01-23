/*
 * Autor: Yair Alejandro Erreguin Franco
 * Descripción: Implementación de funciones encargadas de la entrada del secreto y salida de los SHARES. Biblioteca enfocada en la entrada/salida del esquema de Shamir.
 * Fecha: 22/01/2026
 * .:DEFINICIONES:.
 * */


#include <stdio.h>
#include "sssio.h"

/*
 * Función que lee desde el stream STDIN el secreto que se busca dividir. El uso esta pensado para
 * enviar el secreto mediante el uso de PIPES (|)
 * */
void leer_secreto_stdin(BIGNUM *secreto)
{
  char buffer[2048];
  
  if(!fgets(buffer, sizeof(buffer), stdin))
  {
    fprintf(stderr, "Error: No se recibió el secreto por STDIN (leer_secreto_stdin())\n");
    exit(EXIT_FAILURE);
  }

  buffer[strcspn(buffer, "\n")] = 0;   //Limpiando saltos de linea

  if(BN_hex2bn(&secreto, buffer) == 0) // hex ->BIGNUM
  {
    fprintf(stderr, "Error: El formato de entrada no esta en Hexadecimal (leer_secreto_stdin())\n");
    exit(EXIT_FAILURE);
  }
}

/*
 * Función que hace la escritura de un share siguiendo la estructura definidia, es decir, con 
 * header y footer, el contenido encerrado por el header y footer sera un share del tipo x:y
 * */
void escribir_archivo_share(int id, share *s)
{
  char file_name[64]; 
  snprintf(file_name, sizeof(file_name), "share_%d.sss", id); //creando nombre de archivo
  
  FILE *f = fopen(file_name, "w");
  if(!f)
  {
    fprintf(stderr, "Error: Error al crear archivo de share (escribir_archivo_share())\n");
    exit(EXIT_FAILURE);
  }

  char *y_hex = BN_bn2hex(s->y);

  //FORMATO: HEADER -> X:Y -> FOOTER
  fprintf(f, "%s\n", HEADER);
  fprintf(f, "%d:%s\n",s->x, y_hex);
  fprintf(f, "%s\n", FOOTER);

  OPENSSL_free(y_hex);
  fclose(f);
  fprintf(stderr, "[SHAMIR] Se genero: %s\n", file_name);
}

/*
 * Funcion que se encarga de leer un archivo que contiene un share
 * y almacenarlo en una estructura de tipo share. En ensa función se carga en ram
 * el share compartido para proceder a la interpolación
 * */
int leer_archivo_share(char *file_name, share *s)
{
  FILE *f = fopen(file_name, "r");
  if(!f)
  {
    fprintf(stderr, "Error: Error al abrir archivo de tipo share.sss (leer_archivo_share())\n");
    return 0;
  }

  char linea[2048];
  int header_f = 0;
  int data_f = 0;
  //int footer_f =0;

  while(fgets(linea, sizeof(linea), f))
  {
    linea[strcspn(linea, "\n")] = 0; //Se estan quitando los saltos de linea

    if(strcmp(linea, HEADER) == 0)
    {
      header_f = 1;
      continue;
    }

    if(strcmp(linea, FOOTER) == 0)
      break;

    if(header_f && strchr(linea, ':')) //Se busca x:y
    {
      char *sep = strchr(linea, ':');
      *sep = '\0';

      s->x = atoi(linea);
      if(s->x == 0) continue;

      if(BN_hex2bn(&s->y, sep+1) == 0) continue;

      data_f = 1;
    }
  }
  fclose(f);
  return (header_f && data_f);
}

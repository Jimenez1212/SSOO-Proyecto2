#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cabeceras.h"


#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

//int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo);
//int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock, EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);

int main() 
{
	char* comando[LONGITUD_COMANDO];
	char* orden[LONGITUD_COMANDO];
	char* argumento1[LONGITUD_COMANDO];
	char* argumento2[LONGITUD_COMANDO];
	int  j;
	unsigned long int m;
	int entradadir, grabardatos;
	FILE* fich;
	fich = fopen("particion.bin", "r+b");


	if (fich == NULL) 
	{
		printf("No se ha podido cargar el fichero\n");
		exit(-1);
	}

	EXT_SIMPLE_SUPERBLOCK ext_superblock;
	EXT_BYTE_MAPS ext_bytemaps;
	EXT_BLQ_INODOS ext_blq_inodos;
	EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
	EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
	EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];

	fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fich);

	memcpy(&ext_superblock, (EXT_SIMPLE_SUPERBLOCK*)&datosfich[0], SIZE_BLOQUE);
	memcpy(&directorio, (EXT_ENTRADA_DIR*)&datosfich[3], SIZE_BLOQUE);
	memcpy(&ext_bytemaps, (EXT_BLQ_INODOS*)&datosfich[1], SIZE_BLOQUE);
	memcpy(&ext_blq_inodos, (EXT_BLQ_INODOS*)&datosfich[2], SIZE_BLOQUE);
	memcpy(&memdatos, (EXT_DATOS*)&datosfich[4], MAX_BLOQUES_DATOS * SIZE_BLOQUE);
	
  	for (;;) 
  	{
		do 
		{
			printf(">> ");
			fflush(stdin);
			fgets(comando, LONGITUD_COMANDO, stdin);
		} while (ComprobarComando(comando, orden, argumento1, argumento2) != 0);
		   if (strcmp(orden,"dir") == 0)
		   {
	         Directorio(directorio, &ext_blq_inodos);
	         continue;
			}
			else if(strcmp(orden,"info") == 0)
			{
			   LeeSuperBloque(&ext_superblock);
				continue;
			}
	      else if(strcmp(orden,"bytemaps") == 0)
	      {
				Printbytemaps(&ext_bytemaps);
				continue;
			}
			else if(strcmp(orden,"copy") == 0)
			{
				//Copiar();
				continue;
			}
			else if(strcmp(orden,"rename") == 0)
			{
				//Renombrar();
				continue;
			}
			else if(strcmp(orden,"imprimir") == 0)
			{
				Imprimir(directorio, &ext_blq_inodos, memdatos, argumento1);
				continue;
			}
			else if(strcmp(orden,"remove") == 0)
			{
				//Borrar();
				continue;
			}	 
			
	        else if (strcmp(orden,"salir")==0)
	        {
	
	            fclose(fich);
	       	    return 0;
	        }											
			else
			{
				printf("ERROR: Comando ilegal [info, bytemaps, dir, rename, imprimir, remove, copy, salir]\n");
				continue;
			} 
	}
}


int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre)
{
	int i = 0;

	for (int x = 1; x <= sizeof(directorio); x++) 
	{
		if (strcmp(nombre,directorio[x].dir_nfich)==0) 
		{
			i = x;
		}
	} 
	return i;
}


void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup)
{
	printf("Mostrando la informacion del superbloque:\n\n");
	printf("Bloque %d Bytes\n", psup->s_block_size);
	printf("Inodos particion = %d\n", psup->s_inodes_count);
	printf("Inodos libres = %d\n", psup->s_free_inodes_count);
	printf("Bloques particion = %d\n", psup->s_blocks_count);
	printf("Bloques libres = %d\n", psup->s_free_blocks_count);
	printf("Primer bloque de datos = %d \n", psup->s_first_data_block);
}


void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps) 
{
	printf("Inodos: ");
	for (int i = 0; i < MAX_INODOS; i++) 
	{
		printf("%d ", ext_bytemaps->bmap_inodos[i]);
	}

	printf("\nBloques [0-25]: ");
	for (int x = 0; x < MAX_BLOQUES_PARTICION; x++) 
	{
		printf("%d ", ext_bytemaps->bmap_bloques[x]);
	}

	printf("\n\n");
}


void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos) 
{	
	for (int x = 1; x < MAX_FICHEROS ; x++) 
	{		
		if(directorio[x].dir_nfich != NULL && directorio[x].dir_inodo != NULL_INODO)
		{ 

			//Inodos y tamaño

			printf("%s  ", directorio[x].dir_nfich);
			printf(" tamaño: %d  ", inodos->blq_inodos[directorio[x].dir_inodo].size_fichero);
			printf(" inodo: %d  ", directorio[x].dir_inodo);

			//Bloques

			printf(" Bloques: ");
			for (int y = 0; y < MAX_NUMS_BLOQUE_INODO; y++)
			{
				if(inodos->blq_inodos[directorio[x].dir_inodo].i_nbloque[y] != NULL_BLOQUE)
				{
					printf(" %d", inodos->blq_inodos[directorio[x].dir_inodo].i_nbloque[y]);
				}	
			}
			printf("\n");
		}
	}
}



int ComprobarComando(char* strcomando, char* orden, char* argumento1, char* argumento2) 
{ 
    char *borde;

    borde = strtok(strcomando, "\n");				 
    borde = strtok(strcomando, " ");
    strcpy(orden, borde);						 
	 borde = strtok(NULL, " ");
     
    if(borde != NULL)
    {						 
       strcpy(argumento1, borde);
       borde = strtok(NULL, " ");			
       if(borde != NULL)
       {
    	    strcpy(argumento2, borde);
       }
  	 }
    return 0;
}

void Procs_salir() 
{
	exit(-1);
}

int Imprimir(EXT_ENTRADA_DIR* directorio, EXT_BLQ_INODOS* inodos, EXT_DATOS* memdatos, char* nombre)
{
	int x;
	EXT_DATOS texto[MAX_NUMS_BLOQUE_INODO];
	int fichero = BuscaFich(directorio, inodos, nombre);
	if(fichero == 0)
	{
		printf("ERROR: Fichero %s no encontrado\n", nombre);
		return -1;
	}
	for(x = 0; x < MAX_NUMS_BLOQUE_INODO && inodos->blq_inodos[directorio[fichero].dir_inodo].i_nbloque !=0 ; x++)
	{
		if(inodos->blq_inodos[directorio[fichero].dir_inodo].i_nbloque[x] != NULL_BLOQUE)
		{
			strcpy(&texto[x], &memdatos[inodos->blq_inodos[directorio[fichero].dir_inodo].i_nbloque[x]- PRIM_BLOQUE_DATOS]);
		}	
	}
	memcpy(&texto[x], "\0", 1);
	puts(texto);
	return 0;
}

/**
*@file audio.c
*@brief Implementacion del modulo de configuracion
*
*Este modulo se encarga de todo el manejo de la configuracion basica necesaria para ejecutar el programa
*
*Autor: devLL
*Fecha: Octubre 2025
*Version: 1.0
**/

#include "../include/config.h"

static void trim(char* s); //limpiar variables

static char pathConfig[512]; //Variable que almacena el path del cfg

void configurarDefaults(Config* c)
{
    c->windowW=1366;
    c->windowH=768;
    c->volumen=96;
    c->dif=2;
    c->mode=MODE_SCHOENBERG;
    strcpy(c->playerN,"PLAYER");

    ///Agregar valores nuevos de ser necesario
}

void validarConfig(Config* c)
{
    /*Se validan resoluciones estables, el maximo permitido es 1366 * 768*/
    if(c->windowW<640)
        c->windowW=640;

    if(c->windowW>1366)
        c->windowW=1366;

    if(c->windowH<360)
        c->windowH=360;

    if(c->windowH>768)
        c->windowH=768;

    /*Se valida el volumen maestro, se establecen limites por defecto*/
    if(c->volumen<0)
        c->volumen=0;
    if(c->volumen>128)
        c->volumen=128;

    /*Se valida la dificultad ingresada*/
    if(c->dif<1)
        c->dif=1;

    if(c->dif>5)
        c->dif=5;

    if(c->mode!=MODE_SCHOENBERG && c->mode!=MODE_MOZART)
        c->mode=MODE_SCHOENBERG;

    /*Se valida si se ingreso un caracter al menos, sino se deja el default*/
    if(!c->playerN[0]) strcpy(c->playerN,"PLAYER");

    ///Agregar validaciones necesarias de ser necesario para corregir
}

int cargarConfig(Config* c)
{
    configurarDefaults(c);
    char *eq;
    char *k;
    char linea[256];
    char *v;

    FILE* f=fopen(configPath(),"r");

    /*Si no existe pasa a crear el default*/
    if(!f)
    {
        return -1;
    }


    while(fgets(linea,sizeof(linea),f))
    {
        trim(linea);

        if(!linea[0]||linea[0]=='#'||linea[0]==';')
            continue;

        eq = strchr(linea,'=');

        if(!eq)
            continue;

        *eq = 0;
        k = linea,
        v = eq + 1;

        trim(k);
        trim(v);

        /*Se guarda la información correspondiente en alguno de los valores coincidentes*/
        if(!strcmp(k,"windowW"))
            c->windowW=atoi(v);

        else if(!strcmp(k,"windowH"))
            c->windowH=atoi(v);

        else if(!strcmp(k,"dif"))
            c->dif=atoi(v);

         else if(!strcmp(k,"mode"))
            c->mode=atoi(v)?MODE_MOZART:MODE_SCHOENBERG;

        else if(!strcmp(k,"volumen"))
            c->volumen=atoi(v);

        else if(!strcmp(k,"playerN"))
        {
            strncpy(c->playerN,v,sizeof(c->playerN)-1);
            c->playerN[31]=0;
        }

        ///Agregar nuevas variables para guardar de ser necesario

    }

    fclose(f);

    /*Se establecen valores default para aquellos parametros no validos*/
    validarConfig(c);

    return 0;
}

int guardarConfig(const Config* c)
{

    FILE* f = fopen(configPath(), "w");

    if(!f)
        return -1;
    /*Guarda las variables necesarias*/
    fprintf(f, "# SIMON 80s config\n");
    fprintf(f, "windowW=%d\n", c->windowW);
    fprintf(f, "windowH=%d\n", c->windowH);
    fprintf(f, "dif=%d\n", c->dif);
    fprintf(f, "mode=%d\n", (int)c->mode);
    fprintf(f, "playerN=%s\n", c->playerN);
    fprintf(f, "volumen=%d\n", c->volumen);

    ///Agregar nuevas variables de ser necesario


    fclose(f);

    return 0;
}

/*Quitar espacios*/
static void trim(char* s)
{
    char *q = s;
    while(*q==' ' || *q=='\t')
        q++;

    //se trimea del principio
    memmove(s,q,strlen(q)+1);

    //se trimea el final
    for(int i=(int)strlen(s)-1; i>=0 && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n') ; --i)
        s[i]=0;

}

const char* configPath(void)
{
    if(!pathConfig[0])
    {
        char* base = SDL_GetBasePath();
        snprintf(pathConfig, sizeof(pathConfig), "%sconfig/simon.cfg", base ? base: "./");

        if(base)
            SDL_free(base);
    }

    return pathConfig;
}

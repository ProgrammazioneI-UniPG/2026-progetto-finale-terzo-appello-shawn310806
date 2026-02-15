#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
    bosco, scuola, laboratorio, caverna, strada, 
    giardino, supermercato, centrale_elettrica, 
    deposito_abbandonato, stazione_polizia
} Tipo_zona;

typedef enum {
    nessun_nemico, billi, democane, demotorzone
} Tipo_nemico;

typedef enum {
    nessun_oggetto, bicicletta, maglietta_fuocoinferno, 
    bussola, schitarrata_metallica
} Tipo_oggetto;

struct Zona_soprasotto;
struct Zona_mondoreale;

/* DEFINIZIONE ZONE
Struct per le 2 dimensioni, ogni zona ha:
-Puntatori orizzontali per scorrere la lista della mappa.
-Puntatore verticale per passare alla zona corrispondente nell'altra dimensione. */

typedef struct Zona_mondoreale {
    Tipo_zona tipo;
    Tipo_nemico nemico;
    Tipo_oggetto oggetto;

    struct Zona_mondoreale* prossima_zona;
    struct Zona_mondoreale* zona_precedente;

    struct Zona_soprasotto* link_soprasotto;
}Zona_mondoreale;

typedef struct Zona_soprasotto {
    Tipo_zona tipo;
    Tipo_nemico nemico;

    struct Zona_soprasotto* prossima_zona;
    struct Zona_soprasotto* zona_precedente;

    struct Zona_mondoreale* link_mondoreale;
}Zona_soprasotto;

/* DEFINIZIONE GIOCATORE
Struct per il giocatore, questo ha:
-Dati relativi al giocatore.
-Puntatori per sapere la rispettiva posizione nelle 2 dimensioni.*/

typedef struct Giocatore {
char nome[30];
int mondo;

Zona_mondoreale* pos_mondoreale;
Zona_soprasotto* pos_soprasotto;

int attacco_psichico;
int difesa_psichica;
int fortuna;

Tipo_oggetto zaino[3];

}Giocatore;

void imposta_gioco();
void gioca();
void termina_gioco();
void crediti();
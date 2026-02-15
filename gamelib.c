#include "gamelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static Giocatore* giocatori[4] = {NULL, NULL, NULL, NULL};
static int numero_giocatori = 0;

static Zona_mondoreale* prima_zona_mondoreale = NULL;
static Zona_soprasotto* prima_zona_soprasotto = NULL;

static const char* nomi_zone[] = {"bosco", "scuola", "laboratorio", "caverna", "strada", "giardino", "supermercato", "centrale_elettrica", "deposito_abbandonato", "stazione_polizia"};
static const char* nomi_oggetti[] = {"nessun_oggetto", "bicicletta", "maglietta_fuocoinferno", "bussola", "schitarrata_metallica"};
static const char* nomi_nemici[] = {"nessun_nemico", "billi", "democane", "demotorzone"};

static int mappa_chiusa = 0;
static int turno_attuale = 0;
static int avanza_utilizzato = 0;
static int indietro_utilizzato = 0;
static int demotorzone_sconfitto = 0;

static char ultimi_vincitori[3][50] = {"Nessuno", "Nessuno", "Nessuno"};
static int indice_vincitore_corrente = 0;

/* FUNZIONI SECONDARIE ------------------------------------------- */

static void genera_mappa (){

        /*PULIZIA DELLA MAPPA
        -Se esiste già una prima zona, attraverso un while liberiamo la memoria passando 
         di zona in zona fino a quando non arriviamo a NULL, e la mappa è pulita. */

    if (prima_zona_mondoreale != NULL){
        Zona_mondoreale* attuale_mondoreale = prima_zona_mondoreale;
        Zona_soprasotto* attuale_soprasotto = prima_zona_soprasotto;

        while (attuale_mondoreale != NULL){
            Zona_mondoreale* prossimo_mondoreale = attuale_mondoreale -> prossima_zona;
            Zona_soprasotto* prossimo_soprasotto = attuale_soprasotto -> prossima_zona;

            free(attuale_mondoreale);
            free(attuale_soprasotto);

            attuale_mondoreale = prossimo_mondoreale;
            attuale_soprasotto = prossimo_soprasotto;
        }
    }

        /* INIZIALIZZAZIONE PUNTATORI DI TESTA
        -Resettiamo i puntatori della mappa e prepariamo le variabili di scorrimento. */

    prima_zona_mondoreale = NULL;

    Zona_mondoreale* ultima_zona_mondoreale = NULL;
    Zona_soprasotto* ultima_zona_soprasotto = NULL;


        /* RANDOMIZZIAMO LA POSIZIONE
        -Mettiamo il demotorzone in una delle 15 zone della mappa. */

    int posizione_demotorzone = rand() % 15;

    for (int i = 0; i < 15; i++){

        /* ALLOCAZIONE ZONE  
        -Creiamo due nodi per le due dimensioni e allochiamo lo spazio in memoria. */

        Zona_mondoreale* nuova_zona_mondoreale = (Zona_mondoreale*) malloc(sizeof(Zona_mondoreale));
        Zona_soprasotto* nuova_zona_soprasotto = (Zona_soprasotto*) malloc(sizeof(Zona_soprasotto));

        if(nuova_zona_mondoreale == NULL || nuova_zona_soprasotto == NULL) exit (1);

        /* RANDOMIZZIAMO LE ZONE
        -Rendiamo casuale il tipo della zona, che combacia tra le due dimensioni. */

        Tipo_zona tipo_casuale = rand() % 10;
        nuova_zona_mondoreale -> tipo = tipo_casuale;
        nuova_zona_soprasotto -> tipo = tipo_casuale;

        /* COLLEGAMENTO VERTICALE */

        nuova_zona_mondoreale -> link_soprasotto = nuova_zona_soprasotto;
        nuova_zona_soprasotto -> link_mondoreale = nuova_zona_mondoreale;


        /* COLLEGAMENTO ORIZZONTALE */

        if (i == 0){

            prima_zona_mondoreale = nuova_zona_mondoreale;
            prima_zona_soprasotto = nuova_zona_soprasotto;

            nuova_zona_mondoreale -> zona_precedente = NULL;
            nuova_zona_soprasotto -> zona_precedente = NULL;

        } else {

            ultima_zona_mondoreale -> prossima_zona = nuova_zona_mondoreale;
            nuova_zona_mondoreale -> zona_precedente = ultima_zona_mondoreale;

            ultima_zona_soprasotto -> prossima_zona = nuova_zona_soprasotto;
            nuova_zona_soprasotto -> zona_precedente = ultima_zona_soprasotto;
          }

         nuova_zona_mondoreale -> prossima_zona = NULL;
         nuova_zona_soprasotto -> prossima_zona = NULL;

        /* GENERAZIONE CONTENUTI
        -Assegniamo oggetti e nemici e l'unico Demotorzone. */

         if ((rand() % 100) < 40){
            nuova_zona_mondoreale -> oggetto = (rand() % 4) + 1;
         } else {
            nuova_zona_mondoreale -> oggetto = nessun_oggetto;
           }


        if((rand() % 100) < 30){
            nuova_zona_mondoreale -> nemico = (rand() % 2) + 1;
        } else {
            nuova_zona_mondoreale -> nemico = nessun_nemico;
        }

        if(i == posizione_demotorzone) {
            nuova_zona_soprasotto -> nemico = demotorzone;
        } else {
            if ((rand() % 100) < 40){
                nuova_zona_soprasotto -> nemico = democane;
            } else {
                nuova_zona_soprasotto -> nemico = nessun_nemico;
              }
          }

        /* AGGIORNIAMO L'ULTIMA ZONA INSERITA */

          ultima_zona_mondoreale = nuova_zona_mondoreale;
          ultima_zona_soprasotto = nuova_zona_soprasotto;
    }
}

static void inserisci_zona() {

        /* CONTROLLO CONDIZIONI
        -Verifica delle condizioni necessarie per generare una nuova zona. */

    if(prima_zona_mondoreale == NULL){
        printf("\nERRORE, devi prima generare la mappa base.\n");
        return;
    }

    int posizione = 0;
    printf("\nIn quale posizione vuoi inserire la nuova zona? (0 = INIZIO): ");

    if(scanf("%d", &posizione) != 1){
        while(getchar() != '\n');
        return;
    }

    if (posizione < 0){
        printf("\nERRORE, posizione non valida.\n");
        return;
    }

        /* ALLOCAZIONE MEMORIA
        -Allocazione per la nuova zona e randomizzazione del tipo di zona. */

    Zona_mondoreale* nuova_mondoreale = (Zona_mondoreale*) malloc (sizeof(Zona_mondoreale));
    Zona_soprasotto* nuova_soprasotto = (Zona_soprasotto*) malloc (sizeof(Zona_soprasotto));

    if (nuova_mondoreale == NULL || nuova_soprasotto == NULL) { exit (1);}

    Tipo_zona tipo = rand() % 10;
    nuova_mondoreale -> tipo = tipo;
    nuova_soprasotto -> tipo = tipo;

        /* COLLEGAMENTO VERTICALE */

    nuova_mondoreale -> link_soprasotto = nuova_soprasotto;
    nuova_soprasotto -> link_mondoreale = nuova_mondoreale;

    int scelta_tmp = 0;

        /* OGGETTO MONDOREALE
        -Scelta dell'oggetto in base al giocatore, con cast alla variabile scelta_tmp.*/

    do {
        printf("\nScegli oggetto per il Mondo Reale: \n");
        printf("0 = Nessuno, 1 = Bici, 2 = Maglietta FuocoInferno, 3 = Bussola, 4 = Schitarrata Metallica.");
        printf("\nScelta: ");
        if(scanf("%d", &scelta_tmp) != 1){
            printf("\nERRORE, inserisci un numero!\n");
            while(getchar() != '\n');
            continue;
        }
    }while (scelta_tmp < 0 || scelta_tmp > 4);

    nuova_mondoreale -> oggetto = (Tipo_oggetto)scelta_tmp;

        /* NEMICO MONDOREALE 
        -I possibili inserimenti sono: nessun nemico, billi e democane.*/

    do {
        printf("\nScegli il nemico per il Mondo Reale: \n");
        printf("0 = Nessuno, 1 = Billi, 2 = Democane.\n");
        printf("Scelta: ");
        scanf("%d", &scelta_tmp);
        if(scanf("%d", &scelta_tmp) != 1){
            printf("\nERRORE, inserisci un numero!\n");
            while(getchar() != '\n');
            continue;
        }
    }while(scelta_tmp < 0 || scelta_tmp > 2);

    nuova_mondoreale -> nemico = (Tipo_nemico)scelta_tmp;

        /* NEMICO SOPRASOTTO 
        -I possibili inserimenti sono: nessun nemico, democane e demotorzone.*/

    do {
        printf("\nScegli il nemico per il SopraSotto: \n");
        printf("0 = Nessuno, 2 = Democane, 3 = Demotorzone.\n");
        printf("Scelta: ");
        scanf("%d", &scelta_tmp);
        if(scanf("%d", &scelta_tmp) != 1){
            printf("\nERRORE, inserisci un numero!\n");
            while(getchar() != '\n');
            continue;
        }
    }while(scelta_tmp != 0 && scelta_tmp !=2 && scelta_tmp != 3);

    nuova_soprasotto -> nemico = (Tipo_nemico)scelta_tmp;

        /* INSERIMENTO NELLA LISTA
        -Gestione dell'inserimento in testa, o in mezzo o in coda. */

    if(posizione == 0){
        nuova_mondoreale -> prossima_zona = prima_zona_mondoreale;
        nuova_mondoreale -> zona_precedente = NULL;

        if(prima_zona_mondoreale != NULL) {prima_zona_mondoreale -> zona_precedente = nuova_mondoreale; }
        prima_zona_mondoreale = nuova_mondoreale;

        nuova_soprasotto -> prossima_zona = prima_zona_soprasotto;
        nuova_soprasotto -> zona_precedente = NULL;

         if(prima_zona_soprasotto != NULL) {prima_zona_soprasotto -> zona_precedente = nuova_soprasotto; }
        prima_zona_soprasotto = nuova_soprasotto;

    } else {

        /* COPIA DELLA PRIMA ZONA
        -Creiamo una copia della prima zona da usare come riferimento. */

        Zona_mondoreale* corrente_mondoreale = prima_zona_mondoreale;
        Zona_soprasotto* corrente_soprasotto = prima_zona_soprasotto;
        int conta = 0;

        /* SCORRIAMO LA LISTA 
        -Fino a quand non troviamo il punto dove si vuole inserire la nuova zona. */

        while (conta < posizione - 1 && corrente_mondoreale -> prossima_zona != NULL){
            corrente_mondoreale = corrente_mondoreale -> prossima_zona;
            corrente_soprasotto = corrente_soprasotto -> prossima_zona;
            conta++;
        }

        /* COLLEGHIAMO IL NUOVO BLOCCO 
        -Colleghiamo i blocchi già esistenti a quello nuovo in entrambe le direzioni. */

        nuova_mondoreale -> prossima_zona = corrente_mondoreale -> prossima_zona;
        nuova_soprasotto -> prossima_zona = corrente_soprasotto -> prossima_zona;

        if(corrente_mondoreale -> prossima_zona != NULL){
            corrente_mondoreale -> prossima_zona -> zona_precedente = nuova_mondoreale;
            corrente_soprasotto -> prossima_zona -> zona_precedente = nuova_soprasotto;

        }

        corrente_mondoreale -> prossima_zona = nuova_mondoreale;
        corrente_soprasotto -> prossima_zona = nuova_soprasotto;

        nuova_mondoreale -> zona_precedente = corrente_mondoreale;
        nuova_soprasotto -> zona_precedente = corrente_soprasotto;
    }

    printf("\nZona inserita con successo. \n");

}

static void cancella_zona(){

        /* CONTROLLO CONDIZIONI
        -Verifica delle condizioni necessarie per generare una nuova zona. */

    if(prima_zona_mondoreale == NULL){
        printf("ERRORE, la mappa è vuota. \n");
        return;
    }
    
    int posizione = 0;
    printf("\nQuale posizione vuoi cancellare? (0 = INIZIO): ");
    if(scanf("%d", &posizione) != 1){
        while(getchar() != '\n');
        return;
    }
    
        /* COPIA DELLA PRIMA ZONA
        -Creiamo una copia della prima zona da usare come riferimento. */

    Zona_mondoreale* target_mondoreale = prima_zona_mondoreale;
    Zona_soprasotto* target_soprasotto = prima_zona_soprasotto;

        /* ELIMINAZIONE PRIMO BLOCCO
        -Se si vuole eliminare il primo blocco, spostiamo la prima zona avanti di uno,
         poi mettiamo a NULL il suo campo precedente, e liberiamo il puntatore copia 
         rimuovendo così la zona. */

    if (posizione == 0){
        prima_zona_mondoreale = target_mondoreale -> prossima_zona;
        prima_zona_soprasotto = target_soprasotto -> prossima_zona;

        if (prima_zona_mondoreale != NULL){
            prima_zona_mondoreale -> zona_precedente = NULL;
            prima_zona_soprasotto -> zona_precedente = NULL;
        }

        free(target_mondoreale);
        free(target_soprasotto);
        printf("\nZona in testa cancellata. \n");
        return;
    }

        /* SCORRIAMO LA LISTA 
        -Fino a quando non troviamo il punto dove si vuole inserire la nuova zona. */

    int conta = 0;
    while(conta < posizione && target_mondoreale != NULL) {
        target_mondoreale = target_mondoreale -> prossima_zona;
        target_soprasotto = target_soprasotto -> prossima_zona;
        conta++;
    }

    if(target_mondoreale == NULL){
        printf("ERRORE, posizione non trovata. \n");
        return;
    }

        /* CANCELLAZIONE IN MEZZO ALLA LISTA 
        -Creiamo dei puntatori copia dei campi successivo e precedenti del blocco target. */

    Zona_mondoreale* precedente_mondoreale = target_mondoreale -> zona_precedente;
    Zona_mondoreale* successivo_mondoreale = target_mondoreale -> prossima_zona;

    Zona_soprasotto* precedente_soprasotto = target_soprasotto -> zona_precedente;
    Zona_soprasotto* successivo_soprasotto = target_soprasotto -> prossima_zona;

        /* COLLEGHIAMO I BLOCCHI INTORNO AL TARGET 
        -Facciamo i collegamenti tra blocchi precedenti e successivi al target, 
         in modo che questo rimanga senza collegamenti e si può eliminare. */

    if(precedente_mondoreale != NULL){
        precedente_mondoreale -> prossima_zona = successivo_mondoreale;
        precedente_soprasotto -> prossima_zona = successivo_soprasotto;
    }

    if(successivo_mondoreale != NULL){
        successivo_mondoreale -> zona_precedente = precedente_mondoreale;
        successivo_soprasotto -> zona_precedente = precedente_soprasotto;
    }

    free(target_mondoreale);
    free(target_soprasotto);

    printf("\nZona in posizione %d cancellata. \n", posizione);

}

static void stampa_mappa(){
    
    if(prima_zona_mondoreale == NULL) {
        printf("\nERRORE, la mappa è vuota. \n");
        return;
    }

    int scelta_dimensione = 0;
    do {
        printf("\nQuale mappa vuoi visualizzare?\n");
        printf("1- MondoReale.\n");
        printf("2- SopraSotto. \n");
        printf("Scelta: ");

        if(scanf("%d", &scelta_dimensione) != 1){
            while(getchar() != '\n');
            continue;
        }

    }while (scelta_dimensione != 1 && scelta_dimensione != 2);

    printf("\nVISUALIZZAZIONE MAPPA. \n");
    int i = 0;

    if(scelta_dimensione == 1){
        
        /* PUNTATORE PER SCORRERE LA MAPPA
        -Attraverso un puntatore copia della prima zona scorriamo la lista, 
         e poi stampiamo il numero e tipo della zona in base al nome salvato su
         un array di char. */

        Zona_mondoreale* scorri = prima_zona_mondoreale;

        while (scorri != NULL){
            printf("\nZona %d | Tipo: %s", i, nomi_zone [scorri -> tipo]);

            if(scorri -> oggetto != nessun_oggetto){
                printf("\nOggetto: %s ", nomi_oggetti[scorri -> oggetto]);
            } else {
                printf("\nOggetto: nessuno");
            }

            if(scorri -> nemico != nessun_nemico){
                printf("\nNemico: %s", nomi_nemici[scorri -> nemico]);
            } else {
                printf("\nNemico: nessuno");
            }

            printf("\n");
            scorri = scorri -> prossima_zona;
            i++;
        }

    } else {

        Zona_soprasotto* scorri = prima_zona_soprasotto;

        while (scorri != NULL){

            printf("\nZona %d | Tipo: %s", i, nomi_zone[scorri -> tipo]);

            if(scorri -> nemico == demotorzone){
                printf("\nNemico: DEMOTORZONE, ATTENZIONE.");
            } else if (scorri -> nemico != nessun_nemico){
                printf("\nNemico: %s", nomi_nemici[scorri -> nemico]);
            } else {
                printf("\nNemico: nessuno");
            }

            printf("\n");
            scorri = scorri -> prossima_zona;
            i++;
        }

    }
}    

static void stampa_zona(){

    if(prima_zona_mondoreale == NULL) {
        printf("\nERRORE, la mappa è vuota. \n");
        return;
    }

    int posizione = 0;
    printf("\nInserisci l'indice della zona da stampare. (0 = Inizio): ");
    if (scanf("%d", &posizione) != 1) {
        while(getchar() != '\n');
        return;
    }

    Zona_mondoreale* scorri_reale = prima_zona_mondoreale;
    Zona_soprasotto* scorri_soprasotto = prima_zona_soprasotto;
    int conta = 0;

    while(conta < posizione && scorri_reale != NULL){
        scorri_reale = scorri_reale -> prossima_zona;
        scorri_soprasotto = scorri_soprasotto -> prossima_zona;
        conta++;
    }

    if(scorri_reale == NULL){
        printf("\nERRORE, zona non trovata. \n");
        return;
    }

    printf("\nDETTAGLIO ZONA: %d\n", posizione);

    printf("MONDO REALE: %s\n", nomi_zone[scorri_reale -> tipo]);
    printf("Oggetto: %s\n", nomi_oggetti[scorri_reale -> oggetto]);
    printf("Nemico: %s\n", nomi_nemici[scorri_reale -> nemico]);

    printf("SOPRASOTTO : %s\n", nomi_zone[scorri_soprasotto -> tipo]);
    printf("Nemico: ");
    if (scorri_soprasotto -> nemico == demotorzone) {
    printf("DEMOTORZONE, ATTENZIONE.\n");
    } else {
        printf("%s\n", nomi_nemici[scorri_soprasotto -> nemico]);
    }
}

static void utilizza_oggetto(int indice_giocatore){

    Giocatore* giocatore_corrente = giocatori [indice_giocatore];
    int oggetti_presenti = 0;

    for(int i = 0; i < 3; i++){
        if(giocatore_corrente -> zaino[i] != nessun_oggetto) { oggetti_presenti++;}
    }

    if(oggetti_presenti == 0){
        printf("\nERRORE. Il tuo zaino è vuoto.\n");
        return;
    }

    printf("\nAPRI ZAINO\n");
    for(int i = 0; i < 3; i++){
        printf("Slot %d: ", i + 1);

        if(giocatore_corrente -> zaino[i] == nessun_oggetto){
            printf("(Vuoto)\n");
        } else {
            printf("%s\n", nomi_oggetti[giocatore_corrente -> zaino [i]]);
        }
    }

    int scelta_utente = 0;
    printf("Quale slot vuoi usare? (1-3, oppure 0 per annullare): ");

    if(scanf("%d", &scelta_utente) != 1){
        while(getchar() != '\n');
        return;
    }

    if(scelta_utente == 0){
        printf("Chiudi lo zaino senza usare nulla.\n");
        return;
    }

    int indice = scelta_utente - 1;

    if(indice < 0 || indice > 2 || giocatore_corrente -> zaino[indice] == nessun_oggetto){
        printf("ERRORE, hai scelto uno slot vuoto o non valido.\n");
        return;
    }

    Tipo_oggetto oggetto_usato = giocatore_corrente -> zaino[indice];
    printf("\nUsi: %s\n", nomi_oggetti[oggetto_usato]);

    switch(oggetto_usato){
        case bicicletta:
            printf("Sali nella bicicletta e pedali!\n");
            printf("Se sei nel SopraSotto hai più probabilità di scappare altrimenti non succede niente...\n");
            if(giocatore_corrente -> mondo == 1){
                giocatore_corrente -> fortuna = giocatore_corrente -> fortuna + 5;
                printf("\nHai guadagnato 5 di Fortuna! Ora hai %d Fortuna!\n", giocatore_corrente -> fortuna);
            }

            break;

        case maglietta_fuocoinferno:
            printf("Indossi la maglietta del club e la tua Difesa aumenta. Forse ti da più sicurezza...\n");
            giocatore_corrente -> difesa_psichica = giocatore_corrente -> difesa_psichica + 6;
            printf("\nHai guadagnato 6 di Difesa Psichica! Ora hai %d Difesa!\n", giocatore_corrente -> difesa_psichica);
            printf("Ma attenzione la tua fortuna diminuisce, speriamo che non fai la fine di Eddie...\n");
        
            giocatore_corrente -> fortuna = giocatore_corrente -> fortuna - 3;
            if(giocatore_corrente -> fortuna < 0){ giocatore_corrente -> fortuna = 0; }
            printf("\nHai perso 3 di fortuna, ora hai %d fortuna.\n", giocatore_corrente -> fortuna);
            
            break;

        case bussola: 
            printf("L'ago della bussola gira in direzione del nemico... Sei sempre più vicino alla vittoria.\n");
            printf("Ti fai forza e affronti la battaglia. Il tuo attacco aumenta di poco.\n");
            giocatore_corrente -> attacco_psichico = giocatore_corrente -> attacco_psichico + 3;
            printf("\nHai guadagnato 3 di Attacco Psichico! Ora hai %d Attacco!\n", giocatore_corrente -> attacco_psichico);
            break;

        case schitarrata_metallica:
            printf("Una forte schitarrata aumenta il tuo attacco!\n");

            if(giocatore_corrente -> pos_soprasotto -> nemico == demotorzone){
                printf("\nSei davanti al Demotorzone guadagni ancora più attacco!!");
                giocatore_corrente -> attacco_psichico = giocatore_corrente -> attacco_psichico + 12;
                printf("\nHai guadagnato ben 9 di Attacco Psichico! Ora hai %d Attacco!\n", giocatore_corrente -> attacco_psichico);
            } else {
            giocatore_corrente -> attacco_psichico = giocatore_corrente -> attacco_psichico + 6;
            printf("\nHai guadagnato 6 di Attacco Psichico! Ora hai %d Attacco!\n", giocatore_corrente -> attacco_psichico);
            }

            break;
        
        default:
            printf("Non succede niente...\n");
            break;
    }

    giocatore_corrente -> zaino[indice] = nessun_oggetto;
    printf("L'oggetto è stato consumato e rimosso dallo zaino.\n");

}

static void combatti(int indice_giocatore){
    
    int vita_nemico = 0;
    int attacco_nemico = 0;
    int nemico_trovato = 0;

    Giocatore* giocatore_corrente = giocatori [indice_giocatore];
    Tipo_nemico nemico_corrente = nessun_nemico;

    if(giocatore_corrente -> mondo == 0){
        if(giocatore_corrente -> pos_mondoreale -> nemico != nessun_nemico){
            nemico_corrente = giocatore_corrente -> pos_mondoreale -> nemico;
            nemico_trovato = 1;

            if(nemico_corrente == billi ){
                vita_nemico = 15;
                attacco_nemico = 8;
            } else if(nemico_corrente == democane){
                vita_nemico = 10;
                attacco_nemico = 12;
            }
        } 

    } else {
        if(giocatore_corrente -> pos_soprasotto -> nemico != nessun_nemico){
            nemico_corrente = giocatore_corrente -> pos_soprasotto -> nemico;
            nemico_trovato = 1;

            if(nemico_corrente == demotorzone){
                vita_nemico = 30;
                attacco_nemico = 15;
            } else if(nemico_corrente == democane){
                vita_nemico = 10;
                attacco_nemico = 12;
            }
        } 

    }

    if(nemico_trovato == 0){
        printf("\nNon c'è nessun nemico da combattere in questa zona!\n");
        return;
    }

    printf("\nINIZIO COMBATTIMENTO! Sei contro: %s!\n", nomi_nemici[nemico_corrente]);
    printf("Vita Nemico: %d. Attacco Nemico: %d\n", vita_nemico, attacco_nemico);

    while (vita_nemico > 0 && giocatore_corrente -> difesa_psichica > 0){

        int scelta = 0;
        printf("\nTocca a te %s!", giocatore_corrente -> nome);
        printf("Le tue statistiche...\n Difesa: %d.\n Attacco: %d.\n Fortuna: %d.\n", giocatore_corrente -> difesa_psichica, giocatore_corrente -> attacco_psichico, giocatore_corrente -> fortuna);
        printf("1- Attacco Psichico.\n");
        printf("2- Usa Oggetto dallo Zaino.\n");
        printf("\nScelta: ");
        if(scanf("%d", &scelta) != 1){
            printf("\nERRORE, inserisci un numero!\n");
            while(getchar() != '\n');
            continue;
        }

        switch (scelta){
            case 1: 
                int danno_inflitto = giocatore_corrente -> attacco_psichico + (rand () % 6) + 1;

                if(((rand()% 20) + 6) < giocatore_corrente -> fortuna){
                    printf("\nCOLPO CRITICO! Grazie alla tua fortuna fai più danni!\n");
                    danno_inflitto =danno_inflitto + 6;
                }
                printf("Hai attaccato il nemico e hai inflitto %d danni!\n", danno_inflitto);
                vita_nemico = vita_nemico - danno_inflitto;
                break;
            

            case 2: 
                utilizza_oggetto(indice_giocatore);
                break;
            
            default:
                printf("\nScelta non valida!\n");   
        }

        if(vita_nemico > 0){
            printf("\nIl %s contrattacca!\n", nomi_nemici[nemico_corrente]);
            int danno_subito = attacco_nemico + (rand() % 3);

            printf("Subisci %d danni alla tua Difesa Psichica!\n", danno_subito); 
            giocatore_corrente -> difesa_psichica = giocatore_corrente -> difesa_psichica - danno_subito;
    
        } else {
            printf("\nHai sconfitto il nemico!!\n");

            if(nemico_corrente == demotorzone){
                printf("\nHAI SCONFITTO IL DEMOTORZONE!\n");
                demotorzone_sconfitto = 1;

                strcpy(ultimi_vincitori[indice_vincitore_corrente], giocatore_corrente -> nome);

                indice_vincitore_corrente++;
                if(indice_vincitore_corrente >= 3){
                    indice_vincitore_corrente = 0;
                }
                return;
            }

        }

    }

    if(giocatore_corrente -> difesa_psichica <= 0){
        printf("\nSEI MORTO. Hai perso...\n");
    } else {
        printf("\nSei sopravvissuto allo scontro!\n");
    }

    int scomparsa_nemico = rand () % 100;

    if(scomparsa_nemico < 50){
        printf("\nIl nemico svanisce... la zona ora è libera.\n");

        if(giocatore_corrente -> mondo == 0){
            giocatore_corrente -> pos_mondoreale -> nemico = nessun_nemico;
        } else {
            giocatore_corrente -> pos_soprasotto -> nemico = nessun_nemico;
        }
    } else {
        printf("Il nemico non è stato ucciso... è ancora qui.\n");
    }

}

static void avanza (int indice_giocatore){

    if(avanza_utilizzato == 1){
        printf("\nERRORE, sei già avanzato in questo turno. \n");
        return;
    }

    Giocatore* giocatore_corrente = giocatori[indice_giocatore];

        /* CONTROLLO DEI NEMICI */
        
    int controllo_nemico = 0;

    if(giocatore_corrente -> mondo == 0){
        if(giocatore_corrente -> pos_mondoreale -> nemico != nessun_nemico) {controllo_nemico = 1; }
    } 
    else {
        if (giocatore_corrente -> pos_soprasotto -> nemico != nessun_nemico) {controllo_nemico = 1; }
    }

    if (controllo_nemico == 1){
        printf("\nATTENZIONE NEMICO! Devi combatterlo prima di avanzare. \n");
        combatti(indice_giocatore); 
    }

        /* AVANZAMENTO NELLE DUE DIMENSIONI
        -Controlliamo in quale dimensione sta il giocatore e facciamolo avanzare 
         se esiste una zona successiva. */

    if(giocatore_corrente -> mondo == 0){
        if(giocatore_corrente -> pos_mondoreale -> prossima_zona == NULL){
            printf("\nSei arrivato all'ultima zona del Mondo Reale. Non puoi andare oltre. \n");

        } else {
            giocatore_corrente -> pos_mondoreale = giocatore_corrente -> pos_mondoreale -> prossima_zona;
            giocatore_corrente -> pos_soprasotto = giocatore_corrente -> pos_soprasotto -> prossima_zona;
            printf("\nSei avanzato verso la zona successiva del Mondo Reale. \n");
            avanza_utilizzato = 1;
        }

    } else {
        if(giocatore_corrente -> pos_soprasotto -> prossima_zona == NULL){
            printf("\nSei arrivato all'ultima zona del SopraSotto. Non puoi andare oltre. \n");

        } else {
            giocatore_corrente -> pos_soprasotto = giocatore_corrente -> pos_soprasotto -> prossima_zona;
            giocatore_corrente -> pos_mondoreale = giocatore_corrente -> pos_mondoreale -> prossima_zona;
            printf("\nSei avanzato verso la zona successiva del SopraSotto. \n");
            avanza_utilizzato = 1;
        }
    }
}

static void indietreggia (int indice_giocatore){

    if(indietro_utilizzato == 1){
        printf("\nERRORE, sei già indietreggiato in questo turno. \n");
        return;
    }

    Giocatore* giocatore_corrente = giocatori[indice_giocatore];

        /* CONTROLLO DEI NEMICI */

    int controllo_nemico = 0;
    
    if(giocatore_corrente -> mondo == 0){
        if(giocatore_corrente -> pos_mondoreale -> nemico != nessun_nemico){controllo_nemico = 1;}
        
    } else {
        if(giocatore_corrente -> pos_soprasotto -> nemico != nessun_nemico){controllo_nemico = 1;}
    }

    if(controllo_nemico == 1){
       printf("\nATTENZIONE NEMICO! Devi combatterlo prima di indietreggiare. \n");
        combatti(indice_giocatore); 
    }

        /* INDIETREGGIAMENTO NELLE DUE DIMENSIONI
        -Controlliamo in quale dimensione sta il giocatore e facciamolo indietreggiare 
         se esiste una zona precedente. */

    if(giocatore_corrente -> mondo == 0){
        if(giocatore_corrente -> pos_mondoreale -> zona_precedente == NULL){
            printf("\nSei arrivato alla prima zona del Mondo Reale. Non puoi indietreggiare ancora. \n");
        } else {
            giocatore_corrente -> pos_mondoreale = giocatore_corrente -> pos_mondoreale -> zona_precedente;
            giocatore_corrente -> pos_soprasotto = giocatore_corrente -> pos_soprasotto -> zona_precedente;
            printf("\nSei indietreggiato verso la zona precedente del Mondo Reale. \n");
            indietro_utilizzato = 1;
        }
    } else {
        if(giocatore_corrente -> pos_soprasotto -> zona_precedente == NULL){
            printf("\nSei arrivato alla prima zona del SopraSotto. Non puoi indietreggiare ancora. \n");
        } else {
            giocatore_corrente -> pos_soprasotto = giocatore_corrente -> pos_soprasotto -> zona_precedente;
            giocatore_corrente -> pos_mondoreale = giocatore_corrente -> pos_mondoreale -> zona_precedente;
            printf("\nSei indietreggiato verso la zona precedente del SopraSotto. \n");
            indietro_utilizzato = 1;
        }
    }

}

static void cambia_mondo(int indice_giocatore){

    int lancio_dado = 0;
    Giocatore* giocatore_corrente = giocatori[indice_giocatore];

    /* GESTIONE IN BASE ALLA DIMENSIONE IN CUI SI TROVA IL GIOCATORE
    -Se si trova nel Mondo Reale conta come avanza, altrimenti no ma 
     bisogna utilizzare un dado e la fortuna del giocatore. */

    if(giocatore_corrente -> mondo == 0){

        if(avanza_utilizzato == 1){
            printf("\nERRORE, sei già avanzato in questo turno. \n");
            return;
        }
        
        if(giocatore_corrente -> pos_mondoreale -> nemico != nessun_nemico){
            printf("\nATTENZIONE NEMICO! Devi combatterlo prima di avanzare. \n");
            combatti(indice_giocatore); 
        } else {
            printf("\nCambi dimensione! Ora sei nel SopraSotto.\n");
            giocatore_corrente -> mondo = 1;
            avanza_utilizzato = 1;
        }

    } else {

        lancio_dado = rand() % 20 + 1;
        printf("Hai lanciato un dado, esce: %d! \n", lancio_dado);
        printf("La tua fortuna è: %d", giocatore_corrente -> fortuna);

        if(giocatore_corrente -> pos_soprasotto -> nemico != nessun_nemico){
            printf("\nStai scappando da un nemico!");
            printf("\nPer riuscire a scappare la tua fortuna deve essere maggiore del numero del dado.\n");
        } else {
            printf("\nPe riuscire a cambiare dimensione la tua fortuna deve essere maggiore del numero del dado.\n");
        }

        if(lancio_dado < giocatore_corrente -> fortuna){
            printf("\nSuccesso! Il numero è minore della tua fortuna!\n");
            giocatore_corrente -> mondo = 0;
        } else {
            printf("\nTentativo fallito! Il numero del dado è più alto della tua fortuna. \n");
            printf("Sei rimasto nel SopraSotto. \n");
        }

    }
    
}

static void stampa_giocatore(int indice_giocatore){

    Giocatore* giocatore_corrente = giocatori[indice_giocatore];

    printf("\nDATI DEL GIOCATORE: %s\n", giocatore_corrente -> nome);
    printf("Difesa Psichica: %d\n", giocatore_corrente -> difesa_psichica);
    printf("Attacco Psichico: %d\n", giocatore_corrente -> attacco_psichico);
    printf("Fortuna: %d\n", giocatore_corrente -> fortuna);

    printf("Posizione attuale: ");
    if(giocatore_corrente -> mondo == 0){
        if(giocatore_corrente -> pos_mondoreale != NULL){
            printf("Mondo Reale - %s\n", nomi_zone[giocatore_corrente -> pos_mondoreale -> tipo]);
        } else {
            printf("Mondo Reale - posizione sconosciuta...\n");
        }

    } else {
        if(giocatore_corrente -> pos_soprasotto != NULL){
            printf("SopraSotto - %s\n", nomi_zone[giocatore_corrente -> pos_soprasotto -> tipo]);
        } else {
            printf("SopraSotto - posizione sconosciuta...\n");
        }
    }

    printf("\nZaino: [ ");
    int zaino_vuoto = 1;
    for(int i = 0; i < 3; i++){
        if(giocatore_corrente -> zaino[i] != nessun_oggetto){
            printf("%s, ", nomi_oggetti[giocatore_corrente->zaino[i]]);
            zaino_vuoto = 0;
        }
    }

    if(zaino_vuoto) {
        printf("Vuoto");
    }

    printf(" ]\n");

}

static void stampa_zona_corrente(int indice_giocatore){

    Giocatore* giocatore_corrente = giocatori[indice_giocatore];

    printf("\nPOSIZIONE DEL GIOCATORE: %s\n", giocatore_corrente -> nome);
    
    if(giocatore_corrente -> mondo == 0){
        if(giocatore_corrente -> pos_mondoreale != NULL){
            printf("Dimensione: Mondo Reale\n");
            printf("Zona: %s\n", nomi_zone[giocatore_corrente -> pos_mondoreale -> tipo]);
            printf("Oggetti: %s\n", nomi_oggetti[giocatore_corrente -> pos_mondoreale -> oggetto]);
            printf("Nemico: %s\n", nomi_nemici[giocatore_corrente -> pos_mondoreale -> nemico]);
    
        }
    } else {
        if(giocatore_corrente -> pos_soprasotto != NULL){
            printf("Dimensione: SopraSotto\n");
            printf("Zona: %s\n", nomi_zone[giocatore_corrente -> pos_soprasotto -> tipo]);
            printf("Nemico: " );
                if(giocatore_corrente -> pos_soprasotto -> nemico == demotorzone){
                    printf("DEMOTORZONE!!!\n");
                } else {
                    printf("%s\n", nomi_nemici[giocatore_corrente -> pos_soprasotto -> nemico]);
                }

        }
    }

}

static void raccogli_oggetto(int indice_giocatore){

    Giocatore* giocatore_corrente = giocatori[indice_giocatore];

    if(giocatore_corrente -> mondo == 1){
        printf("\nNon ci sono oggetti da raccogliere qui nel SopraSotto!\n");
        return;
    }

    if(giocatore_corrente -> pos_mondoreale -> nemico != nessun_nemico){
        printf("\nC'è un nemico! %s ti sta per assalire devi prima combatterlo!\n", nomi_nemici[giocatore_corrente -> pos_mondoreale -> nemico]);
        return;
    }

    if(giocatore_corrente -> pos_mondoreale -> oggetto == nessun_oggetto){
        printf("\nNon c'è nessun oggetto da raccogliere qui!\n");
        return;
    }

    int slot_libero = -1;

    for(int i = 0; i < 3; i++){
        if(giocatore_corrente -> zaino[i] == nessun_oggetto){
            slot_libero = i;
            break;
        }
    }

    if(slot_libero == -1){
        printf("\nIl tuo zaino è pieno! Hai già 3 oggetti.\n");
        printf("Devi usare o scartare qualcosa prima di raccogliere altro. \n");
        return;
    }


    Tipo_oggetto object = giocatore_corrente -> pos_mondoreale -> oggetto;
    giocatore_corrente -> zaino[slot_libero] = object;
    
    printf("\nHai raccolto: %s!\n", nomi_oggetti[giocatore_corrente -> pos_mondoreale -> oggetto]);
    giocatore_corrente -> pos_mondoreale -> oggetto = nessun_oggetto;

    printf("Consiglio: ");
    switch(object) {
        case bicicletta:
            printf("forse meglio utilizzarla nel SopraSotto...\n");
            break;

        case maglietta_fuocoinferno:
            printf("Forse ti potrebbe proteggere dai danni mentali? Meglio tenerla...\n");
            break;

        case bussola:
            printf("Se vai verso il nemico hai bisogno di più forza, questa potrebbe aiutarti un po'.\n");
            break;
        
        case schitarrata_metallica:
            printf("Attenzione, questa potrebbe essere il tuo asso nella manica... ");
            printf("\nChe abbia senso usarla contro il nemico più forte? Meglio tenerla fino alla fine.\n");
            break;
        default:
            printf("Un oggetto misterioso...");
    }

    printf("\nOggetto inserito nello slot %d dello zaino.\n", slot_libero);
}

static void passa(int indice_giocatore){

    printf("\n%s passa il turno. \n", giocatori[indice_giocatore] -> nome);
}

/* FUNZIONI PRINCPALI ------------------------------------------- */

void imposta_gioco() {
    
    for(int i = 0; i < 4; i++){
        if(giocatori[i] != NULL){
            free(giocatori[i]);
            giocatori[i] = NULL;
        }
    }

    int undici_assegnato = 0;
    numero_giocatori = 0;

    do{
        /* NUMERO GIOCATORI
        -Stabiliamo che il numero di giocatori sia nel range accettabile. */

        printf("Inserisci il numero di giocatori da 1 a 4: ");
        if (scanf("%d", &numero_giocatori) != 1){
            while(getchar() != '\n');
        }

    }while(numero_giocatori < 1 || numero_giocatori > 4);

        /* CICLO CREAZIONE GIOCATORI*/

    for (int i = 0; i < numero_giocatori; i++){
        printf("\nCreazione Giocatore %d \n", i + 1);

        /* ALLOCAZIONE MEMORIA
        -Creiamo dello spazio in memoria per ogni giocatore, e stabiliamo
         le sue statistiche e modifiche a queste. */

        giocatori[i] = (Giocatore*) malloc(sizeof(Giocatore));
        if (giocatori[i] == NULL) exit(1);

        printf("Inserisci nome: ");
        scanf("%s", giocatori[i]->nome);
        while(getchar() != '\n');

        giocatori[i] -> attacco_psichico = rand() %20 + 1;
        giocatori[i] -> difesa_psichica = rand() %20 + 1;
        giocatori[i] -> fortuna = rand() %20 + 1;

        /*INIZIALIZZAZIONE PER EVITARE CRASH
        -Inizializziamo i puntatori e lo zaino per evitare che contengano valori
         casuali che potrebbero compromettere l'esecuzione. */

        giocatori[i] -> mondo = 0;
        giocatori [i] -> pos_mondoreale = NULL;
        giocatori [i] -> pos_soprasotto = NULL;
        for(int j = 0; j < 3; j++){
            giocatori[i] -> zaino[j] = nessun_oggetto;
        }

        printf("\nLanciamo un dado per le tue statistiche %s...\n", giocatori[i] -> nome);
        printf("Il tuo attacco è: %d \n", giocatori[i] -> attacco_psichico);
        printf("La tua difesa è: %d \n", giocatori[i] -> difesa_psichica);
        printf("La tua fortuna è: %d \n", giocatori[i] -> fortuna);

    int scelta_valida = 0;
    int scelta_classe = 0;

        /*VALIDITA' DELLA SCELTA
        -Utilizziamo un do-while per assicurarci che nel caso l'opzione non velga scelta correttamente
        il ciclo venga ripetuto e vengano scelte correttamente le modalità. */

    do{
        printf("\nScegli la modalità e le rispettive statistiche: \n");
        printf("1- Modalità stregone: +3 Attacco -3 Difesa. \n");
        printf("2- Modalità protettore: -3 Attacco  +3 Difesa. \n");
        printf("3- Modalità saggio: nessuna modifica alle statistiche. \n");


        /*VERIFICA PER UNDICIVIRGOLACINQUE*/

        if(undici_assegnato == 0) {
            printf("4- Modalità UndiciVirgolaCinque (x1): +4 Attacco +4 Difesa -7 Fortuna \n");
        }

        printf("\nScelta: ");
        if(scanf("%d", &scelta_classe) != 1){
            printf("\nERRORE, inserisci un numero!\n");
            while(getchar() != '\n');
            continue;
        }
        
        /*SWITCH CASE PER STATISTICHE E LA LORO GESTIONE
        -Modifichiamo le statstiche in base alla scelta in input, e si controlla che i valori
         non vadano in negativo, modificando anche il nome del giocatore in "UndiciVirgolaCinque". */

        switch(scelta_classe){
            case 1:
                giocatori[i] -> attacco_psichico =  giocatori[i] -> attacco_psichico + 3;
                giocatori[i] -> difesa_psichica = giocatori[i] -> difesa_psichica - 3;
                
                if(giocatori[i] -> difesa_psichica < 0){ giocatori[i] -> difesa_psichica = 0; }
                scelta_valida = 1;

                printf("\nIl tuo attacco è: %d \n", giocatori[i] -> attacco_psichico);
                printf("La tua difesa è: %d \n", giocatori[i] -> difesa_psichica);
                printf("La tua fortuna è: %d \n", giocatori[i] -> fortuna);

                break;

            case 2:
                giocatori[i] -> attacco_psichico =  giocatori[i] -> attacco_psichico - 3;
                giocatori[i] -> difesa_psichica = giocatori[i] -> difesa_psichica + 3;
                
                if(giocatori[i] -> attacco_psichico < 0){ giocatori[i] -> attacco_psichico = 0; }
                scelta_valida = 1;

                printf("\nIl tuo attacco è: %d \n", giocatori[i] -> attacco_psichico);
                printf("La tua difesa è: %d \n", giocatori[i] -> difesa_psichica);
                printf("La tua fortuna è: %d \n", giocatori[i] -> fortuna);

                break;

            case 3:
                scelta_valida = 1;

                printf("\nIl tuo attacco è: %d \n", giocatori[i] -> attacco_psichico);
                printf("La tua difesa è: %d \n", giocatori[i] -> difesa_psichica);
                printf("La tua fortuna è: %d \n", giocatori[i] -> fortuna);

                break;

            case 4:
                if(undici_assegnato == 0){
                    giocatori[i] -> attacco_psichico =  giocatori[i] -> attacco_psichico + 4;
                    giocatori[i] -> difesa_psichica = giocatori[i] -> difesa_psichica + 4;
                    giocatori[i] -> fortuna = giocatori[i] -> fortuna - 7;
                    strcpy(giocatori[i] -> nome, "UndiciVirgolaCinque");

                    if(giocatori[i] -> fortuna < 0){ giocatori[i] -> fortuna = 0; }
                    undici_assegnato = 1;
                    scelta_valida = 1;

                    printf("\nIl tuo attacco è: %d \n", giocatori[i] -> attacco_psichico);
                    printf("La tua difesa è: %d \n", giocatori[i] -> difesa_psichica);
                    printf("La tua fortuna è: %d \n", giocatori[i] -> fortuna);

                } else {
                    printf("\nERRORE, il ruolo di UndiciVirgolaCinque è già stato assegnato una volta. \n");
                    scelta_valida = 0;
                  }
                break;

            default:
                printf("\nScelta non valida! \n");
                scelta_valida = 0;
        }

    }while(scelta_valida == 0);

    }
        /* GESTIONE MAPPA */ 

    int scelta_menu_mappa = 0;

    do {
        printf("\nGestione della MAPPA \n");
        printf("1- Genera mappa casuale (15 zone)\n");
        printf("2- Inserisci zona manualmente. \n");
        printf("3- Cancella zona. \n");
        printf("4- Stampa mappa. \n");
        printf("5- Stampa zona. \n");
        printf("6- Chiudi menù e torna a quello principale.\n");
        printf("Scelta: ");

        if(scanf("%d", &scelta_menu_mappa) != 1){
            while(getchar() != '\n');
            continue;
        }

        switch(scelta_menu_mappa){
            case 1:
                genera_mappa();
                printf("\nMappa generata! \n");
                break;

            case 2:
                inserisci_zona();
                break;

            case 3:
                cancella_zona();
                break;

            case 4:
                stampa_mappa();
                break;


            case 5:
                stampa_zona();
                break;

            case 6:
                if (prima_zona_mondoreale == NULL){
                    printf("Non hai generato una mappa! \n");
                    scelta_menu_mappa = 0;
                } else {

                    int conta_zone = 0;
                    int conta_demotorzone = 0;

                    Zona_soprasotto* zona_corrente = prima_zona_soprasotto;
                    while (zona_corrente != NULL){
                        conta_zone++;
                        if(zona_corrente -> nemico == demotorzone){
                            conta_demotorzone++;
                        }
                        zona_corrente = zona_corrente -> prossima_zona;
                    }

                    if(conta_zone < 15){
                        printf("\nErrore, la mappa ha solo %d zone. Devono essere almeno 15. \n", conta_zone);
                        scelta_menu_mappa = 0;
                    } 
                    else if(conta_demotorzone != 1){
                        printf("\nERRORE, ci sono %d Demotorzone nel SopraSotto! \n", conta_demotorzone);
                        printf("\nDeve esserci UNO E UNO SOLO Demotorzone! \n");
                        printf("Usa Cancella Zona e Inserisci Zona per impostare correttamente il gioco. \n");
                        scelta_menu_mappa = 0;     
                    } 
                    else {
                        printf("\nMappa confermata, configurazione terminata. \n");
                        mappa_chiusa = 1;
                    }

                }
                break;

            default:
            printf("Scelta non valida. \n");

        } 

    } while (scelta_menu_mappa != 6);



}

void gioca() {

    if (mappa_chiusa == 0){
        printf("\nERRORE, il gioco non è ancora stato impostato! \n");
        return;
    }

    printf("\nPARTITA INIZIATA. Entriamo ad Occhinz... \n");

    for(int i = 0; i < numero_giocatori; i++){
        if(giocatori[i] != NULL){
            giocatori[i] -> mondo = 0;
            giocatori[i] -> pos_mondoreale = prima_zona_mondoreale;
            giocatori[i] -> pos_soprasotto = prima_zona_soprasotto;
        }
    }

    int ordine_turni[4] = {0, 1, 2, 3};
    int partita_in_corso = 1;

    while(partita_in_corso){
        printf("\nINIZIO NUOVO ROUND.\n");

        for(int i = 0; i < numero_giocatori; i++){
            int random = rand() % numero_giocatori;
            int temp = ordine_turni[i];
            ordine_turni[i] = ordine_turni[random];
            ordine_turni[random] = temp; 
        }

        for(int j = 0; j < numero_giocatori; j++){
            int indice_corrente = ordine_turni[j];

            if(giocatori[indice_corrente] == NULL) { continue; }
            if(giocatori[indice_corrente] -> difesa_psichica <= 0) { continue;}

            printf("\nTURNO DEL GIOCATORE: %s\n", giocatori[indice_corrente] -> nome);

            turno_attuale = 1 ;
            avanza_utilizzato = 0;
            indietro_utilizzato = 0;

            while(turno_attuale == 1){
                int scelta_azione = 0;

                printf("\nCosa vuoi fare?\n");
                printf("1- Avanza\n");
                printf("2- Indietreggia\n");
                printf("3- Cambia Mondo\n");
                printf("4- Combatti\n");
                printf("5- Usa Oggetto\n");
                printf("6- Raccogli Oggetto\n");
                printf("7- Stampa Giocatore\n");
                printf("8- Stampa Zona Attuale\n");
                printf("9- Passa Turno\n");
                printf("Scelta: ");
                
                if (scanf("%d", &scelta_azione) != 1) {
                    while(getchar() != '\n'); 
                    continue;
                }

                switch(scelta_azione){
                    case 1:
                        avanza(indice_corrente);
                        break;

                    case 2:
                        indietreggia(indice_corrente);
                        break;

                    case 3:
                        cambia_mondo(indice_corrente);
                        break;

                    case 4:
                        combatti(indice_corrente);
                        break;

                    case 5:
                        utilizza_oggetto(indice_corrente);
                        break;
                    case 6:
                        raccogli_oggetto(indice_corrente);
                        break;
                    
                    case 7:
                        stampa_giocatore(indice_corrente);
                        break;
                    
                    case 8:
                    stampa_zona_corrente(indice_corrente);
                        break;

                    case 9:
                        passa(indice_corrente);
                        turno_attuale = 0;
                        break;

                    default:
                        printf("\nAzione non valida.\n");
                }

                if(giocatori[indice_corrente] -> difesa_psichica <= 0){
                    printf("\n%s è caduto a terra... fine del turno.\n", giocatori[indice_corrente] -> nome);
                    free(giocatori[indice_corrente]);
                    giocatori[indice_corrente] = NULL;
                    turno_attuale = 0;
                }

            }

        }

        if(demotorzone_sconfitto == 1){

            printf("\nVITTORIA!!! Hai sconfitto il Demotorzone! \n");
            printf("\n Il gioco termina qui. \n");
            partita_in_corso = 0;
        }

        int giocatori_vivi = 0;
        for(int i = 0; i < numero_giocatori; i++){
            if(giocatori[i] != NULL && giocatori[i] -> difesa_psichica > 0){
                giocatori_vivi++;
            }
        }

        if(giocatori_vivi == 0){
            printf("\nTutti i giocatori sono morti...\n");
            printf("Il gioco termina con una sconfitta...\n");
            partita_in_corso = 0;
        }

    }

}

void termina_gioco() {
    printf("\n-------------------------------------\n");
    printf("    GRAZIE PER AVER GIOCATO!    \n");
    printf("\n-------------------------------------\n");
    exit(0);
}

void crediti() {
    printf("\n------- CREDITI -------\n");
    printf("\nSviluppato da: Shawn Cionca\n");
    printf("Matricola: 390633\n");
    printf("Esame di Programmazione Procedurale - 2025/2026\n");

    printf("\n------- ULTIMI 3 VINCITORI -------\n");

    for(int i = 0; i < 3; i++){
        printf("%d, %s\n", i + 1, ultimi_vincitori[i]);
    }

    printf("\n-------------------------------------\n");
}
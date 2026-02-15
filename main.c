#include "gamelib.h"
#include <stdio.h>

int main (){
    int scelta_utente = 0;
    srand(time(NULL));
    system("clear");

    printf("\n--------------------------------------\n");
    printf(" COSE STRANE STAGIONE 6! \n");
    printf(" Progetto Programmazione Procedurale");
    printf("\n--------------------------------------\n");

    do {
        printf("\n---- MENU PRINCIPALE ----\n");
        printf("1- Imposta Gioco\n");
        printf("2- Gioca\n");
        printf("3- Termina Gioco\n");
        printf("4- Crediti\n");
        printf("Inserisci la tua scelta: ");

        /*PULIZIA BUFFER
        -Nel caso di valori errati, diversi dai numeri interi 1-4
         puliamo da eventuali caratteri inseriti che possono rimanere nel buffer. */
        
         if(scanf("%d", &scelta_utente) != 1) {
            printf("\n ERRORE! Inserise solo numeri INTERI!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (scelta_utente){
            case 1:
            imposta_gioco();
            break;
            case 2:
            gioca();
            break;
            case 3:
            termina_gioco();
            break;
            case 4:
            crediti();
            break;
            default:
                printf("\n ERRORE! Scelta non valida, inserisci numero da 1 a 4.\n");
        }

    }while(scelta_utente != 3);

    return 0;
}
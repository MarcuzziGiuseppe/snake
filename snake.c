/* Marcuzzi Giuseppe 21/10/2022 - 23/10/2022
    TO DO
    --> "cancellare" la posizione precedente della testa
    --> algoritmo IA (pensavo di farlo andare sempre a dx quando poteva e quando no controllava se sotto c'era spazio se c'era andava sotto e poi a dx appena poteva altrimenti andava sopra e a dx appena poteva)
    --> Concludere il menù
    --> Distanza Colonne ed ampiezza di esse fissa o meno
    --> Usando i file aggiungere le lingue
    // to do molto improbabili da fare
    --> Una specie di caricamento (LOADING... magari scritto pure in modo carino)
    --> aggiungere varie difficoltà (con i muri che si spostano)
    --> altro
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <time.h>

#define altezzaCampo 10
#define larghezzaCampo 25
#define ampiezzaMinimaSpazioPerPassare 1
#define ampiezzaMassimaSpazioPerPassare 2
#define distanzaMinimaTraColonne 4 // non minore di 2
#define distanzaMassimaTraColonne 6

// serve per salvarsi la posizione della testa
typedef struct _posizione {
    int posizioneX=0;
    int posizioneY=1;
    char simboloCheIndicaLaTesta='?';

} posizione;

// funzioni "strane"
void fermaStampa(); // non fa altro che impedire che le stampe successive vengano eseguite
int randomNumber(int max, int min);

// int setOrGetPunteggio(int serOrGet); //0 = set, 1 = get

// gestione del campo
posizione creazioneCampo(char (*matrix)[larghezzaCampo], posizione testaSerpente);
void stampaCampo(char (*matrix)[larghezzaCampo], int punteggio, posizione testaSerpente);

// gestione movimento
char recevimentoMovimentoSpostamento();
int spostamento(char (*matrix)[larghezzaCampo], char direction, posizione *testaSerpente);

int main(int argc, char const *argv[]) {
    srand(time(NULL));

    bool esciDalGioco = false;
    bool sceltaErrata = false;

    posizione testaSerpernte;
    int punteggio=0;

    printf("Ciao, ben venuto in snake\n");
    do {
        printf("Menu' di gioco:\n--> 1) Giocare = 1\n--> 2) Manuale = 2\n--> 3) IA = 3\n--> 4) Partecipanti = 4\n--> 0) Esci dal programma = 0\nquando hai scelto premi il numero corrispondente\n");
        esciDalGioco=false;
        sceltaErrata=false;
        char sceltaPlayer;
        sceltaPlayer=getch();
        switch (sceltaPlayer) {
        case '1':{
            // Giocare
            // le parentesi grafe servono per evitare l'errore jump-to-case-label-in-switch-statement 
            // dovuto alla inizializzazione di variabili all'interno di un case dello switch ma che non vengono inizializzate nei case successivi
                system("CLS");
                char campo[altezzaCampo][larghezzaCampo];
                
                punteggio = randomNumber(100, 1); // attualmente random giusto per test

                testaSerpernte = creazioneCampo(campo, testaSerpernte);
                stampaCampo(campo, punteggio, testaSerpernte);

                bool arrivatoAllaFine=false;
                do {
                    int risultatoSpostamento = spostamento(campo, recevimentoMovimentoSpostamento(), &testaSerpernte);
                    if (risultatoSpostamento==-1) {
                        printf("Direzione non Consentita perche' andresti contro il muro o andresti fuori dalla mappa\n");
                    } else {
                        if (testaSerpernte.posizioneX==larghezzaCampo-1) {
                            printf("Sei arrivato alla fine del percorso\nComplimenti hai vinto\n");
                            arrivatoAllaFine=true;
                        }
                    }
                    stampaCampo(campo, punteggio, testaSerpernte);
                } while (arrivatoAllaFine==false);
                fermaStampa();
            } break;
        case '2':
            // Manuale
            system("CLS");
            printf("Manuale\n");
            fermaStampa();
            break;
        case '3':
            // IA
            system("CLS");
            printf("IA\n");
            fermaStampa();
            break;
        case '4':
            // Partecipanti
            system("CLS");
            printf("Credits\n");
            fermaStampa();
            break;
        case '0':
            // esce dal programma
            esciDalGioco=true;
            break;
        default:
            sceltaErrata=true;
            system("CLS");
            printf("Scelta errata\n\n");
            break;
        }
        if (sceltaErrata!=true) {
            system("CLS");
        }
    } while (esciDalGioco==false);
    
    return 0;
}

void fermaStampa() {
    // getchar mi peremette di non dover premere invio dopo aver scritto ma ovviamnete prende solo il primo carattere
    printf("Premi un tasto per continuare\n");
    char fermaStampa = getchar();
}

int randomNumber(int max, int min){	

	return rand() % (max - min + 1 ) + min;
}

posizione creazioneCampo(char (*matrix)[larghezzaCampo], posizione testaSerpente) {
    for (size_t i = 0; i < altezzaCampo; i++) {
        for (size_t j = 0; j < larghezzaCampo; j++) {
            matrix[i][j]='#';
        }
    }
    // sostituisco il valore inserito con ' ' per avere il campo interno vuoto
    for (size_t i = 1; i < altezzaCampo-1; i++) {
        for (size_t j = 1; j < larghezzaCampo-1; j++) {
            matrix[i][j]=' ';
        }
    }

    // creazione punto di partenza e di arrivo
    int coordinataYDiParrtenza = randomNumber(altezzaCampo-2, 1);
    matrix[coordinataYDiParrtenza][0]=testaSerpente.simboloCheIndicaLaTesta; // partenza
    testaSerpente.posizioneY=coordinataYDiParrtenza;
    matrix[randomNumber(altezzaCampo-2,1)][larghezzaCampo-1]=' '; // arrivo

    // creazione colonne
    for (size_t i = randomNumber(distanzaMassimaTraColonne,distanzaMinimaTraColonne); i < larghezzaCampo-3; i+=randomNumber(distanzaMassimaTraColonne,distanzaMinimaTraColonne)) {
        for (size_t j = 1; j < altezzaCampo-1; j++) {
            matrix[j][i]='#';
        }
        int grandezzaSpazioPerPassare = randomNumber(ampiezzaMassimaSpazioPerPassare, ampiezzaMinimaSpazioPerPassare); // quanto grande lo spazio per passare
        int puntoPerLoSpazio = randomNumber(altezzaCampo-ampiezzaMassimaSpazioPerPassare, 1); // punto in cui creare lo spazio
        for (size_t j = 0; j < grandezzaSpazioPerPassare; j++) {
            if (puntoPerLoSpazio<altezzaCampo-1) {
                matrix[puntoPerLoSpazio][i]=' ';
                puntoPerLoSpazio++;
            }
        }
    }
    return testaSerpente;
}

void stampaCampo(char (*matrix)[larghezzaCampo], int punteggio, posizione testaSerpente) {

    matrix[testaSerpente.posizioneY][testaSerpente.posizioneX]=testaSerpente.simboloCheIndicaLaTesta;
    for (size_t i = 0; i < altezzaCampo; i++) {
        for (size_t j = 0; j < larghezzaCampo; j++) {
            printf("%c", matrix[i][j]);
        }
        if (i==0) {
            printf("  Il tuo punteggio attuale e': %d", punteggio);
        }
        printf("\n");
    }

    // questa parte serve unicamnete per rendere piu' carina la separazione tra le 2 stampe
    for (size_t i = 0; i < larghezzaCampo; i++) {
        printf("-");
    }
    printf("\n");
}

char recevimentoMovimentoSpostamento() {
    bool sceltaErrata=false;
    char spostamento;
    do {
        sceltaErrata=false;
        printf("Dimmi dove muovermi: w=su s=giu' d=destra a=sinistra ");
        spostamento = _getch();
        //scanf("%c", &spostamento);
        if (spostamento!='w' && spostamento != 's' && spostamento != 'a' && spostamento!='d') {
            sceltaErrata=true;
            printf("\nQuesta non e' una direzione consentita\n");
        }
    } while (sceltaErrata==true);
    printf(" Direzione scelta: %c\n", spostamento);
    return spostamento;
}

int spostamento(char (*matrix)[larghezzaCampo], char direction, posizione *testaSerpente) {

    switch (direction) {
    case 'w':
        //alto
        if (matrix[testaSerpente->posizioneY-1][testaSerpente->posizioneX]!='#') {
            testaSerpente->posizioneY-=1;
            return 0;
        }
        break;
    case 's':
        //basso
        if (matrix[testaSerpente->posizioneY+1][testaSerpente->posizioneX]!='#') {
            testaSerpente->posizioneY+=1;
            return 0;
        }
        break;
    case 'a':
        //sinistra
        if (matrix[testaSerpente->posizioneY][testaSerpente->posizioneX-1]!='#' && testaSerpente->posizioneX-1>0) {
            testaSerpente->posizioneX-=1;
            return 0;
        }
        break;
    case 'd':
        //destra
        if (matrix[testaSerpente->posizioneY][testaSerpente->posizioneX+1]!='#') {
            testaSerpente->posizioneX= testaSerpente->posizioneX+1;
            return 0;
        }
        break;
    default:
        break;
    }
    return -1; // al momento mi serve per capire se è andato contro un muro o se cerca di uscire dalla mappa
}
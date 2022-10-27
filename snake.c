/* Marcuzzi Giuseppe 21/10/2022 - 26/10/2022
    Campagnolo Alberto
    TO DO
    --> Usando i file aggiungere le lingue fatto solo per l'Italiano, aggiungere anche l'inglese (Giuseppe)
    --> algoritmo IA (tengo la mano sul muro di dx) (Alberto (+ Giuseppe?))
    --> Concludere il menù
    --> Distanza Colonne ed ampiezza di esse fissa o meno (bho non se se lo faremo, non  trovo molta utilità)
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
#include <string.h>


#define altezzaCampo 10
#define larghezzaCampo 25
#define ampiezzaMinimaSpazioPerPassare 1
#define ampiezzaMassimaSpazioPerPassare 2
#define distanzaMinimaTraColonne 4 // non minore di 2
#define distanzaMassimaTraColonne 6
#define lunghezzaMassimaStringa 100

#define linguaDefault "italiano" // solamente temporanea al momento

// serve per salvarsi la posizione della testa
typedef struct {
    int posizioneX;
    int posizioneY;
    char simboloCheIndicaLaTesta;

} posizione;

// funzioni "strane"
void fermaStampa(); // non fa altro che impedire che le stampe successive vengano eseguite
int randomNumber(int max, int min);
void clearScreen();

// int setOrGetPunteggio(int serOrGet); //0 = set, 1 = get

// gestione del campo
posizione creazioneCampo(char (*matrix)[larghezzaCampo], posizione testaSerpente);
void stampaCampo(char (*matrix)[larghezzaCampo], int punteggio, posizione testaSerpente);

// gestione movimento
char recevimentoMovimentoSpostamento();
int spostamento(char (*matrix)[larghezzaCampo], char direction, posizione *testaSerpente);

// gestione dei file
bool controlloDeiFile(char lingua[]);
void stampaAVideoIlTesto(char paragrafo[], char lingua[]);

int main(int argc, char const *argv[]) {
    srand(time(NULL));

    bool esciDalGioco = false;
    bool sceltaErrata = false;

    posizione testaSerpernte;
    testaSerpernte.posizioneX=0;
    testaSerpernte.posizioneY=0;
    testaSerpernte.simboloCheIndicaLaTesta='?';
    int punteggio=0;

    stampaAVideoIlTesto("introduzione", linguaDefault);
    do {
        stampaAVideoIlTesto("menu", linguaDefault);
        esciDalGioco=false;
        sceltaErrata=false;
        char sceltaPlayer;
        sceltaPlayer=getch();
        switch (sceltaPlayer) {
        case '1':{
            // Giocare
            // le parentesi grafe servono per evitare l'errore jump-to-case-label-in-switch-statement 
            // dovuto alla inizializzazione di variabili all'interno di un case dello switch ma che non vengono inizializzate nei case successivi
                clearScreen();
                char campo[altezzaCampo][larghezzaCampo];
                
                punteggio = randomNumber(100, 1); // attualmente random giusto per test

                testaSerpernte = creazioneCampo(campo, testaSerpernte);
                stampaCampo(campo, punteggio, testaSerpernte);

                bool arrivatoAllaFine=false;
                do {
                    int risultatoSpostamento = spostamento(campo, recevimentoMovimentoSpostamento(), &testaSerpernte);
                    if (risultatoSpostamento==-1) {
                        stampaAVideoIlTesto("direzioneNonConsentita", linguaDefault);
                    } else {
                        if (testaSerpernte.posizioneX==larghezzaCampo-1) {
                            stampaAVideoIlTesto("vittoriaGioco", linguaDefault);
                            arrivatoAllaFine=true;
                        }
                    }
                    stampaCampo(campo, punteggio, testaSerpernte);
                } while (arrivatoAllaFine==false);
                fermaStampa();
            } break;
        case '2':
            // Manuale
            clearScreen();
            stampaAVideoIlTesto("regole", linguaDefault);
            fermaStampa();
            break;
        case '3':
            // IA
            clearScreen();
            stampaAVideoIlTesto("IA", linguaDefault);
            fermaStampa();
            break;
        case '4':
            // Partecipanti
            clearScreen();
            stampaAVideoIlTesto("partecipanti",linguaDefault);
            fermaStampa();
            break;
        case '0':
            // esce dal programma
            esciDalGioco=true;
            break;
        default:
            sceltaErrata=true;
            clearScreen();
            stampaAVideoIlTesto("sceltaErrata",linguaDefault);
            break;
        }
        if (sceltaErrata!=true) {
            clearScreen();
        }
    } while (esciDalGioco==false);
    
    return 0;
}

void fermaStampa() {
    // getchar mi peremette di non dover premere invio dopo aver scritto ma ovviamnete prende solo il primo carattere
    stampaAVideoIlTesto("fermaStampa",linguaDefault);
    char fermaStampa = getchar();
}

int randomNumber(int max, int min){	

	return rand() % (max - min + 1 ) + min;
}

void clearScreen() {
    system("@cls||clear");
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
            printf("  Score: %d", punteggio);
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
        stampaAVideoIlTesto("direzione", linguaDefault);
        spostamento = _getch();
        //scanf("%c", &spostamento);
        if (spostamento!='w' && spostamento != 's' && spostamento != 'a' && spostamento!='d') {
            sceltaErrata=true;
            stampaAVideoIlTesto("direzioneSbagliata", linguaDefault);
        }
    } while (sceltaErrata==true);
    // printf(" Direzione scelta: %c\n", spostamento);
    return spostamento;
}

int spostamento(char (*matrix)[larghezzaCampo], char direction, posizione *testaSerpente) {

    int posizioneXOriginale = testaSerpente->posizioneX;
    int posizioneYOriginale = testaSerpente->posizioneY;

    bool spostamenteoRiuscito=false;

    switch (direction) {
    case 'w':
        //alto
        if (matrix[testaSerpente->posizioneY-1][testaSerpente->posizioneX]!='#') {
            testaSerpente->posizioneY-=1;
            spostamenteoRiuscito=true;
        }
        break;
    case 's':
        //basso
        if (matrix[testaSerpente->posizioneY+1][testaSerpente->posizioneX]!='#') {
            testaSerpente->posizioneY+=1;
            spostamenteoRiuscito=true;
        }
        break;
    case 'a':
        //sinistra
        if (matrix[testaSerpente->posizioneY][testaSerpente->posizioneX-1]!='#' && testaSerpente->posizioneX-1>0) {
            testaSerpente->posizioneX-=1;
            spostamenteoRiuscito=true;
        }
        break;
    case 'd':
        //destra
        if (matrix[testaSerpente->posizioneY][testaSerpente->posizioneX+1]!='#') {
            testaSerpente->posizioneX= testaSerpente->posizioneX+1;
            spostamenteoRiuscito=true;
        }
        break;
    default:
        break;
    }

    if (spostamenteoRiuscito==true) {
        matrix[posizioneYOriginale][posizioneXOriginale]=' ';
        return 0;
    }
    
    return -1; // al momento mi serve per capire se è andato contro un muro o se cerca di uscire dalla mappa
}

bool controlloDeiFile(char lingua[]){
    char stringaTemp[100];
    strcpy(stringaTemp, lingua);
    strcat(stringaTemp, ".txt");
    FILE *fin = fopen(stringaTemp, "r");
    if (fin!=NULL) {
        return true;
        fclose(fin);
    }
    return false;
}

void stampaAVideoIlTesto(char paragrafo[], char lingua[]){

    if (controlloDeiFile(lingua) == false){
        printf("Error! opening file, file not in the same folder of the program");
        fermaStampa();
        // Programma esce se non esite il file nella stessa cartella del programma-*+
        exit(1);
    } else {
        char stringaTemp[100];
        strcpy(stringaTemp, lingua);
        strcat(stringaTemp, ".txt");
        FILE *fin = fopen(stringaTemp, "r");

        char stringaDaStampare[100]="bho"; 
        char tagIniziale[100]="<";
        strcat(strcat(tagIniziale, paragrafo), ">\n");
        char tagFinale[100]= "</";
        strcat(strcat(tagFinale, paragrafo), ">\n");
        do {
            fgets(stringaDaStampare, sizeof(stringaDaStampare), fin);
            if (strcmp(stringaDaStampare,tagIniziale)==0) {
                do {
                    fgets(stringaDaStampare, sizeof(stringaDaStampare), fin);
                    if ((strcmp(stringaDaStampare, tagIniziale)!=0) && (strcmp(stringaDaStampare, tagFinale)!=0)){
                        printf("%s", stringaDaStampare);
                    }
                } while (strcmp(stringaDaStampare, tagFinale)!=0);
            }
        } while ((strcmp(stringaDaStampare, tagFinale)!=0));
        fclose(fin);
    }
	return;
}

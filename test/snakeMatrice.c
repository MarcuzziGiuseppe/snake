/* Marcuzzi Giuseppe 21/10/2022 - 26/10/2022
    Campagnolo Alberto
    TO DO
    --> algoritmo IA (tengo la mano sul muro di dx) (Alberto (+ Giuseppe?))
    --> Concludere il menù
    --> Distanza Colonne ed ampiezza di esse fissa o meno (bho non se se lo faremo, non  trovo molta utilità)
    // to do molto improbabili da fare
    --> aggiungere varie difficoltà (con i muri che si spostano)
    --> altro
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
// a linux conio.h non piace. Per cosa lo usiamo conio?? conio la usiamo per getch
// possibile soluzione: https://www.includehelp.com/c-programs/gotoxy-clrscr-getch-getche-for-gcc-linux.aspx
// switch based on os system: https://stackoverflow.com/questions/6649936/c-compiling-on-windows-and-linux-ifdef-switch
// questa roba dovrebbe piacerli
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#elif __linux__
    #include <unistd.h>
    #include <termios.h>
static struct termios old, new;
void initTermios(int echo) {
    tcgetattr(0, &old); //grab old terminal i/o settings
    new = old; //make new settings same as old settings
    new.c_lflag &= ~ICANON; //disable buffered i/o
    new.c_lflag &= echo ? ECHO : ~ECHO; //set echo mode
    tcsetattr(0, TCSANOW, &new); //apply terminal io settings
}

/* Restore old terminal i/o settings */
void resetTermios(void) {
    tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) {
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}
char getch(void) {
    return getch_(0);
}
#endif

#define altezzaCampo 10
#define larghezzaCampo 25
#define ampiezzaMinimaSpazioPerPassare 1
#define ampiezzaMassimaSpazioPerPassare 2
#define distanzaMinimaTraColonne 4 // non minore di 2
#define distanzaMassimaTraColonne 6
#define lunghezzaMassimaStringa 100

// source di #undef https://stackoverflow.com/questions/9274500/redefining-or-changing-macro-value
//#define linguaScelta "italiano"

// variabili globali
int numero_monete = 0;
int punti = 0;
int linguaTesto=0; // 0=Italiano 1=Inglese

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
void loading();

// gestione del campo
int controllaPunteggio(int coordinatay, int coordinatax, char (*matrix)[larghezzaCampo]);
void generaElemento(char elemento, int numeroMassimo, char (*matrix)[larghezzaCampo]);
posizione creazioneCampo(char (*matrix)[larghezzaCampo], posizione testaSerpente);
void stampaCampo(char (*matrix)[larghezzaCampo], int punteggio, posizione testaSerpente);

// gestione movimento
char recevimentoMovimentoSpostamento();
int spostamento(char (*matrix)[larghezzaCampo], char direction, posizione *testaSerpente);

// gestione dei file
bool controlloDeiFile(char lingua[]);
void stampaAVideoIlTesto(char paragrafo[], int linguaTesto);
void stampaLoading(char paragrafo[]);

int main(int argc, char const *argv[]) {
    srand(time(NULL));

    bool esciDalGioco = false;
    bool sceltaErrata = false;

    stampaAVideoIlTesto("introduzione", linguaTesto);
    do {
        punti = 0;
        
        stampaAVideoIlTesto("menu", linguaTesto);
        esciDalGioco=false;
        sceltaErrata=false;
        char sceltaPlayer;
        sceltaPlayer=getch();
        switch (sceltaPlayer) {
        case '1':
            // Giocare
            // le parentesi grafe servono per evitare l'errore jump-to-case-label-in-switch-statement 
            // dovuto alla inizializzazione di variabili all'interno di un case dello switch ma che non vengono inizializzate nei case successivi
            
                // "costruzione" della struct (tipo il constractor nelle classsi)
                posizione testaSerpernte;
                testaSerpernte.posizioneX=0;
                testaSerpernte.posizioneY=0;
                testaSerpernte.simboloCheIndicaLaTesta='?';

                clearScreen();
                char campo[altezzaCampo][larghezzaCampo];
                testaSerpernte = creazioneCampo(campo, testaSerpernte);
                loading();
                stampaCampo(campo, punti, testaSerpernte);

                bool arrivatoAllaFine=false;
                do {
                    int risultatoSpostamento = spostamento(campo, recevimentoMovimentoSpostamento(), &testaSerpernte);
                    if (risultatoSpostamento==-1) {
                        stampaAVideoIlTesto("direzioneNonConsentita", linguaTesto);
                    } else {
                        if(risultatoSpostamento==-2){
                            break;
                        }
                    }
                    if (testaSerpernte.posizioneX==larghezzaCampo-1) {
                        stampaAVideoIlTesto("vittoriaGioco", linguaTesto);
                        arrivatoAllaFine=true;
                    }
                    
                    stampaCampo(campo, punti, testaSerpernte);
                } while (arrivatoAllaFine==false);
                fermaStampa();
            break;
        case '2':
            // Manuale
            clearScreen();
            stampaAVideoIlTesto("regole", linguaTesto);
            fermaStampa();
            break;
        case '3':
            // IA
            clearScreen();
            stampaAVideoIlTesto("IA", linguaTesto);
            fermaStampa();
            break;
        case '4':
            // Partecipanti
            clearScreen();
            stampaAVideoIlTesto("partecipanti",linguaTesto);
            fermaStampa();
            break;
        case '5':
            // Partecipanti
            bool linguaSelezionataCorrettamente = true;
            do {
                clearScreen();
                stampaAVideoIlTesto("lingue",linguaTesto);
                sceltaPlayer = getch();
                switch (sceltaPlayer) {
                case '1':
                    linguaTesto=0;
                    break;
                case '2':
                    linguaTesto=1;
                    break;
                default:
                    linguaSelezionataCorrettamente=false;
                    stampaAVideoIlTesto("sceltaErrata", linguaTesto);
                    fermaStampa();
                    break;
                }
            } while (linguaSelezionataCorrettamente!=true);
            fermaStampa(); // solo temporanea finchè non metto che imposta la lingua
            break;
        case '0':
            // esce dal programma
            esciDalGioco=true;
            break;
        default:
            sceltaErrata=true;
            clearScreen();
            stampaAVideoIlTesto("sceltaErrata",linguaTesto);
            break;
        }
        clearScreen();
    } while (esciDalGioco==false);
    
    return 0;
}

void fermaStampa() {
    // getchar mi peremette di non dover premere invio dopo aver scritto ma ovviamnete prende solo il primo carattere
    stampaAVideoIlTesto("fermaStampa",linguaTesto);
    char fermaStampa = getchar();
}

int randomNumber(int max, int min){	

	return rand() % (max - min + 1 ) + min;
}

void clearScreen() {
    system("@cls||clear");
}

void generaElemento(char elemento, int numeroMassimo, char (*matrix)[larghezzaCampo]){
	int elementi_totali = randomNumber(numeroMassimo,1);	

	for(int z=0; z < elementi_totali; z++){
		int elementox = randomNumber(larghezzaCampo-2,1);
		int elementoy = randomNumber(altezzaCampo-2,1);
		while(matrix[elementoy][elementox] == '#'){
			elementoy = randomNumber(larghezzaCampo-2,1);
			elementox = randomNumber(altezzaCampo-2,1);
		}
		matrix[elementoy][elementox] = elemento;
	}
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
        int grandezzaSpazioPerPassare = randomNumber(ampiezzaMassimaSpazioPerPassare, ampiezzaMinimaSpazioPerPassare);
	int puntoPerLoSpazio = randomNumber(altezzaCampo-ampiezzaMassimaSpazioPerPassare, 1); // punto in cui creare lo spazio
        for (size_t j = 0; j < grandezzaSpazioPerPassare; j++) {
            if (puntoPerLoSpazio<altezzaCampo-1) {
                matrix[puntoPerLoSpazio][i]=' ';
                puntoPerLoSpazio++;
            }
        }
    }
    generaElemento('$',7,matrix);
    generaElemento('!',2,matrix);
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
        stampaAVideoIlTesto("direzione", linguaTesto);
        spostamento = _getch();
        //scanf("%c", &spostamento);
        if (spostamento!='w' && spostamento != 's' && spostamento != 'a' && spostamento!='d' && spostamento!='q') {
            sceltaErrata=true;
            stampaAVideoIlTesto("direzioneSbagliata", linguaTesto);
        }
    } while (sceltaErrata==true);
    // printf(" Direzione scelta: %c\n", spostamento);
    return spostamento;
}

int controllaPunteggio(int coordinatax, int coordinatay, char (*matrix)[larghezzaCampo]){
	if(matrix[coordinatay][coordinatax] == '$'){
		numero_monete++;
	} else if(matrix[coordinatay][coordinatax] == '!'){
		numero_monete = numero_monete/2;
	} else {
		punti--;
	}
}

int spostamento(char (*matrix)[larghezzaCampo], char direction, posizione *testaSerpente) {

    int posizioneXOriginale = testaSerpente->posizioneX;
    int posizioneYOriginale = testaSerpente->posizioneY;

    bool spostamenteoRiuscito=false;

    switch (direction) {
    case 'w':
        //alto
        if (matrix[testaSerpente->posizioneY-1][testaSerpente->posizioneX]!='#') {
            controllaPunteggio(testaSerpente->posizioneY-1, testaSerpente->posizioneX, matrix);
            testaSerpente->posizioneY-=1;
            spostamenteoRiuscito=true;
        }
        break;
    case 's':
        //basso
        if (matrix[testaSerpente->posizioneY+1][testaSerpente->posizioneX]!='#') {
            controllaPunteggio(testaSerpente->posizioneY+1, testaSerpente->posizioneX, matrix);
            testaSerpente->posizioneY+=1;
            spostamenteoRiuscito=true;
        }
        break;
    case 'a':
        //sinistra
        if (matrix[testaSerpente->posizioneY][testaSerpente->posizioneX-1]!='#' && testaSerpente->posizioneX-1>0) {
            controllaPunteggio(testaSerpente->posizioneY, testaSerpente->posizioneX-1, matrix); 
            testaSerpente->posizioneX-=1;
            spostamenteoRiuscito=true;
        }
        break;
    case 'd':
        //destra
        if (matrix[testaSerpente->posizioneY][testaSerpente->posizioneX+1]!='#') {
            controllaPunteggio(testaSerpente->posizioneY, testaSerpente->posizioneX+1, matrix);
            testaSerpente->posizioneX= testaSerpente->posizioneX+1;
            spostamenteoRiuscito=true;
        }
        break;
    case 'q':
        return -2;
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

void stampaAVideoIlTesto(char paragrafo[], int linguaTesto){

    char lingua[]="italiano";
    if (linguaTesto==1) {
        strcpy(lingua, "inglese");
    }

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

void stampaLoading(char paragrafo[]){

    char lingua[]="loadings";

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

        char stringaDaStampare[300]="bho"; 
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

void loading(){
	char a = ' ', b = '#';
	printf("\n\n\n\n");
    if (controlloDeiFile("loadings") == false){
        printf("Error! opening file, file not in the same folder of the program");
        fermaStampa();
        // Programma esce se non esite il file nella stessa cartella del programma
        exit(1);
    } else {
        // leggo la prima riga del file loadings.txt per sapere quanti loading ci sono nel file
        int numeroLoadings=1;
        FILE *fin = fopen("loadings.txt", "r");
        char numeroDiLoadingsLetti[10]="5"; 
        fgets(numeroDiLoadingsLetti, sizeof(numeroDiLoadingsLetti), fin);
        numeroLoadings=atoi(numeroDiLoadingsLetti);
        fclose(fin);

        char c[10];
        sprintf(c, "%ld", randomNumber(numeroLoadings, 1));
        stampaLoading(c);
        printf("\t\t\t[");
        
        for(int i = 0;i < 26; i++){	
            printf("%c", a);
        }
        printf("]");
        printf("\r");
        printf("\t\t\t");

        for(int i=0;i < 26; i++){
            if(i==0){
                printf("[");
            }
            printf("%c", b);
            fflush(stdout);
            Sleep(i==24 ? randomNumber(300,100)*10 : randomNumber(20,10)*10);
        }
    }

    clearScreen();
}

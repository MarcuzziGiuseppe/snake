/*  Marcuzzi Giuseppe  894698
    Campagnolo Alberto 897569
    Faccin Leonardo 896837
    21/10/2022 - 02/12/2022
    TO DO
    --> aggiungere la possibilità far mettere dall'utente un labirinto inerito da tastiera (Fax)
    --> Documentazione
    --> algoritmo IA (path finding, DFS) (Campa)
    // to do molto improbabili da fare
    --> altro
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
// switch based on os system: https://stackoverflow.com/questions/6649936/c-compiling-on-windows-and-linux-ifdef-switch
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #define fineTag ">\n"
#else
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
    void Sleep(int tempo) {
        sleep(tempo/700); // controllare meglio il valore per cui dividere (non so non mi paice molto il tempo di attesa)
    }
    #define fineTag ">\r\n"
#endif

#define altezzaCampo 10
#define larghezzaCampo 25
#define ampiezzaMinimaSpazioPerPassare 1
#define ampiezzaMassimaSpazioPerPassare 2
#define distanzaMinimaTraColonne 3 // non minore di 2
#define distanzaMassimaTraColonne 6

// variabili Globali
int linguaTesto=0; // 0=Italiano 1=Inglese

// Struct varie
typedef struct {
    // posizioni e simboli snake + fine
    int posizioneXSnake;
    int posizioneYSnake;
    int posizioneXSnakeOriginali;
    int posizioneYSnakeOriginali;
    int posizioneXFine;
    int posizioneYFine;
    char simboloScia;
    char simboloSnakeTesta;
    char simboloSnakeCorpo;
    int numeroPezziCorpo;
    bool removeBody;
    // campo e "potenziamenti"
    char campoVergine[altezzaCampo][larghezzaCampo];
    char campoSporco[altezzaCampo][larghezzaCampo];
    int punti;
    int numero_monete;
    int numberOfDrill;
    // salvataggio movimenti
    char moves[300];
    int indiceMoves;
} posizione;

// funzioni "strane"
void fermaStampa(); // non fa altro che impedire che le stampe successive vengano eseguite
int randomNumber(int max, int min);
void clearScreen();

// gestione Campo
void creazioneCampo(posizione* campo);
void stampaCampo(posizione* campo, bool isIA);
void generaElemento(char elemento, int numeroMassimo, char (*matrix)[larghezzaCampo]);
void controllaPunteggio(int coordinatay, int coordinatax, posizione* campo);


// gestione movimento
char recevimentoMovimentoSpostamento();
int spostamento(char direction, posizione *campo, bool isIA);

// gestione dei file/dati
bool controlloDeiFile(char nomeFile[]);
void stampaAVideoIlTesto(char paragrafo[], bool isLoading);
void loading();
int nextNumberOfTag(char *file);
void saveReplay(posizione datiPartita);
void watchReplay();

// IA
bool algoritmoIA(posizione* dati, char direzione, char direzioneIniziale);
int checkDictions(posizione *campo, char direzione);
void goToPointAndGetIt(posizione *campo);
char checkPositions(posizione posizioni);
char checkEnd (posizione* campo, char direzioneOriginale, char direzione);

int main(int argc, char const *argv[]) {
    srand(time(NULL));
    posizione datiPartita;
    datiPartita.simboloSnakeTesta='?';
    datiPartita.simboloSnakeCorpo='0';

    bool arrivatoAllaFine=false; // indica se sono arrivato alla fine del labirinto
    bool esciDalGioco = false;

    do {
        // "costruzione" della struct (tipo il constractor nelle classi)
        datiPartita.posizioneXSnake=0;
        datiPartita.posizioneYSnake=0;
        datiPartita.posizioneXSnakeOriginali=0;
        datiPartita.posizioneYSnakeOriginali=0;
        datiPartita.posizioneXFine=0;
        datiPartita.posizioneYFine=0;
        datiPartita.numeroPezziCorpo=0;
        datiPartita.removeBody = false;
        
        datiPartita.punti=0;
        datiPartita.numero_monete=0;
        datiPartita.numberOfDrill=0;

        datiPartita.indiceMoves=0;

        stampaAVideoIlTesto("introduzione", false);
        stampaAVideoIlTesto("menu", false);
        esciDalGioco=false;
        char sceltaPlayer;
        sceltaPlayer=getch();
        switch (sceltaPlayer) {
        case '1':{
            // Giocare
            // le parentesi grafe servono per evitare l'errore jump-to-case-label-in-switch-statement 
            // dovuto alla inizializzazione di variabili all'interno di un case dello switch ma che non vengono inizializzate nei case successivi

            // riechiesta di creazione campo dimensioni forniti dall'utente oppure se generato random (stessa cosa da fare per l'IA)
                // temporaneamente lasciato come di default (ovvere come prima delle modifiche)
            creazioneCampo(&datiPartita);
            clearScreen();
            loading();
            stampaCampo(&datiPartita, false);
            char direzione;
            int risultatoSpostamento;
            arrivatoAllaFine=false;
            do {
                direzione = recevimentoMovimentoSpostamento();
                risultatoSpostamento = spostamento(direzione, &datiPartita, false);
                if(risultatoSpostamento==-2){
                    break;
                } else if (risultatoSpostamento != -1) {
                    datiPartita.moves[datiPartita.indiceMoves]=direzione;
                    datiPartita.indiceMoves++;
                }
                clearScreen();
                if ((datiPartita.posizioneXFine==datiPartita.posizioneXSnake) && (datiPartita.posizioneYFine==datiPartita.posizioneYSnake)) {
					stampaAVideoIlTesto("vittoriaGioco", false);
                    printf("Score ==>%d\n", (datiPartita.punti+(datiPartita.numero_monete*10)));
					arrivatoAllaFine=true;
                    saveReplay(datiPartita);
                }
                stampaCampo(&datiPartita, false);
            } while (arrivatoAllaFine==false);
            fermaStampa();
		} break;
        case '2':
            // Manuale
            clearScreen();
            stampaAVideoIlTesto("regole", false);
            fermaStampa();
            break;
        case '3': {
            // IA
            creazioneCampo(&datiPartita);
            clearScreen();
            //loading();
            stampaCampo(&datiPartita, true);


            datiPartita.indiceMoves=0;
            for (size_t i = 0; i < altezzaCampo; i++) {
                for (size_t k = 0; k < larghezzaCampo; k++) {
                    if (datiPartita.campoSporco[i][k]==datiPartita.simboloSnakeTesta) {
                        datiPartita.posizioneYSnake=i;
                        datiPartita.posizioneXSnake=k;
                    } else if (datiPartita.campoSporco[i][k]=='_') {
                        datiPartita.posizioneYFine=i;
                        datiPartita.posizioneXFine=k;
                    }
                }
            }
            algoritmoIA(&datiPartita, checkPositions(datiPartita), checkPositions(datiPartita));

            printf("Moves: ");
            for (int i = 0; i < datiPartita.indiceMoves; i++) {
                printf("%c", datiPartita.moves[i]);
            }
            saveReplay(datiPartita);
            printf("\n");
            fermaStampa();
            } break;
        case '4':
            // replay
            clearScreen();
            watchReplay();
            fermaStampa();
            break;
        case '5':
            // Partecipanti
            clearScreen();
            stampaAVideoIlTesto("partecipanti",false);
            fermaStampa();
            break;
        case '6':{
            // lingue
            bool linguaSelezionataCorrettamente = true;
            do {
                linguaSelezionataCorrettamente = true;
                clearScreen();
                stampaAVideoIlTesto("lingue",false);
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
                    stampaAVideoIlTesto("sceltaErrata", false);
                    fermaStampa();
                    break;
                }
            } while (linguaSelezionataCorrettamente!=true);
            fermaStampa();
		} break;
        case '0':
            // esce dal programma
            esciDalGioco=true;
            break;
        default:
            clearScreen();
            stampaAVideoIlTesto("sceltaErrata",false);
            break;
        }
        clearScreen();
        for (size_t i = 0; i < datiPartita.indiceMoves; i++) {
            datiPartita.moves[i]='\0';
        }
        
    } while (esciDalGioco==false);
    


    return 0;
}

void fermaStampa() {
    // getchar mi peremette di non dover premere invio dopo aver scritto ma ovviamnete prende solo il primo carattere
    stampaAVideoIlTesto("fermaStampa",false);
    char fermaStampa = getch();
}

int randomNumber(int max, int min){	

	return rand() % (max - min + 1 ) + min;
}

void clearScreen() {
    system("@cls||clear");
}

void creazioneCampo(posizione* campo) {

    clearScreen();
    printf("INSERISCI IL CAMPO\n");

    char rigaCheMiPassa[larghezzaCampo];
    bool ultimaRiga=false;
    for (size_t i = 0; i < altezzaCampo && ultimaRiga==false; i++) {
        fgets(rigaCheMiPassa, larghezzaCampo+1, stdin);
        if (rigaCheMiPassa[0]!='\n') {
            strcpy(campo->campoVergine[i], rigaCheMiPassa);
            if (i==altezzaCampo-1 && rigaCheMiPassa[larghezzaCampo-1]=='#') {
                ultimaRiga=true;
            }
        } else {
            i--;
        }
    }

    for (size_t i = 0; i < altezzaCampo; i++) {
        for (size_t k = 0; k < larghezzaCampo; k++) {
            campo->campoSporco[i][k]=campo->campoVergine[i][k];
        }
    }

    return ;
}

void stampaCampo(posizione* campo, bool isIA) {
    if (isIA==true) {
        campo->simboloScia='.';
    } else {
        campo->simboloScia=' ';
    }
    
    // for per "resettare" il corpo di snake
    int tempY = campo->posizioneYSnake;
    int tempX = campo->posizioneXSnake;
    for (size_t i = campo->indiceMoves; i > 0; i--) {
        switch (campo->moves[i-1]) {
            case 'w':
                campo->campoSporco[tempY+1][tempX] = campo->simboloScia;
                tempY++;
                break;
            case 's':
                campo->campoSporco[tempY-1][tempX] = campo->simboloScia;
                tempY--;
                break;
            case 'a':
                campo->campoSporco[tempY][tempX+1] = campo->simboloScia;
                tempX++;
                break;
            case 'd':
                campo->campoSporco[tempY][tempX-1] = campo->simboloScia;
                tempX--;
                break;
            default:
                break;
        }
    }

    campo->campoSporco[campo->posizioneYSnake][campo->posizioneXSnake]=campo->simboloSnakeTesta;
    
    int index = 0;
    bool termina=false;
    tempY = campo->posizioneYSnake;
    tempX = campo->posizioneXSnake;
    for (size_t i = campo->indiceMoves; index < campo->numeroPezziCorpo; i--) {
        switch (campo->moves[i-1]) {
            case 'w':
                if (campo->campoSporco[tempY+1][tempX]!=campo->simboloSnakeTesta) {
                    campo->campoSporco[tempY+1][tempX] = campo->simboloSnakeCorpo;
                } else {
                    termina=true;
                }
                tempY++;
                break;
            case 's':
                if (campo->campoSporco[tempY-1][tempX]!=campo->simboloSnakeTesta) {
                    campo->campoSporco[tempY-1][tempX] = campo->simboloSnakeCorpo;
                } else {
                    termina=true;
                }
                tempY--;
                break;
            case 'a':
                if (campo->campoSporco[tempY][tempX+1]!=campo->simboloSnakeTesta) {
                    campo->campoSporco[tempY][tempX+1] = campo->simboloSnakeCorpo;
                } else {
                    termina=true;
                }
                tempX++;
                break;
            case 'd':
                if (campo->campoSporco[tempY][tempX-1]!=campo->simboloSnakeTesta) {
                    campo->campoSporco[tempY][tempX-1] = campo->simboloSnakeCorpo;
                } else {
                    termina=true;
                }
                tempX--;
                break;
            default:
                break;
        }
        if (termina==true) {
            campo->numero_monete -= campo->numeroPezziCorpo - index;
            if (campo->numero_monete<0) {
                campo->numero_monete=0;
            }
            
            break;
        }
        index++;
    }
    campo->numeroPezziCorpo=index;
    printf("\n");
    for (size_t i = 0; i < altezzaCampo; i++) {
        for (size_t j = 0; j < larghezzaCampo; j++) {
            printf("%c", campo->campoSporco[i][j]);
        }
	printf("\n");
    }
    // questa parte serve unicamnete per rendere piu' carina la separazione tra le 2 stampe
    for (size_t i = 0; i < larghezzaCampo; i++) {
        if(i==3){
            printf("|%d|", campo->punti);
        } else if(i==8){
            printf("|%d|", campo->numero_monete);
        } else if (i==13){
            printf("|%d|", campo->numberOfDrill);
        } else {
            printf(" ");
        }
    }
    printf("\n");
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

void controllaPunteggio(int coordinatay, int coordinatax, posizione* campo){
	if(campo->campoSporco[coordinatay][coordinatax] == '$'){
		campo->numero_monete++;
        campo->numeroPezziCorpo++;
	} else if(campo->campoSporco[coordinatay][coordinatax] == '!'){
		campo->numero_monete = campo->numero_monete/2;
        if (campo->numeroPezziCorpo>0) {
            campo->numeroPezziCorpo--;
        }
	} else if (campo->campoSporco[coordinatay][coordinatax] == 'T') {
        campo->numberOfDrill +=3;
    } else  if (campo->campoSporco[coordinatay][coordinatax] == campo->simboloSnakeCorpo) {
        // dico che mi sono mangiato e quindi devo togliere i pezzi del corpo
        if (campo->numeroPezziCorpo>1) {
            campo->removeBody=true;
        }
    } else {
        campo->punti--;
    }
}

char recevimentoMovimentoSpostamento() {
    bool sceltaErrata=false;
    char spostamento;
    do {
        sceltaErrata=false;
        spostamento = getch();
        //scanf("%c", &spostamento);
        if (spostamento!='w' && spostamento != 's' && spostamento != 'a' && spostamento!='d' && spostamento!='q') {
            sceltaErrata=true;
        }
    } while (sceltaErrata==true);
    // printf(" Direzione scelta: %c\n", spostamento);
    return spostamento;
}

int spostamento(char direction, posizione *campo, bool isIA) {

    int posizioneXOriginale = campo->posizioneXSnake;
    int posizioneYOriginale = campo->posizioneYSnake;

    bool spostamenteoRiuscito=false;

    switch (direction) {
    case 'w':
        //alto
        if ((campo->campoSporco[campo->posizioneYSnake-1][campo->posizioneXSnake]!='#' || campo->numberOfDrill>=1) && campo->posizioneYSnake-1>=0) {
            if (campo->numberOfDrill>=1 && campo->campoSporco[campo->posizioneYSnake-1][campo->posizioneXSnake]=='#') {
                campo->numberOfDrill--;
            }
            controllaPunteggio(campo->posizioneYSnake-1, campo->posizioneXSnake, campo);
            campo->posizioneYSnake-=1;
            spostamenteoRiuscito=true;
        }
        break;
    case 's':
        //basso
        if ((campo->campoSporco[campo->posizioneYSnake+1][campo->posizioneXSnake]!='#' || campo->numberOfDrill>=1) && campo->posizioneYSnake+1<altezzaCampo) {
            if (campo->numberOfDrill>=1 && campo->campoSporco[campo->posizioneYSnake+1][campo->posizioneXSnake]=='#') {
                campo->numberOfDrill--;
            }
            controllaPunteggio(campo->posizioneYSnake+1, campo->posizioneXSnake, campo);
            campo->posizioneYSnake+=1;
            spostamenteoRiuscito=true;
        }
        break;
    case 'a':
        //sinistra
        if (((campo->campoSporco[campo->posizioneYSnake][campo->posizioneXSnake-1]!='#' && campo->posizioneXSnake-1>0) || campo->numberOfDrill>=1) && campo->posizioneXSnake-1>=0) {
            if (campo->numberOfDrill>=1 && campo->campoSporco[campo->posizioneYSnake][campo->posizioneXSnake-1]=='#') {
                campo->numberOfDrill--;
            }
            controllaPunteggio(campo->posizioneYSnake, campo->posizioneXSnake-1, campo);
            campo->posizioneXSnake-=1;
            spostamenteoRiuscito=true;
        }
        break;
    case 'd':
        //destra
        if ((campo->campoSporco[campo->posizioneYSnake][campo->posizioneXSnake+1]!='#' || campo->numberOfDrill>=1) && campo->posizioneXSnake+1<larghezzaCampo) {
            if (campo->numberOfDrill>=1 && campo->campoSporco[campo->posizioneYSnake][campo->posizioneXSnake+1]=='#') {
                campo->numberOfDrill--;
            }
            controllaPunteggio(campo->posizioneYSnake, campo->posizioneXSnake+1, campo);
            campo->posizioneXSnake= campo->posizioneXSnake+1;
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
        if (isIA==true) {
            campo->campoSporco[posizioneYOriginale][posizioneXOriginale]='.';
        } else {
            campo->campoSporco[posizioneYOriginale][posizioneXOriginale]=' ';
        }
        return 0;
    }
    
    return -1; // al momento mi serve per capire se è andato contro un muro o se cerca di uscire dalla mappa
}

bool controlloDeiFile(char nomeFile[]){
    char stringaTemp[100];
    strcpy(stringaTemp, nomeFile);
    strcat(stringaTemp, ".txt");
    FILE *fin = fopen(stringaTemp, "r");
    if (fin!=NULL) {
        fclose(fin);
        return true;
    }
    return false;
}

void stampaAVideoIlTesto(char paragrafo[], bool isLoading){

    char file[100];
    strcpy(file, "italiano");
    if (isLoading==true) {
        strcpy(file, "loadings");
    } else if (linguaTesto==1) {
        strcpy(file, "inglese");
    }

    if (controlloDeiFile(file) == false){
        printf("Error! opening file, file not in the same folder of the program\n");
        fermaStampa();
        // Programma esce se non esite il file nella stessa cartella del programma-*+
        exit(1);
    } else {
        strcat(file, ".txt");
        FILE *fin = fopen(file, "r");

        char stringaDaStampare[300]="bho"; 
        char tagIniziale[100]="<";
        strcat(strcat(tagIniziale, paragrafo), fineTag);
        char tagFinale[100]= "</";
        strcat(strcat(tagFinale, paragrafo), fineTag);
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
    // leggo la prima riga del file loadings.txt per sapere quanti loading ci sono nel file
    int numeroLoadings=nextNumberOfTag("loadings.txt")-1;

    char c[10];
    sprintf(c, "%d", randomNumber(numeroLoadings, 1));
    stampaAVideoIlTesto(c, true);
    printf("\n\t\t\t[");
    
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
    clearScreen();
}

int nextNumberOfTag(char *file) {
    char fileString[100];
    char initialTag[]="<";
    char secondTag[]="</";
    int fileNumber=1;
    char fileNumberChar[2];
    char tag1[7];
    char tag2[7];
    FILE *fin = fopen(file, "r");
    while (!feof(fin)) {
        // "pulisco" i tag
        for (size_t i = 0; i < sizeof(tag1); i++) {
            tag1[i]='\0';
            tag2[i]='\0';
        }
        //concateno le stringhe
        itoa(fileNumber, fileNumberChar, 10);
        // concateno il primo tag
        strcat(tag1, initialTag);
        strcat(tag1,fileNumberChar);
        strcat(tag1, fineTag);
        // concateno il secondo tag
        itoa(fileNumber, fileNumberChar, 10);
        secondTag[0]='<';
        strcat(tag2, secondTag);
        strcat(tag2,fileNumberChar);
        tag2[4]='\0';
        strcat(tag2, fineTag);

        fgets(fileString, sizeof(fileString), fin);
        // controllo se la stringa che legge è una stringa che mi interressa
        if (strcmp(fileString, tag1)==0) {
            // le 2 stringhe sono uguali
            fileNumber++;
            // leggo fino a quando non leggo il tag di fine
            bool endTagFinde=false;
            while (endTagFinde==false) {
                fgets(fileString, sizeof(fileString), fin);
                if (strcmp(fileString, tag2)==0) {
                    endTagFinde=true;
                }
            }
        }
    }
    fclose(fin);

    return fileNumber;
}

void saveReplay(posizione datiPartita) {
    // mi i replay nel file replays.txt
    if (controlloDeiFile("replays") == false) {
        clearScreen();
        printf("Error! opening file, file not in the same folder of the program\n");
        fermaStampa();
        // Programma esce se non esite il file nella stessa cartella del programma
        exit(1);
    } else {
        // mi salvo su un file il campo con i relativi movimenti
        int nextNumber = nextNumberOfTag("replays.txt");
        char initialTag[]="<";
        char secondTag[]="</";
        char fileNumberChar[2];
        char tag1[7] = {"\0"};
        char tag2[7] = {"\0"};
        //concateno le stringhe
        itoa(nextNumber, fileNumberChar, 10);
        // concateno il primo tag
        strcat(tag1, initialTag);
        strcat(tag1,fileNumberChar);
        strcat(tag1, fineTag);
        // concateno il secondo tag
        itoa(nextNumber, fileNumberChar, 10);
        secondTag[0]='<';
        strcat(tag2, secondTag);
        strcat(tag2,fileNumberChar);
        tag2[4]='\0';
        strcat(tag2, fineTag);
        char fieldLine[larghezzaCampo+1];
        fieldLine[larghezzaCampo]='\0';
        strcat(fieldLine, "\n");
        FILE *fout = fopen("replays.txt", "a");
        // scrivo il primo tag
        fprintf(fout, tag1);
        for (size_t i = 0; i < altezzaCampo; i++) {
            for (size_t k = 0; k < larghezzaCampo; k++) {
                fieldLine[k]=datiPartita.campoVergine[i][k];
            }
            fprintf(fout, fieldLine);
            fprintf(fout, "\n");
        }
        itoa(datiPartita.indiceMoves, fileNumberChar, 10);
        fprintf(fout, fileNumberChar);
        fprintf(fout, "\n");
        datiPartita.moves[datiPartita.indiceMoves]='\0';
        fprintf(fout, datiPartita.moves);
        fprintf(fout, "\n");
        tag2[6]='\0';
        fprintf(fout, tag2);
        fclose(fout);        
    }
    return ;
}

void watchReplay() {
    if (controlloDeiFile("replays") == false) {
        clearScreen();
        printf("Error! opening file, file not in the same folder of the program\n");
        fermaStampa();
        // Programma esce se non esite il file nella stessa cartella del programma
        exit(1);
    } else {
        // numero di replay disponibili
        int numberReplaysAvailable = nextNumberOfTag("replays.txt") -1;
        if (numberReplaysAvailable==0) {
            clearScreen();
            stampaAVideoIlTesto("replay3", false);
            return;
        }
        stampaAVideoIlTesto("replay", false);
        printf("%d ", numberReplaysAvailable);
        stampaAVideoIlTesto("replay2", false);
        int getNumber = 0;
        scanf("%d", &getNumber);
        if (getNumber>=1 && getNumber<=numberReplaysAvailable) {
            // mi ha dato un numero che esiste tra i miei replay
            char fileString[100];
            char initialTag[]="<";
            char fileNumberChar[2];
            char tag1[7];
            posizione parita;
            parita.posizioneXSnake=0;
            parita.posizioneYSnake=1;
            parita.simboloSnakeTesta='?';
            parita.simboloSnakeCorpo='0';

            // for fors inutile ma lo faccio per sicurezza (per "pulire" l'array)
            for (size_t i = 0; i < sizeof(tag1); i++) {
                tag1[i]='\0';
            }
            //concateno le stringhe
            itoa(getNumber, fileNumberChar, 10);
            // concateno il primo tag
            strcat(tag1, initialTag);
            strcat(tag1,fileNumberChar);
            strcat(tag1, fineTag);

            FILE *fin = fopen("replays.txt", "r");
            do {
                fgets(fileString, sizeof(fileString), fin);
            } while (strcmp(fileString, tag1)!=0);
            for (size_t i = 0; i < altezzaCampo; i++) {
                for (size_t k = 0; k < larghezzaCampo+1; k++) {
                    parita.campoSporco[i][k] = fgetc(fin);
                    if (parita.campoSporco[i][k]=='?') {
                    }
                }
                
            }
            fgets(fileString, sizeof(fileString), fin);
            parita.indiceMoves = atoi(fileString);
            fgets(parita.moves, sizeof(parita.moves), fin);
            fclose(fin);
            
            // ricreo il labirinto 
            for (size_t i = 0; i < altezzaCampo; i++) {
                if (parita.campoSporco[i][0]=='?') {
                    parita.posizioneXSnake=0;
                    parita.posizioneYSnake=i;
                }
                for (size_t k = 1; k < larghezzaCampo; k++) {
                    if (parita.campoSporco[i][k]=='.' || parita.campoSporco[i][k]=='?') {
                        parita.campoSporco[i][k]=' ';
                    }
                }
            }
            parita.campoSporco[parita.posizioneYSnake][parita.posizioneXSnake]=parita.simboloSnakeTesta;

            // fare scelta automatica o spostamenteo "manuale"
            char chose;
            do {
                clearScreen();
                stampaAVideoIlTesto("replay4", false);
                chose = getch();
                if (chose != '1' && chose !='2') {
                    clearScreen();
                    stampaAVideoIlTesto("sceltaErrata", false);
                    fermaStampa();
                }
            } while (chose!='1' && chose!='2');

            clearScreen();
            parita.punti=0;
            parita.numero_monete=0;
            parita.numberOfDrill=0;
            parita.numeroPezziCorpo=0;
            int numeroMosse = parita.indiceMoves;
            parita.indiceMoves=0;
            loading();
            for (size_t i = 0; i < numeroMosse+1; i++) {
                clearScreen();
                stampaCampo(&parita, true);
                spostamento(parita.moves[i], &parita, true);
                parita.indiceMoves++;
                if (chose=='1' && i<numeroMosse) {
                    fermaStampa();
                }
            }
            printf("Score ==>%d\n", (parita.punti +(parita.numero_monete*10)));
        } else {
            clearScreen();
            stampaAVideoIlTesto("sceltaErrata", false);
        }
    }
    return; 
}

bool algoritmoIA(posizione* dati, char direzione, char direzioneIniziale) {
    bool arrivatoAllaFine = false;
    bool spazioLiberoSotto = false;
    int piuOMeno=+1;
    if (direzioneIniziale=='a') {
        piuOMeno=-1;
    }

    direzione = checkEnd(dati, direzioneIniziale, direzione);
    
    int risultatoSpostamento = spostamento(direzione, dati, true);
    if (risultatoSpostamento==-1) {
        // sono andato contro un muro
        int contatore=0;
        for (size_t i = 1; i < altezzaCampo-1; i++) {
            if (dati->campoSporco[i][dati->posizioneXSnake+1]==' ' || dati->campoSporco[i][dati->posizioneXSnake+1]=='_' || dati->campoSporco[i][dati->posizioneXSnake+1]=='T' || dati->campoSporco[i][dati->posizioneXSnake+1]=='$' || dati->campoSporco[i][dati->posizioneXSnake+1]=='!') {
                if (!(dati->campoSporco[i-1][dati->posizioneXSnake+1]==' ' || dati->campoSporco[i-1][dati->posizioneXSnake+1]=='_' || dati->campoSporco[i-1][dati->posizioneXSnake+1]=='T' || dati->campoSporco[i-1][dati->posizioneXSnake+1]=='$' || dati->campoSporco[i-1][dati->posizioneXSnake+1]=='!')) {
                    contatore++;
                }
            }
        }
        
        if (contatore==1) {
            for (int i = dati->posizioneYSnake+1; i < altezzaCampo-1; i++) {
                if (dati->campoSporco[i][dati->posizioneXSnake+1]==' ' || dati->campoSporco[i][dati->posizioneXSnake+1]=='_' || dati->campoSporco[i][dati->posizioneXSnake+1]=='T' || dati->campoSporco[i][dati->posizioneXSnake+1]=='$' || dati->campoSporco[i][dati->posizioneXSnake+1]=='!') {
                    if (checkDictions(dati, 's')==0) {
                        direzione = 's';
                    } else {
                        // sono bloccato
                        printf("\nSONO BLOCCATO VERSO S\n");
                        fermaStampa();
                        return false;
                    }
                    spazioLiberoSotto = true;
                }
            }
            if (spazioLiberoSotto==false) {
                if (checkDictions(dati, 'w')==0) {
                    direzione = 'w';
                } else {
                    // sono bloccato
                    printf("\nSONO BLOCCATO VERSO W\n");
                    fermaStampa();
                    return false;
                }
            }
        } else {
            // ho più strade
            //printf("\nHO PIU STRADE, %d\n", contatore);
            //stampaCampo(dati, true);
            //fermaStampa();
            posizione **figli = malloc(sizeof(posizione)*2);
            int index=1;
            for (size_t i = 0; i < contatore; i++) {
                for (size_t j = index; j < altezzaCampo-1; j++) {
                    if (dati->campoSporco[j][dati->posizioneXSnake+1]==' ' || dati->campoSporco[j][dati->posizioneXSnake+1]=='_' || dati->campoSporco[j][dati->posizioneXSnake+1]=='T' || dati->campoSporco[j][dati->posizioneXSnake+1]=='$' || dati->campoSporco[j][dati->posizioneXSnake+1]=='!') {
                        if (!(dati->campoSporco[j-1][dati->posizioneXSnake+1]==' ' || dati->campoSporco[j-1][dati->posizioneXSnake+1]=='_' || dati->campoSporco[j-1][dati->posizioneXSnake+1]=='T' || dati->campoSporco[j-1][dati->posizioneXSnake+1]=='$' || dati->campoSporco[j-1][dati->posizioneXSnake+1]=='!') && dati->campoSporco[dati->posizioneYSnake+1][dati->posizioneXSnake]!='#') {
                            figli[i]=dati;
                            for (size_t k = 0; k < figli[i]->indiceMoves; k++) {
                                // for per essere sicuro di avere l'arrey di moves "resettato"
                                figli[i]->moves[k] = '\0';
                            }
                            figli[i]->campoSporco[0][0]='#';
                            figli[i]->indiceMoves = 0;
                            figli[i]->posizioneYSnakeOriginali=j;
                            figli[i]->posizioneYSnake=j;
                            figli[i]->posizioneXSnakeOriginali=dati->posizioneXSnake;
                            figli[i]->posizioneXSnake = dati->posizioneXSnake;
                            break;
                        }
                    }
                    index++;
                }
                bool result = algoritmoIA(figli[i], checkPositions(*figli[i]), checkPositions(*figli[i]));
                if (result==true) {
                    int dist, temp;
                    // si arriva ad una fine
                    if (dati->posizioneYSnake>figli[i]->posizioneYSnakeOriginali) {
                        // devo andare su
                        dist = dati->posizioneYSnake - figli[i]->posizioneYSnakeOriginali;
                        for (size_t k = 0; k < dist; k++) {
                            dati->moves[dati->indiceMoves] = 'w';
                            dati->indiceMoves++;
                            temp = spostamento('w', dati, true);
                            clearScreen();
                            stampaCampo(dati, true);
                        }
                        for (size_t k = 0; k < figli[i]->indiceMoves; k++) {
                            dati->indiceMoves++;
                            dati->moves[dati->indiceMoves] = figli[i]->moves[k];
                        }
                        for (size_t k = 0; k < dati->indiceMoves; k++) {
                            printf("%c", dati->moves[dati->indiceMoves]);
                        }
                    } else {
                        // devo andare giù
                        dist = figli[i]->posizioneYSnakeOriginali - dati->posizioneYSnake;
                        for (size_t k = 0; k < dist; k++) {
                            dati->moves[dati->indiceMoves] = 's';
                            dati->indiceMoves++;
                            temp = spostamento('s', dati, true);
                            clearScreen();
                            stampaCampo(dati, true);
                        }
                        for (size_t k = 0; k < figli[i]->indiceMoves; k++) {
                            dati->indiceMoves++;
                            dati->moves[dati->indiceMoves] = figli[i]->moves[k];
                        }
                        for (size_t k = 0; k < dati->indiceMoves; k++) {
                            printf("%c", dati->moves[dati->indiceMoves]);
                        }
                    }
                    break;
                } else {
                    return false;
                }
            }
        }
    } else {
        dati->moves[dati->indiceMoves] = direzione;
        dati->indiceMoves++;
        direzione = direzioneIniziale;
    }
    
    if ((direzione!=direzioneIniziale && dati->campoSporco[dati->posizioneYSnake][dati->posizioneXSnake+piuOMeno]==' ') || dati->campoSporco[dati->posizioneYSnake][dati->posizioneXSnake+piuOMeno]=='$' || dati->campoSporco[dati->posizioneYSnake][dati->posizioneXSnake+piuOMeno]=='_' || dati->campoSporco[dati->posizioneYSnake][dati->posizioneXSnake+piuOMeno]=='!') {
        direzione = direzioneIniziale;
    }
    
    
    if (dati->posizioneXFine==dati->posizioneXSnake && dati->posizioneYFine==dati->posizioneYSnake) {
        stampaAVideoIlTesto("vittoriaGioco", false);
        printf("Score ==>%d\n", (dati->punti+(dati->numero_monete*10)));
        arrivatoAllaFine=true;
        for (size_t i = 0; i < dati->indiceMoves; i++) {
            printf("%c", dati->moves[i]);
        }
        printf("\n");
    }
    if (arrivatoAllaFine!=true) {
        clearScreen();
    }
    
    stampaCampo(dati, true);
    if (arrivatoAllaFine==true) {
        fermaStampa();
    }
    
    //fermaStampa(); // per debug
    if (arrivatoAllaFine==false) {
        return algoritmoIA(dati, direzione, direzioneIniziale);
    }

    return arrivatoAllaFine;
}

int checkDictions(posizione *campo, char direzione) {

    bool possoSpostarmi=false;

    switch (direzione) {
    case 'w':
        //alto
        if ((campo->campoSporco[campo->posizioneYSnake-1][campo->posizioneXSnake]!='#' || campo->numberOfDrill>=1) && campo->posizioneYSnake-1>=0) {
            possoSpostarmi=true;
        }
        break;
    case 's':
        //basso
        if ((campo->campoSporco[campo->posizioneYSnake+1][campo->posizioneXSnake]!='#' || campo->numberOfDrill>=1) && campo->posizioneYSnake+1<altezzaCampo) {
            possoSpostarmi=true;
        }
        break;
    case 'a':
        //sinistra
        if (((campo->campoSporco[campo->posizioneYSnake][campo->posizioneXSnake-1]!='#' && campo->posizioneXSnake-1>0) || campo->numberOfDrill>=1) && campo->posizioneXSnake-1>=0) {
            possoSpostarmi=true;
        }
        break;
    case 'd':
        //destra
        if ((campo->campoSporco[campo->posizioneYSnake][campo->posizioneXSnake+1]!='#' || campo->numberOfDrill>=1) && campo->posizioneXSnake+1<larghezzaCampo) {
            possoSpostarmi=true;
        }
        break;
    default:
        break;
    }

    if (possoSpostarmi==true) {
        return 0;
    }
    
    return -1; // mi serve per capire se è andato contro un muro o se cerca di uscire dalla mappa
}

void goToPointAndGetIt(posizione *campo) {
    int numeroMoneteTrovate=0;
    char direzione;

    for (int i = campo->posizioneYSnake+1; i < altezzaCampo; i++) {
        // guarado se sotto c'e' almeno una moneta
        if (campo->campoSporco[i][campo->posizioneXSnake]=='$') {
            numeroMoneteTrovate++;
        }
    }
    if (numeroMoneteTrovate!=0) {
        // vado a prendermi i punti
        direzione='s';
        int numeroMoneteAttuali = 0;
        for (int i = campo->posizioneYSnake; numeroMoneteAttuali < numeroMoneteTrovate; i++) {
            if (campo->campoSporco[i+1][campo->posizioneXSnake]=='$') {
                numeroMoneteAttuali++;
            }
            campo->moves[campo->indiceMoves]=direzione;
            campo->indiceMoves++;
            spostamento(direzione, campo, true);
            clearScreen();
            stampaCampo(campo, true);
        }
    }

    // ora vado a vedere se sopra ho delle monete
    numeroMoneteTrovate=0;
    for (int i = campo->posizioneYSnake-1; i > 0; i--) {
        // guarado se sopra c'e' almeno una moneta
        if (campo->campoSporco[i][campo->posizioneXSnake]=='$') {
            numeroMoneteTrovate++;
        }
    }
    if (numeroMoneteTrovate!=0) {
        // vado a prendermi i punti
        direzione='w';
        int numeroMoneteAttuali = 0;
        for (int i = campo->posizioneYSnake-1; numeroMoneteAttuali < numeroMoneteTrovate; i--) {
            if (campo->campoSporco[i][campo->posizioneXSnake]=='$') {
                numeroMoneteAttuali++;
            }
            campo->moves[campo->indiceMoves]=direzione;
            campo->indiceMoves++;
            spostamento(direzione, campo, true);
            clearScreen();
            stampaCampo(campo, true);
        }
    }
}

char checkPositions(posizione posizioni) {
    // controllo se la fine si trova alla mia Dx o sotto di me o alla mia SX
    if (posizioni.posizioneXSnake<posizioni.posizioneXFine) {
        // la fine si trova alla mia dx
        return 'd';
    } else if (posizioni.posizioneXFine==posizioni.posizioneXSnake) {
        //si trova sotto di me
        return 's';
    }
    // la fine si trova alla mia sx
    return 'a';
}

char checkEnd (posizione* campo, char direzioneOriginale, char direzione) {
    // mi ritorna true se nella colonna successiva c'è la fine altrimenti false

    if (campo->posizioneYSnake==campo->posizioneYFine && campo->posizioneXSnake==campo->posizioneXFine) {
        direzione = direzioneOriginale;
    } else {
        for (int i = campo->posizioneYSnake+1; i < altezzaCampo; i++) {
            // guarado se sotto c'e' la fine
            if (campo->campoSporco[i][campo->posizioneXSnake]=='_') {
                direzione='s';
            }
        }

        for (int i = campo->posizioneYSnake-1; i > 0; i--) {
            // guarado se sopra c'e' la fine
            if (campo->campoSporco[i][campo->posizioneXSnake]=='_') {
                direzione='w';
            }
        }
    }
    return direzione;
}

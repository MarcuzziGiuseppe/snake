#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <time.h>
#include <string.h>

#define linguaDefault "italiano"

void fermaStampa();

bool controlloDeiFile(char lingua[]);

void stampaAVideoIlTesto(char paragrafo[], char lingua[]);

int main(int argc, char const *argv[]) {

/*
    if (controlloDeiFile("italiano") == true) {
        printf("File esiste\n");
    }
*/
    stampaAVideoIlTesto("menu", "italiano");
    
    return 0;
}

void fermaStampa() {
    // getchar mi peremette di non dover premere invio dopo aver scritto ma ovviamnete prende solo il primo carattere
    stampaAVideoIlTesto("fermaStampa",linguaDefault);
    char fermaStampa = getchar();
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
        char nomeFile[100];
        strcpy(nomeFile, lingua);
        strcat(nomeFile, ".txt");
        FILE *fin = fopen(nomeFile, "r");

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
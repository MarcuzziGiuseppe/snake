/*
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
*/

/*
typedef struct __node{
    __ *parent;
    bool visited;
    char directions[4]; // W S A D
    struct __node *children[4];
    int x;
    int y;
} _node;
*/
void algoritrmo(posizione* campo, _node* node){
    // caso base
    if (campo->posizioneXFine==node->x && campo->posizioneYFine==node->y) {
        // ritorna il percorso
    } else {
        for (size_t i = 0; i < 4; i++) {
            int result = spostamentoIA(node->directions[i], campo, node);
            _node* child;
            child->parent = &node;
            switch (i) {
            case 0:
                // W

                break;
            case 1:
                // S
                break;
            case 2:
                // A
                break;
            case 03:
                // D
                break;
            default:
                break;
            }
            if (result==0) {
                // posso spostarmi
                // crea nodo
                child->visited=false;
                node->children[i] = child;
            } else {
                // ho il muro
                child->visited=true;
                node->children[i] = child;
            }
            
        }
    }
}

int spostamentoIA(char direction, posizione* campo, _node* node){
    bool spostamentoRiuscito = false;
    switch (direction) {
    case 'w':
        //alto
        if ((campo->campoSporco[node->y-1][node->x]!='#' || campo->numberOfDrill>=1) && node->y-1>=0) {
            if (campo->numberOfDrill>=1 && campo->campoSporco[node->y-1][node->x]=='#') {
                campo->numberOfDrill--;
            }
            spostamentoRiuscito=true;
        }
        break;
    case 's':
        //basso
        if ((campo->campoSporco[node->y+1][node->x]!='#' || campo->numberOfDrill>=1) && node->y+1<altezzaCampo) {
            if (campo->numberOfDrill>=1 && campo->campoSporco[node->y+1][node->x]=='#') {
                campo->numberOfDrill--;
            }
            spostamentoRiuscito=true;
        }
        break;
    case 'a':
        //sinistra
        if (((campo->campoSporco[node->y][node->x-1]!='#' && node->x-1>0) || campo->numberOfDrill>=1) && node->x-1>=0) {
            if (campo->numberOfDrill>=1 && campo->campoSporco[node->y][node->x-1]=='#') {
                campo->numberOfDrill--;
            }
            spostamentoRiuscito=true;
        }
        break;
    case 'd':
        //destra
        if ((campo->campoSporco[node->y][node->x+1]!='#' || campo->numberOfDrill>=1) && node->x+1<larghezzaCampo) {
            if (campo->numberOfDrill>=1 && campo->campoSporco[node->y][node->x+1]=='#') {
                campo->numberOfDrill--;
            }
            spostamentoRiuscito=true;
        }
        break;
    default:
        break;
    }
    if (spostamentoRiuscito==true) {
        return 0;
    }
    
    return -1; // al momento mi serve per capire se è andato contro un muro o se cerca di uscire dalla mappa
}
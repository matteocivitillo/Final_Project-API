    /*
    GESTIONE MAGAZZINO: 
        combinazione di hashmap e heap:
        creo hashmap che ha come chiave il nome dell'ingrediente e come valore un puntatore al min-heap che gesisce le quantità (vedi foglio).
            Inserimento: L'elemento viene aggiunto alla fine dell'heap e viene applicato l'heapify up per mantenere la proprietà del min-heap.
            Estrazione: L'elemento con la data di scadenza minima viene rimosso e viene applicato l'heapify down per mantenere la proprietà del min-heap.
        --> se la dimensione dell'array viene sforata bisogna fare una realloc e ricollocare tutto l'array con complessità O(N)
        --> non so come gestire le collisioni nell'hashmap

        Dimensione della tabella: Scegli un numero primo maggiore del numero di elementi stimati (es. 10.007).
        Funzione di hash: DJB2 o MurmurHash per bilanciare efficienza e bassa collisione.
        Gestione delle collisioni: Usa liste concatenate o probing lineare/quadratico per gestire le collisioni.
    */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_NAME_LEN 256
#define INITIAL_CAPACITY

//struct magazzino

typedef struct nodo_heap {
    int scadenza; 
    int q;
} Nodo_heap;

typedef struct minHeap{
    Nodo_heap *array;
    int size;
    int cap;
} MinHeap;



int istante=0;

int main (){
    FILE *fp= fopen("istruzioni.txt", "r");
    if(fp==NULL){
        printf("errore nell'apertura del file");
        return 1;
    }
    char stringa[100];
    //ciclo while che finche il file contiene stringhe continua a leggere
    while (fscanf(fp, "%s", &stringa)!= EOF){

        if(strcmp(stringa, "aggiungi_ricetta")==0){
            printf("aggiunta ricetta\n");
        }

        else if(strcmp(stringa, "rimuovi_ricetta")==0){
            printf("rimossa ricetta: %s\n");
        }

        else if(strcmp(stringa, "ordine")==0){
            printf("ordine\n");
        }

        else if(strcmp(stringa, "rifornimento")==0){
            printf("rifornimento\n");
            //completare questa parte 
        }

        istante++;
    }
    fclose(fp);
    return 0;
}
    /*
    GESTIONE MAGAZZINO: 
        combinazione di hashmap e heap:
        creo hashmap che ha come chiave il nome dell'ingrediente e come valore un puntatore al min-heap che gesisce le quantità (vedi foglio).
            Inserimento: L'elemento viene aggiunto alla fine dell'heap e viene applicato l'heapify up per mantenere la proprietà del min-heap.
            Estrazione: L'elemento con la data di scadenza minima viene rimosso e viene applicato l'heapify down per mantenere la proprietà del min-heap.
        --> se la dimensione dell'array viene sforata bisogna fare una realloc e ricollocare tutto l'array con complessità O(N)
        --> non so come gestire le collisioni nell'hashmap
    */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_NAME_LEN 256
int istante=0;

int main (){
    FILE *fp= fopen("ricette.txt", "r");
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
        }

        istante++;
    }
    fclose(fp);
    return 0;
}
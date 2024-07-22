/*
passaggi da seguire:
    1) ricezione della frequenza di ritiro ordini e della capacità del camion
    2) ricezione e aggiunta delle ricette
    3) ricezione e aggiunta scorte a magazzino
    4) ricezione e gestione degli ordini
    5) ritiro e consegna ordini
*/

/*
    GESTIONE MAGAZZINO: 
        combinazione di hashmap e heap:
        creo hashmap che ha come chiave il nome dell'ingrediente e come valore un puntatore al min-heap che gesisce le quantità (vedi foglio).
            Inserimento: L'elemento viene aggiunto alla fine dell'heap e viene applicato l'heapify up per mantenere la proprietà del min-heap.
            Estrazione: L'elemento con la data di scadenza minima viene rimosso e viene applicato l'heapify down per mantenere la proprietà del min-heap.
        --> se la dimensione dell'array viene sforata bisogna fare una realloc e ricollocare tutto l'array con complessità O(N)
        --> non so come gestire le collisioni nell'hashmap

    GESTIONE RICETTE: 
        nell'ultimo test ci sono 1558 ricette quindi un hashmap con 2000 elementi potrebbe andare bene.
        chiave dell'hashmap è il nome della ricetta e il valore è un altra mappa che associa ciascun ingrediente a un valore

    GESTIONE ORDINI: 
        coda FIFO, orine piu recente viene elaborato e se eseguito viene messo in ordini eseguiti e i materiali vengono rimossi dal magazzino, può essere utile calcolare e salvare il peso dell'ordine.
        se l'ordine non puo essere eseguito viene messo in un altra lista FIFO per gli ordini in attesa
        --> quando eseguo un ordine devo anche eliminare tutti gli ingredienti gia scaduti
        come verifico la fattibilità di un'orine senza prelevare gli elementi? cioe devo essere sicuro che l'ordine sia fattibile prima di prelevare elementi

    FREQUENZA E CAPACITA' RITIRO:
        sono le prima informazioni fornite, salvate in variabili globali perche tanto non verranno piu modificate

    RITIRO E CONSEGNA ORDINI: 
        scelgo ordini da consegnare in base all'ordine nella coda FIFO di ordini completati e li inserisco ordinati in base al peso in una lista ordini da spedire che poi stampo
*/




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void aggiungi_ricetta();
void rimuovi_ricetta();
void ordine();
void rifornimento();
int frequenza_camion=0; 
int capienza_camion=0;
int istante=0;

int main (){
    FILE *fp= fopen("istruzioni.txt", "r");
    if(fp==NULL){
        printf("errore nell'apertura del file");
        return 1;
    }
    fscanf(fp,"%d", &frequenza_camion);
    fscanf(fp,"%d", &capienza_camion);
    char stringa[100];
    //ciclo while che finche il file contiene stringhe continua a leggere
    while (fscanf(fp, "%s", &stringa)!= EOF){

        if(strcmp(stringa, "aggiungi_ricetta")==0){
            //prendi parametri e chiama aggiungi ricetta
            printf("aggiungi_ricetta\n");

        }

        else if(strcmp(stringa, "rimuovi_ricetta")==0){
            printf("rimuovi_ricetta\n");
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


    {
        //aggiorna tempo corrente--> forse non conviene scartare subito i prodotti scaduti ma conviene 
        //lasciarli dove sono e quando vengono incontrati in seguito si verifica se sono da scartare
        
        /* 
        chiama una delle seguenti funzioni e modifica le strutture dati:

        */
    }
    

}
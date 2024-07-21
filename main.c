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
    */




#include <stdlib.h>
#include <stdio.h>

int main (){
    //ciclo while che finche il file contiene stringhe continua a leggere
    while (0)
    {
        //aggiorna tempo corrente--> forse non conviene scartare subito i prodotti scaduti ma conviene 
        //lasciarli dove sono e quando vengono incontrati in seguito si verifica se sono da scartare
        
        /* 
        chiama una delle seguenti funzioni e modifica le strutture dati:

        */
    }
    

}
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
#define CAP_INIZIALE 128
#define HASH_TABLE_M_LEN 10007

//struct magazzino

typedef struct nodo_heap {
    int scadenza; 
    int q;
} Nodo_heap;

typedef struct minHeap{
    Nodo_heap* array;
    int size;
    int cap_heap;
} MinHeap;

typedef struct hashNode{
    char* chiave;
    MinHeap* valore;
    struct hashNode* next;
} HashNode;

typedef struct hashtable {
    HashNode** puntatore_array;
    int cap_table;
    int size;
} HashTable;

//prototipi funzioni
unsigned int hash (char*);
MinHeap* creaHeap (int);
void swap(Nodo_heap*, Nodo_heap*);
int padre(int);
int figlio_sx(int);
int figlio_dx(int);
void heapifyUp(MinHeap*, int);
void heapifyDown(MinHeap*, int);
void inserisci_in_heap(MinHeap*, int, int);
Nodo_heap minimo(MinHeap*);
HashTable* creaHashTable (int);
HashNode* creaNodoHash (char*, MinHeap*);
void inserisci_in_table(HashTable*, char*, MinHeap*);
MinHeap* getHeap (HashTable*, char*);

//debug
void stampaHashTable(HashTable*);
void stampaHeap(MinHeap*);
void rimuovi_scaduti(MinHeap*, int);
int processa_ordine(HashTable*, char*, int);


int istante=0;

int main (){
    FILE *fp= fopen("magazzino.txt", "r");
    if(fp==NULL){
        printf("errore nell'apertura del file");
        return 1;
    }
    HashTable* magazzino = creaHashTable(HASH_TABLE_M_LEN);
    
    char stringa[256];

    //ciclo while che finche il file contiene stringhe continua a leggere
    while (fscanf(fp, "%s", stringa)!= EOF){

        if(strcmp(stringa, "aggiungi_ricetta")==0){
            printf("aggiunta ricetta\n");
        }

        else if(strcmp(stringa, "rimuovi_ricetta")==0){
            printf("rimossa ricetta: %s\n");
        }

        else if(strcmp(stringa, "ordine")==0){
            
            char ingrediente[MAX_NAME_LEN];
            int quantita;
            fscanf(fp, "%s%d", ingrediente, &quantita);
            printf("ordine di %s, quantità %d\n", ingrediente, quantita);
            
            if(processa_ordine(magazzino, ingrediente, quantita)) {
                printf("Ordine di %d unità di %s evaso.\n", quantita, ingrediente);
            } else {
                printf("Ordine di %d unità di %s non evaso (quantità insufficiente).\n", quantita, ingrediente);
            }
            
            

        }

        else if(strcmp(stringa, "rifornimento")==0){
            char ingrediente [MAX_NAME_LEN];
            int quantita; 
            int scadenza;
            while (fscanf (fp, "%s%d%d", ingrediente, &quantita, &scadenza)==3)
            {
                MinHeap* heap =getHeap(magazzino, ingrediente);
                if(heap==NULL) {
                    heap=creaHeap(CAP_INIZIALE);
                    inserisci_in_table(magazzino, ingrediente, heap);
                }
                inserisci_in_heap(heap, scadenza, quantita);
                printf("Rifornito %s con %d unità, scadenza %d\n", ingrediente, quantita, scadenza);
            }
            
        }

        istante++;
    }
    stampaHashTable(magazzino);
    fclose(fp);
    //non so se servono
    for (int i = 0; i < magazzino->cap_table; i++) {
        HashNode *node = magazzino->puntatore_array[i];
        while (node) {
            HashNode *temp = node;
            node = node->next;
            free(temp->chiave);
            free(temp->valore->array);
            free(temp->valore);
            free(temp);
        }
    }
    free(magazzino->puntatore_array);
    free(magazzino);

    return 0;
}



//funzioni
unsigned int hash(char* nome) {
    unsigned long hash = 5381;
    int c;
    while ((c = *nome++)) {
        hash = hash * 33 + c;
    }
    return hash % HASH_TABLE_M_LEN;
}

MinHeap* creaHeap (int cap){
    MinHeap *minHeap= (MinHeap*) malloc(sizeof(MinHeap));
    minHeap->size=0;
    minHeap->cap_heap=cap;
    minHeap->array=(Nodo_heap*) malloc (cap*sizeof(Nodo_heap)); //grandezza= num nodi * gradezza nodo
    return minHeap;
}

void swap (Nodo_heap* a, Nodo_heap* b){
    Nodo_heap temp= *a; //assegna a temp il puntatore ad a
    *a=*b; //il puntatore ad a ora punta a b
    *b =temp; //il puntatore a b ora è temp quindi punta al vecchio a
}

int padre(int i){
    return (i-1)/2; 
}
int figlio_sx(int i){
    return (2*i +1);
}
int figlio_dx(int i){
    return (2*i +2);
}

void heapifyUp(MinHeap* minHeap, int indice){
    while (indice!=0 && minHeap->array[padre(indice)].scadenza > minHeap->array[indice].scadenza)
    {
        swap (&minHeap->array[indice], &minHeap->array[padre(indice)]);
        indice = padre(indice);
    }  
}


void heapifyDown (MinHeap* minHeap, int indice){
    int minore= indice;
    int sx= figlio_sx(indice);
    int dx= figlio_dx(indice);
    if(sx < minHeap->size && minHeap->array[sx].scadenza < minHeap->array[minore].scadenza){
        minore=sx;
    }
    if(dx < minHeap->size && minHeap->array[dx].scadenza < minHeap->array[minore].scadenza){
        minore=dx;  //non devo mettere un else?
    }
    if(minore != indice){
        swap(&minHeap->array[indice], &minHeap->array[minore]);
        heapifyDown(minHeap, minore);
    }

}

void inserisci_in_heap(MinHeap* minHeap, int scadenza , int q){
    if(minHeap->size==minHeap->cap_heap){
        minHeap->cap_heap*=2;
        minHeap->array=(Nodo_heap* ) realloc(minHeap->array, minHeap->cap_heap * sizeof(Nodo_heap));

    }
    int indice = minHeap->size++;
    minHeap->array[indice].scadenza=scadenza;
    minHeap->array[indice].q=q;
    heapifyUp(minHeap, indice);

}

Nodo_heap minimo(MinHeap* minHeap){
    if(minHeap->size==1){
        minHeap->size--;
        return minHeap->array[0];
    }
    Nodo_heap testa = minHeap->array[0];
    minHeap->array[0]= minHeap->array[(minHeap->size)-1];
    minHeap->size--;
    heapifyDown(minHeap, 0);
    return testa;

}

HashTable* creaHashTable (int cap){
    HashTable* hashtable =(HashTable*) malloc (sizeof(HashTable));
    hashtable->cap_table= cap;
    hashtable->size=0;
    hashtable->puntatore_array = (HashNode**) calloc(cap, sizeof(HashNode*));
    for (int i = 0; i < cap; i++)   
    {
        hashtable->puntatore_array[i]=NULL;
    }
    return hashtable;
}

HashNode* creaNodoHash (char* chiave, MinHeap* valore){
    HashNode* nuovo_nodo=(HashNode*) malloc (sizeof(HashNode));
    nuovo_nodo->chiave= strdup(chiave); //newNode->key = strdup(key);
    nuovo_nodo->valore=valore;
    nuovo_nodo->next=NULL;
    return nuovo_nodo;
}
//fare  nuovo_nodo->chiave= chiave; potrebbe dare problemi, nel caso fare newNode->key = strdup(key);
//e implementare strdup come : 

char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* new_s = (char*) malloc(len);
    if (new_s == NULL) return NULL;
    memcpy(new_s, s, len);
    return new_s;
}


void inserisci_in_table(HashTable* hashtable, char* chiave , MinHeap* valore){
    unsigned long val_hash =hash(chiave);
    HashNode* nuovo_nodo = creaNodoHash(chiave, valore);
    if(hashtable->puntatore_array[val_hash]==NULL){
        hashtable->puntatore_array[val_hash]=nuovo_nodo;
    } else {
        HashNode* temp= hashtable->puntatore_array[val_hash];
        while (temp->next!=NULL)   
        {
            temp=temp->next;
        }
        temp->next=nuovo_nodo;    
    }
    hashtable->size++;
}


MinHeap* getHeap (HashTable* hashtable, char* chiave){
    unsigned long val_hash = hash (chiave);
    HashNode* nodo = hashtable->puntatore_array[val_hash];
    while (nodo!= NULL)
    {
        if(strcmp(nodo->chiave, chiave)==0){
            return nodo->valore;
        }
        nodo=nodo->next;
    }
    return NULL;
}


//funzioni di debug
void stampaHeap(MinHeap* heap) {
    if (heap == NULL) {
        printf("Heap is NULL\n");
        return;
    }
    printf("Heap size: %d\n", heap->size);
    for (int i = 0; i < heap->size; i++) {
        printf("Scadenza: %d, Quantita: %d\n", heap->array[i].scadenza, heap->array[i].q);
    }
}

void stampaHashTable(HashTable* hashtable) {
    if (hashtable == NULL) {
        printf("HashTable is NULL\n");
        return;
    }
    for (int i = 0; i < hashtable->cap_table; i++) {
        HashNode* node = hashtable->puntatore_array[i];
        if (node == NULL) continue;
        printf("Indice: %d\n", i);
        while (node != NULL) {
            printf("Ingrediente: %s\n", node->chiave);
            stampaHeap(node->valore);
            node = node->next;
        }
    }
}

void rimuovi_scaduti(MinHeap* heap, int istante) {
    while(heap->size > 0 && heap->array[0].scadenza <= istante) {
        minimo(heap);
    }
}

int processa_ordine(HashTable* magazzino, char* ingrediente, int quantita) {
    MinHeap* heap = getHeap(magazzino, ingrediente);
    if(!heap) return 0;

    rimuovi_scaduti(heap, istante);
    
    int totale = 0;
    while(heap->size > 0 && totale < quantita) {
        Nodo_heap min = minimo(heap);
        if(totale + min.q <= quantita) {
            totale += min.q;
        } else {
            min.q -= (quantita - totale);
            totale = quantita;
            inserisci_in_heap(heap, min.scadenza, min.q);
        }
    }
    
    return totale >= quantita;
}
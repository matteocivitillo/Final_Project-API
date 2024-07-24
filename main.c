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

        dimensione iniziale ottimale heap=128 e quando sfora realloc
        mediamente sono presenti 8500 ingredienti diversi --> quindi hash table di 10007 elementi

    GESTIONE RICETTE: 
        nell'ultimo test ci sono 1558 ricette quindi un hashmap con 2081 elementi potrebbe andare bene.
        chiave dell'hashmap è il nome della ricetta e il valore è un altra mappa che associa ciascun ingrediente a un valore
        ogni ricetta ha un counter che indica il numero di ordini arrivati per quella ricetta (sia ordini completati sia ordini in attesa)
        il counter aumentato di 1 quando ricevo un ordine relativo a quella ricetta e va diminuito di 1 quando invio un ordine di quella ricetta
        quando provo a rimuovere una ricetta devo verificare che il counter di essa sia a zero
        una volta inseriti gli ingredienti calcolo anche il peso totale della ricetta

    GESTIONE ORDINI: 
        coda FIFO, orine piu recente viene elaborato e se eseguito viene messo in ordini eseguiti e i materiali vengono rimossi dal magazzino, può essere utile calcolare e salvare il peso dell'ordine.
        se l'ordine non puo essere eseguito viene messo in un altra lista FIFO per gli ordini in attesa
        --> quando eseguo un ordine devo anche eliminare tutti gli ingredienti gia scaduti
        come verifico la fattibilità di un'orine senza prelevare gli elementi? cioe devo essere sicuro che l'ordine sia fattibile prima di prelevare elementi

    FREQUENZA E CAPACITA' RITIRO:
        sono le prima informazioni fornite, salvate in variabili globali perche tanto non verranno piu modificate

    RITIRO E CONSEGNA ORDINI: 
        scelgo ordini da consegnare in base all'ordine nella coda FIFO di ordini completati e li inserisco ordinati in base al peso in una lista ordini da spedire che poi stampo
        ricordarsi di decrementare il counter di una ricetta quando si inserisce l'ordine nel camion
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//magazzino
#define HASH_TABLE_LEN 2081
#define MAX_NAME_LEN 256
//magazzino
#define MAX_NAME_LEN 256
#define CAP_INIZIALE 128
#define HASH_TABLE_M_LEN 10007

//struct ricettario
typedef struct ingrediente {
    char nome_ingrediente[MAX_NAME_LEN];
    int q;
    struct ingrediente* prossimo_ing;
} Ingrediente; 

typedef struct ricetta {
    char nome_ricetta[MAX_NAME_LEN];
    int counter; 
    int peso_tot;
    Ingrediente* ingredienti_ricetta;
    struct ricetta* prossima_ricetta;
} Ricetta;

typedef struct hashTable{
    Ricetta* table[HASH_TABLE_LEN];
} HashTable_r;

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

//Struct ordini
typedef struct ordine{
    char nome_ricetta[MAX_NAME_LEN];
    int q;
    int istante;
    struct ordine* next;
} Ordine;

typedef struct coda{
    Ordine* primo_ord;
    Ordine* ultimo_ord;
    int size;
} Coda;

//prototipo funzioni ricettario
unsigned int hash(char*);
HashTable_r* crea_hashTable();
int trova_ricetta(char*);
void inserisci_ricetta(char*, Ingrediente*);
void rimuovi_ricetta(char*);

//prototipo funzioni magazzino
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

//prototipo funzioni ordini
Coda* crea_coda();
void enqueue(Coda*, char*, int, int);
Ordine* dequeue(Coda*);
void rimuovi_scaduti(MinHeap* , int );
int processa_ordine(HashTable* , char* , int );
int verifica_ordine(HashTable* , Ricetta* , int);
void ordine(HashTable* , Coda* , Coda* , char* , int );



//variabili globali
int frequenza_camion=0; 
int capienza_camion=0;
int istante=0;
HashTable_r* ht_r;
Coda* ordini_in_attesa;
Coda* ordini_completati;


int main (){
    FILE *fp= fopen("istruzioni.txt", "r");
    if(fp==NULL){
        printf("errore nell'apertura del file");
        return 1;
    }
    fscanf(fp,"%d", &frequenza_camion);
    fscanf(fp,"%d", &capienza_camion);
    char stringa[MAX_NAME_LEN];

    ht_r=crea_hashTable();
    HashTable* magazzino = creaHashTable(HASH_TABLE_M_LEN);



    //ciclo while che finche il file contiene stringhe continua a leggere
    while (fscanf(fp, "%s", stringa)!= EOF){

        if(strcmp(stringa, "aggiungi_ricetta")==0){
            char nome_ricetta[MAX_NAME_LEN];
            fscanf(fp, "%s",&nome_ricetta);
            Ingrediente* primo=NULL;
            Ingrediente* ultimo= NULL;
            char nome_ingrediente[MAX_NAME_LEN];
            int q;

            while(fscanf(fp, "%s", nome_ingrediente)==1){
                fscanf(fp,"%d", &q);
                Ingrediente* nuovo_ingrediente= (Ingrediente*)malloc(sizeof(Ingrediente));
                strcpy(nuovo_ingrediente->nome_ingrediente, nome_ingrediente);
                nuovo_ingrediente->q=q;
                nuovo_ingrediente->prossimo_ing=NULL;
                if(primo==NULL){
                    primo=nuovo_ingrediente;
                } else {
                    ultimo->prossimo_ing=nuovo_ingrediente;
                }
                ultimo=nuovo_ingrediente;
                int prossimo_carattere= fgetc(fp);
                if (prossimo_carattere=='\n' || prossimo_carattere== EOF)
                {
                    break;
                } else {
                    ungetc(prossimo_carattere, fp);
                }
                
            }
            inserisci_ricetta(nome_ricetta, primo); 
        }

        else if(strcmp(stringa, "rimuovi_ricetta")==0){
            char nome_ricetta[MAX_NAME_LEN];
            fscanf(fp, "%s", &nome_ricetta);
            rimuovi_ricetta(nome_ricetta);
            printf("rimossa ricetta: %s\n", nome_ricetta);
        }

        else if(strcmp(stringa, "ordine")==0){
            char nome_ricetta[MAX_NAME_LEN];
            int q;
            fscanf(fp, "&s%d", nome_ricetta, &q);
            ordine(magazzino, ordini_completati, ordini_in_attesa, nome_ricetta, q );
            
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
            processa_ordini_in_attesa(magazzino, ordini_completati, ordini_in_attesa);
        }

        istante++;
    }
    fclose(fp);

    //deallocazione memoria
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


//------------------------------------------------------------------------------------------------------
//implementazione funzioni ordini

Coda* crea_coda(){
    Coda* coda = (Coda*)malloc (sizeof(Coda));
    coda->primo_ord=coda->ultimo_ord=NULL;
    coda->size=0;
    return coda;
}

void enqueue(Coda* coda, char* nome_ricetta, int q, int istante){
    Ordine* nuovo_ordine= (Ordine*) malloc(sizeof(Ordine));
    strcpy(nuovo_ordine->nome_ricetta, nome_ricetta);
    nuovo_ordine->q=q;
    nuovo_ordine->istante=istante;
    nuovo_ordine->next=NULL;
    if(coda->ultimo_ord==NULL){
        coda->primo_ord=coda->ultimo_ord=nuovo_ordine;
    } else{
        coda->ultimo_ord->next=nuovo_ordine;
        coda->ultimo_ord=nuovo_ordine;
    }
    coda->size++;

}
Ordine* dequeue(Coda* coda){
    if(coda->primo_ord==NULL){
        return NULL;
    }
    Ordine* temp=coda->primo_ord;
    coda->primo_ord= coda->primo_ord->next;
    if(coda->primo_ord==NULL){
        coda->ultimo_ord=NULL;
    }
    coda->size--;
    return temp;

}
void rimuovi_scaduti(MinHeap* heap, int istante) {
    while (heap->size > 0 && heap->array[0].scadenza <= istante) {
        minimo(heap);
    }
}

int processa_ordine(HashTable* magazzino, char* ingrediente, int quantita) {
    MinHeap* heap = getHeap(magazzino, ingrediente);
    if (!heap) {
        return 0;
    }
    rimuovi_scaduti(heap, istante);

    int totale = 0;
    while (heap->size > 0 && totale < quantita) {
        Nodo_heap min = minimo(heap);
        if (totale + min.q <= quantita) {
            totale += min.q;
        } else {
            min.q -= (quantita - totale);
            totale = quantita;
            inserisci_in_heap(heap, min.scadenza, min.q);
        }
    }

    return totale >= quantita;
}

int verifica_ordine(HashTable* magazzino, Ricetta* ricetta, int quantita) {
    Ingrediente* ing = ricetta->ingredienti_ricetta;
    while (ing != NULL) {
        MinHeap* heap = getHeap(magazzino, ing->nome_ingrediente);
        if (!heap) return 0;

        rimuovi_scaduti(heap, istante);

        int totale = 0;
        int temp_size = heap->size;
        Nodo_heap* temp_array = (Nodo_heap*)malloc(temp_size * sizeof(Nodo_heap));
        memcpy(temp_array, heap->array, temp_size * sizeof(Nodo_heap));

        while (temp_size > 0 && totale < ing->q * quantita) {
            Nodo_heap min = temp_array[0];
            if (totale + min.q <= ing->q * quantita) {
                totale += min.q;
                temp_size--;
                memmove(temp_array, temp_array + 1, temp_size * sizeof(Nodo_heap));
            } else {
                totale += ing->q * quantita;
                break;
            }
        }

        free(temp_array);
        if (totale < ing->q * quantita) return 0;
        ing = ing->prossimo_ing;
    }
    return 1;
}

void ordine(HashTable* magazzino, Coda* ordini_completati, Coda* ordini_in_attesa, char* nome_ricetta, int quantita) {
    if (!trova_ricetta(nome_ricetta)) {
        printf("Ricetta non trovata: %s\n", nome_ricetta);
        return;
    }

    Ricetta* ricetta = ht_r->table[hash(nome_ricetta)];
    while (ricetta) {
        if (strcmp(ricetta->nome_ricetta, nome_ricetta) == 0) break;
        ricetta = ricetta->prossima_ricetta;
    }

    if (verifica_ordine(magazzino, ricetta, quantita)) {
        Ingrediente* ing = ricetta->ingredienti_ricetta;
        while (ing != NULL) {
            processa_ordine(magazzino, ing->nome_ingrediente, ing->q * quantita);
            ing = ing->prossimo_ing;
        }
        Ordine* nuovo_ordine = (Ordine*)malloc(sizeof(Ordine));
        strcpy(nuovo_ordine->nome_ricetta, nome_ricetta);
        nuovo_ordine->q = quantita;
        nuovo_ordine->next = NULL;

        if (ordini_completati->ultimo_ord == NULL) {
            ordini_completati->primo_ord = ordini_completati->ultimo_ord = nuovo_ordine;
        } else {
            ordini_completati->ultimo_ord->next = nuovo_ordine;
            ordini_completati->ultimo_ord = nuovo_ordine;
        }
        ricetta->counter++;
        printf("Ordine completato: %s x%d\n", nome_ricetta, quantita);
    } else {
        enqueue(ordini_in_attesa, nome_ricetta, quantita, istante);
        printf("Ordine in attesa: %s x%d\n", nome_ricetta, quantita);
    }
}

void processa_ordini_in_attesa(HashTable* magazzino, Coda* ordini_completati, Coda* ordini_in_attesa) {
    Ordine* curr = ordini_in_attesa->primo_ord;
    Ordine* prec = NULL;
    while (curr != NULL) {
        if (trova_ricetta(curr->nome_ricetta)) {
            Ricetta* ricetta = ht_r->table[hash(curr->nome_ricetta)];
            while (ricetta) {
                if (strcmp(ricetta->nome_ricetta, curr->nome_ricetta) == 0) break;
                ricetta = ricetta->prossima_ricetta;
            }
            if (verifica_ordine(magazzino, ricetta, curr->q)) {
                Ingrediente* ing = ricetta->ingredienti_ricetta;
                while (ing != NULL) {
                    processa_ordine(magazzino, ing->nome_ingrediente, ing->q * curr->q);
                    ing = ing->prossimo_ing;
                }
                if (prec == NULL) {
                    ordini_in_attesa->primo_ord = curr->next;
                } else {
                    prec->next = curr->next;
                }
                if (curr == ordini_in_attesa->ultimo_ord) {
                    ordini_in_attesa->ultimo_ord = prec;
                }
                curr->next = NULL;
                if (ordini_completati->ultimo_ord == NULL) {
                    ordini_completati->primo_ord = ordini_completati->ultimo_ord = curr;
                } else {
                    ordini_completati->ultimo_ord->next = curr;
                    ordini_completati->ultimo_ord = curr;
                }
                curr = (prec == NULL) ? ordini_in_attesa->primo_ord : prec->next;
                printf("Ordine completato dall'attesa: %s x%d\n", curr->nome_ricetta, curr->q);
            } else {
                prec = curr;
                curr = curr->next;
            }
        } else {
            printf("Ricetta non trovata per l'ordine in attesa: %s\n", curr->nome_ricetta);
            prec = curr;
            curr = curr->next;
        }
    }
}


//------------------------------------------------------------------------------------------------------
//implementazioni funzioni magazzino

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

//------------------------------------------------------------------------------------------------------
//implementazione funzioni ricettario

unsigned int hash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = hash*33 + c; 
    }

    return hash % HASH_TABLE_LEN;
}


HashTable_r* crea_hashTable(){
    HashTable_r* ht= (HashTable_r*)malloc(sizeof(HashTable_r));
    for (int i=0; i< HASH_TABLE_LEN; i++){
        ht->table[i]= NULL;
    }
    return ht;
}

void inserisci_ricetta(char* nome, Ingrediente* ingredienti){
    unsigned int val_hash= hash(nome);
    //verifica che la ricetta non sia presente prima di inserirla
    if(trova_ricetta(nome)){
        printf("ricetta gia presente, comando ignorato\n");
        return;
    }    
    Ricetta* nuova_ricetta= (Ricetta*) malloc(sizeof(Ricetta));
    strcpy(nuova_ricetta->nome_ricetta, nome); //copio il nome
    Ingrediente* current = ingredienti; 
    int counter=0;
    while (current!=NULL)
    {
        counter+= current->q;
        current=current->prossimo_ing;
    }
    nuova_ricetta->counter=counter;
    nuova_ricetta->ingredienti_ricetta=ingredienti; //passo il puntatore
    //inserimento in testa
    nuova_ricetta->prossima_ricetta=ht_r->table[val_hash];
    ht_r->table[val_hash]= nuova_ricetta;
    printf("aggiunta ricetta: %s, con peso %d\n", nuova_ricetta->nome_ricetta, nuova_ricetta->counter );
}


int trova_ricetta(char* nome) {
    unsigned int val_hash = hash(nome);
    Ricetta* ricetta = ht_r->table[val_hash];
    while (ricetta != NULL) {
        if(strcmp(ricetta->nome_ricetta, nome) == 0 ){
            return 1; 
        } else {
            ricetta = ricetta->prossima_ricetta;
        }   
    }
    return 0;    
}

void rimuovi_ricetta(char* nome){
    unsigned int val_hash=hash (nome);
    Ricetta* current = ht_r->table[val_hash];
    Ricetta* prec=NULL;
    while (current != NULL&& strcmp(current->nome_ricetta, nome)!=0)
    {
        prec = current; 
        current= current->prossima_ricetta;
    }
    if(current==NULL){
        //non ho trovato la ricetta da eliminare
        printf("ricetta non trovata\n");
        return;
    }
    if(current->counter!=0){
        printf("ordini in sospeso\n");
        return;
    }
    else if(prec == NULL){
        //sono in testa alla lista
        ht_r->table[val_hash]=current->prossima_ricetta;
        free(current);
        return;
    } else {
        prec->prossima_ricetta= current->prossima_ricetta;
        free(current);
        return;
    }    
}
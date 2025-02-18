#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define HASH_TABLE_LEN 2081
#define MAX_NAME_LEN 256

//struct
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


//prototipo funzioni hash_table
unsigned int hash(char*);
HashTable_r* crea_hashTable();
int trova_ricetta(char*);
void inserisci_ricetta(char*, Ingrediente*);
void rimuovi_ricetta(char*);
void stampa_hashTable();


//variabili globali
HashTable_r* ht_r;
int frequenza_camion=0; 
int capienza_camion=0;
int istante=0;



int main (){
    FILE *fp= fopen("ricette.txt", "r");
    if(fp==NULL){
        printf("errore nell'apertura del file");
        return 1;
    }
    fscanf(fp,"%d", &frequenza_camion);
    fscanf(fp,"%d", &capienza_camion);
    char stringa[100];
    ht_r= crea_hashTable();
    //ciclo while che finche il file contiene stringhe continua a leggere
    while (fscanf(fp, "%s", &stringa)!= EOF){

        if(strcmp(stringa, "aggiungi_ricetta")==0){
            char nome_ricetta[MAX_NAME_LEN];
            fscanf(fp, "%s",&nome_ricetta);
            Ingrediente* primo=NULL;
            Ingrediente* ultimo= NULL;
            char nome_ingrediente[MAX_NAME_LEN];
            int q;

            while(fscanf(fp, "%s", &nome_ingrediente)==1){
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
            printf("ordine\n");
        }

        else if(strcmp(stringa, "rifornimento")==0){
            printf("rifornimento\n");
        }

        istante++;
    }
    stampa_hashTable();
    fclose(fp);
    return 0;
}



//implementazione funzioni 

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
    if(current->counter==0){
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
void stampa_hashTable(){
    for (int i = 0; i < HASH_TABLE_LEN; i++) {
        if (ht_r->table[i] != NULL) {
            Ricetta* ricetta = ht_r->table[i];
            while (ricetta != NULL) {
                printf("Index %d: %s\n", i, ricetta->nome_ricetta);
                Ingrediente* ingrediente = ricetta->ingredienti_ricetta;
                while (ingrediente != NULL) {
                    printf("  Ingrediente: %s, Quantità: %d\n", ingrediente->nome_ingrediente, ingrediente->q);
                    ingrediente = ingrediente->prossimo_ing;
                }
                ricetta = ricetta->prossima_ricetta;
            }
        }
    }
}
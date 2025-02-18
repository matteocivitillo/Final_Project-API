#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 50
#define HASH_SIZE 100

typedef struct Lotto {
    int scadenza;
    int quantita;
} Lotto;

typedef struct Heap {
    Lotto *lotti;
    int size;
    int capacity;
} Heap;

typedef struct HashNode {
    char nome[MAX_NAME_LEN];
    Heap *heap;
    struct HashNode *next;
} HashNode;

typedef struct HashMap {
    HashNode *table[HASH_SIZE];
} HashMap;

Heap* createHeap(int capacity) {
    Heap *heap = (Heap *)malloc(sizeof(Heap));
    heap->lotti = (Lotto *)malloc(capacity * sizeof(Lotto));
    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

void swap(Lotto *a, Lotto *b) {
    Lotto temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(Heap *heap, int index) {
    while (index > 0 && heap->lotti[index].scadenza < heap->lotti[(index - 1) / 2].scadenza) {
        swap(&heap->lotti[index], &heap->lotti[(index - 1) / 2]);
        index = (index - 1) / 2;
    }
}

void heapifyDown(Heap *heap, int index) {
    int minIndex = index;
    int leftChild = 2 * index + 1;
    int rightChild = 2 * index + 2;

    if (leftChild < heap->size && heap->lotti[leftChild].scadenza < heap->lotti[minIndex].scadenza) {
        minIndex = leftChild;
    }
    if (rightChild < heap->size && heap->lotti[rightChild].scadenza < heap->lotti[minIndex].scadenza) {
        minIndex = rightChild;
    }
    if (index != minIndex) {
        swap(&heap->lotti[index], &heap->lotti[minIndex]);
        heapifyDown(heap, minIndex);
    }
}

void insertHeap(Heap *heap, int scadenza, int quantita) {
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;
        heap->lotti = (Lotto *)realloc(heap->lotti, heap->capacity * sizeof(Lotto));
    }
    heap->lotti[heap->size].scadenza = scadenza;
    heap->lotti[heap->size].quantita = quantita;
    heapifyUp(heap, heap->size);
    heap->size++;
}

Lotto extractMinHeap(Heap *heap) {
    Lotto minLotto = heap->lotti[0];
    heap->lotti[0] = heap->lotti[heap->size - 1];
    heap->size--;
    heapifyDown(heap, 0);
    return minLotto;
}

int isEmptyHeap(Heap *heap) {
    return heap->size == 0;
}

unsigned int hash(const char *str) {
    unsigned int hash = 0;
    while (*str) {
        hash = (hash << 5) + *str++;
    }
    return hash % HASH_SIZE;
}

HashMap* createHashMap() {
    HashMap *hashMap = (HashMap *)malloc(sizeof(HashMap));
    for (int i = 0; i < HASH_SIZE; i++) {
        hashMap->table[i] = NULL;
    }
    return hashMap;
}

HashNode* createHashNode(const char *nome, Heap *heap) {
    HashNode *node = (HashNode *)malloc(sizeof(HashNode));
    strcpy(node->nome, nome);
    node->heap = heap;
    node->next = NULL;
    return node;
}

void insertHashMap(HashMap *hashMap, const char *nome, int scadenza, int quantita) {
    unsigned int index = hash(nome);
    HashNode *node = hashMap->table[index];
    while (node != NULL) {
        if (strcmp(node->nome, nome) == 0) {
            insertHeap(node->heap, scadenza, quantita);
            return;
        }
        node = node->next;
    }
    Heap *heap = createHeap(10);
    insertHeap(heap, scadenza, quantita);
    node = createHashNode(nome, heap);
    node->next = hashMap->table[index];
    hashMap->table[index] = node;
}

Heap* getHeap(HashMap *hashMap, const char *nome) {
    unsigned int index = hash(nome);
    HashNode *node = hashMap->table[index];
    while (node != NULL) {
        if (strcmp(node->nome, nome) == 0) {
            return node->heap;
        }
        node = node->next;
    }
    return NULL;
}

void printHeap(Heap *heap, int index, int depth) {
    if (index >= heap->size) {
        return;
    }
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("(%d, %d)", heap->lotti[index].scadenza, heap->lotti[index].quantita);
    printHeap(heap, 2 * index + 1, depth + 1);
    printHeap(heap, 2 * index + 2, depth + 1);
    printf("\n");
}

int main() {
    HashMap *magazzino = createHashMap();

    // Aggiungi lotti
    insertHashMap(magazzino, "zucchero", 150, 200);
    insertHashMap(magazzino, "farina", 220, 1000);
    insertHashMap(magazzino, "zucchero", 100, 50);  // Lotto con scadenza più ravvicinata

    // Aggiungi nuovi lotti
    insertHashMap(magazzino, "zucchero", 20, 50);   // Nuovo lotto con scadenza 20
    insertHashMap(magazzino, "zucchero", 170, 100); // Nuovo lotto con scadenza 170
    insertHashMap(magazzino, "zucchero", 120, 50);
    insertHashMap(magazzino, "zucchero", 200, 100);

    // Usa ingredienti
    Heap *heapZucchero = getHeap(magazzino, "zucchero");
    if (heapZucchero != NULL) {
        Lotto lotto = extractMinHeap(heapZucchero);
        printf("Utilizzati %d grammi di zucchero con scadenza %d\n", lotto.quantita, lotto.scadenza);
    }

    // Stampa lo heap come un albero
    printf("\nHeap zucchero:\n");
    heapZucchero = getHeap(magazzino, "zucchero");
    if (heapZucchero != NULL) {
        printHeap(heapZucchero, 0, 0);
    }

    // Rimuovi scaduti
    int tempo_corrente = 160;
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode *node = magazzino->table[i];
        while (node != NULL) {
            Heap *heap = node->heap;
            while (!isEmptyHeap(heap) && heap->lotti[0].scadenza <= tempo_corrente) {
                extractMinHeap(heap);
            }
            node = node->next;
        }
    }

    return 0;
}

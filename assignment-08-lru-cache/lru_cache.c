#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MAP_SIZE 10000
#define MAX_VALUE_LEN 100

typedef struct Node {
    int key;
    char value[MAX_VALUE_LEN];
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct {
    int capacity;
    int size;
    Node *front;
    Node *rear;
    Node *map[MAX_MAP_SIZE]; // Hash map: index = key → value = Node*
} LRUCache;

LRUCache cache;

Node *createNode(int key, const char *value) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->key = key;
    strcpy(node->value, value);
    node->prev = NULL;
    node->next = NULL;
    return node;
}

void moveToFront(Node *node) {
    if (cache.front == node) return;

    if (node->prev) node->prev->next = node->next;
    if (node->next) node->next->prev = node->prev;
    if (cache.rear == node) cache.rear = node->prev;

    node->prev = NULL;
    node->next = cache.front;

    if (cache.front) cache.front->prev = node;
    cache.front = node;
}

void detachRear() {
    if (cache.rear == NULL) return;

    Node *temp = cache.rear;
    cache.map[temp->key] = NULL;

    if (cache.front == cache.rear) {
        cache.front = cache.rear = NULL;
    } else {
        cache.rear = cache.rear->prev;
        cache.rear->next = NULL;
    }
    free(temp);
    cache.size--;
}

void createCache(int capacity) {
    cache.capacity = capacity;
    cache.size = 0;
    cache.front = NULL;
    cache.rear = NULL;

    for (int i = 0; i < MAX_MAP_SIZE; i++)
        cache.map[i] = NULL;
}

void put(int key, const char *value) {
    if (key < 0 || key >= MAX_MAP_SIZE) return;

    Node *node = cache.map[key];

    if (node != NULL) {
        strcpy(node->value, value);
        moveToFront(node);
        return;
    }

    Node *newNode = createNode(key, value);
    newNode->next = cache.front;

    if (cache.front) cache.front->prev = newNode;
    cache.front = newNode;

    if (cache.rear == NULL) cache.rear = newNode;

    cache.map[key] = newNode;
    cache.size++;

    if (cache.size > cache.capacity) {
        detachRear();
    }
}

void get(int key) {
    if (key < 0 || key >= MAX_MAP_SIZE) {
        printf("NULL\n");
        return;
    }

    Node *node = cache.map[key];

    if (node == NULL) {
        printf("NULL\n");
        return;
    }

    printf("%s\n", node->value);
    moveToFront(node);
}

int main() {
    char command[20];
    int key, capacity;
    char value[MAX_VALUE_LEN];

    while (1) {
        scanf("%s", command);

        if (strcmp(command, "createCache") == 0) {
            scanf("%d", &capacity);
            createCache(capacity);
        } 
        else if (strcmp(command, "put") == 0) {
            scanf("%d %s", &key, value);
            put(key, value);
        } 
        else if (strcmp(command, "get") == 0) {
            scanf("%d", &key);
            get(key);
        } 
        else if (strcmp(command, "exit") == 0) {
            break;
        }
    }

    return 0;
}
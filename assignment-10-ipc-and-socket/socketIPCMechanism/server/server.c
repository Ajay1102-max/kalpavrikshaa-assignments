#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

pthread_mutex_t lock;

void* handleClient(void* arg) {
    int clientSock = *(int*)arg;
    int choice, amount;
    FILE* fp;

    recv(clientSock, &choice, sizeof(int), 0);

    pthread_mutex_lock(&lock);
    fp = fopen("../resource/accountDB.txt", "r+");
    int balance;
    fscanf(fp, "%d", &balance);

    if(choice == 1) {
        recv(clientSock, &amount, sizeof(int), 0);
        if(amount <= balance) {
            balance -= amount;
            rewind(fp);
            fprintf(fp, "%d", balance);
            send(clientSock, &balance, sizeof(int), 0);
        }
    } else if(choice == 2) {
        recv(clientSock, &amount, sizeof(int), 0);
        balance += amount;
        rewind(fp);
        fprintf(fp, "%d", balance);
        send(clientSock, &balance, sizeof(int), 0);
    } else if(choice == 3) {
        send(clientSock, &balance, sizeof(int), 0);
    }

    fclose(fp);
    pthread_mutex_unlock(&lock);
    close(clientSock);
    return NULL;
}

int main() {
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr = {AF_INET, htons(8080), INADDR_ANY};

    bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSock, 5);

    pthread_mutex_init(&lock, NULL);

    while(1) {
        int clientSock = accept(serverSock, NULL, NULL);
        pthread_t tid;
        pthread_create(&tid, NULL, handleClient, &clientSock);
    }
}
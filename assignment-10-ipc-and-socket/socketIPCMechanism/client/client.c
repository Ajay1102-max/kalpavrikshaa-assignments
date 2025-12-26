#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr = {AF_INET, htons(8080), inet_addr("127.0.0.1")};
    connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    int choice, amount;
    printf("1 Withdraw\n2 Deposit\n3 Balance\n");
    scanf("%d", &choice);
    send(sock, &choice, sizeof(int), 0);

    if(choice == 1 || choice == 2) {
        scanf("%d", &amount);
        send(sock, &amount, sizeof(int), 0);
    }

    int balance;
    recv(sock, &balance, sizeof(int), 0);
    printf("Balance: %d\n", balance);
    close(sock);
    return 0;
}
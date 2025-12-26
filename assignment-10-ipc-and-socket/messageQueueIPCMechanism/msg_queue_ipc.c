#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct msg {
    long type;
    int arr[5];
};

void sortArray(int arr[], int n) {
    for(int i = 0; i < n-1; i++)
        for(int j = i+1; j < n; j++)
            if(arr[i] > arr[j]) {
                int t = arr[i];
                arr[i] = arr[j];
                arr[j] = t;
            }
}

int main() {
    key_t key = ftok("prog", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);

    struct msg message;
    message.type = 1;
    int arr[5] = {6, 2, 9, 1, 4};

    for(int i = 0; i < 5; i++)
        message.arr[i] = arr[i];

    msgsnd(msgid, &message, sizeof(message.arr), 0);
    msgrcv(msgid, &message, sizeof(message.arr), 1, 0);

    sortArray(message.arr, 5);

    printf("Sorted array: ");
    for(int i = 0; i < 5; i++)
        printf("%d ", message.arr[i]);
    printf("\n");

    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}
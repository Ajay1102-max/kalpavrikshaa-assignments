#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

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
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(int) * 5, 0666 | IPC_CREAT);
    int *arr = (int*)shmat(shmid, NULL, 0);

    int data[5] = {10, 8, 6, 4, 2};
    for(int i = 0; i < 5; i++)
        arr[i] = data[i];

    sortArray(arr, 5);

    printf("Sorted array: ");
    for(int i = 0; i < 5; i++)
        printf("%d ", arr[i]);
    printf("\n");

    shmdt(arr);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
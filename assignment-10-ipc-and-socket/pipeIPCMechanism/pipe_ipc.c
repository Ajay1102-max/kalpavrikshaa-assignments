#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

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
    int fd[2];
    int arr[5] = {9, 7, 5, 3, 1};

    pipe(fd);

    if(fork() == 0) {
        close(fd[1]);
        read(fd[0], arr, sizeof(arr));
        sortArray(arr, 5);
        close(fd[0]);
        write(fd[1], arr, sizeof(arr));
    } else {
        close(fd[0]);
        printf("Before sorting: ");
        for(int i = 0; i < 5; i++) printf("%d ", arr[i]);
        printf("\n");

        write(fd[1], arr, sizeof(arr));
        wait(NULL);
        read(fd[1], arr, sizeof(arr));

        printf("After sorting: ");
        for(int i = 0; i < 5; i++) printf("%d ", arr[i]);
        printf("\n");
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void sortArray(int arr[], int n) {
    for(int i = 0; i < n - 1; i++) {
        for(int j = i + 1; j < n; j++) {
            if(arr[i] > arr[j]) {
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

int main() {
    int arr[5] = {5, 1, 4, 2, 3};
    FILE *fp;

    fp = fopen("data.txt", "w");
    for(int i = 0; i < 5; i++)
        fprintf(fp, "%d ", arr[i]);
    fclose(fp);

    printf("Before sorting: ");
    for(int i = 0; i < 5; i++)
        printf("%d ", arr[i]);
    printf("\n");

    if(fork() == 0) {
        fp = fopen("data.txt", "r");
        for(int i = 0; i < 5; i++)
            fscanf(fp, "%d", &arr[i]);
        fclose(fp);

        sortArray(arr, 5);

        fp = fopen("data.txt", "w");
        for(int i = 0; i < 5; i++)
            fprintf(fp, "%d ", arr[i]);
        fclose(fp);
        exit(0);
    } else {
        wait(NULL);
        fp = fopen("data.txt", "r");
        printf("After sorting: ");
        for(int i = 0; i < 5; i++) {
            fscanf(fp, "%d", &arr[i]);
            printf("%d ", arr[i]);
        }
        printf("\n");
        fclose(fp);
    }
    return 0;
}
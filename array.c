#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#ifdef PRINT
void printr(int *array, int start, int length){
    if(start < length){
        printf("[");
        for(int i=start; i<length; i++){
            printf("%d, ", array[i]);
        }
        printf("\b\b]\n");
    }else{
        printf("[]");
    }
}

void print(int *array, int length){
    printr(array, 0, length);
}
#endif

void shuffle(int *array, int length){
    for(int i=0; i<length; i++){
        int j = i + rand() % (length-i);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

void merge(int *src, int *aux, int lb, int mb, int ub){
    int i = lb, j = mb, k = lb;
    while(i < mb && j < ub)
        aux[k++] = src[i] < src[j] ? src[i++] : src[j++];
    while(i < mb)
        aux[k++] = src[i++];
    while(k > lb)
        src[--j] = aux[--k];
}

void mergesort_aux(int *src, int *aux, int lb, int ub){
    if(lb + 1 < ub){
        int mb = (ub - lb) / 2 + lb;
        mergesort_aux(src, aux, lb, mb);
        mergesort_aux(src, aux, mb, ub);
        merge(src, aux, lb, mb, ub);
    }
}

void mergesort(int *array, int length){
    int *aux = malloc(length * sizeof(*aux));
    mergesort_aux(array, aux, 0, length);
    free(aux);
}

int main(int argc, char *argv[]){
    int N, S;
    if(argc != 3 || sscanf(argv[1], "%d", &N) != 1 || sscanf(argv[2], "%d", &S) != 1){
        fprintf(stderr, "usage: %s N S\n", argv[0]);
        return EXIT_FAILURE;
    }
    srand(S);
    int *array = malloc(N * sizeof(*array));
    for(int i=0; i<N; i++){
        array[i] = i;
    }

    #ifdef PRINT
    print(array, N);
    if(S) shuffle(array, N);
    print(array, N);
    mergesort(array, N);
    print(array, N);
    #else
    if(S) shuffle(array, N);
    mergesort(array, N);
    #endif

    struct rusage start, end;

    getrusage(RUSAGE_SELF, &start);
    for(int i=0; i<N; i++){
        if(array[i] != i){
            fprintf(stderr, "error: final array unsorted\n");
            return EXIT_FAILURE;
        }
    }
    getrusage(RUSAGE_SELF, &end);
    time_t sdelta = end.ru_utime.tv_sec - start.ru_utime.tv_sec;
    suseconds_t udelta = end.ru_utime.tv_usec - start.ru_utime.tv_usec;
    int total = 1000000ll * (long long)sdelta + (long long)udelta;
    printf("%d\n", total);

    return 0;
}

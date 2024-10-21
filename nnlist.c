#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>


void shuffle(int *array, int length){
    for(int i=0; i<length; i++){
        int j = i + rand() % (length-i);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}

typedef struct node_* node;
struct node_ {
    bool flag;
    int x;
    node next, nnext;
};

node make_node(int x){
    node n = malloc(sizeof(*n));
    n->flag = false;
    n->x = x;
    n->next = NULL;
    n->nnext = NULL;
    return n;
}

void free_node(node n){
    free(n);
}

typedef struct list_* list;
struct list_{
    node head;
    node tail, ttail;
};

list make_list(){
    list l = malloc(sizeof(*l));
    l->head = NULL;
    l->tail = NULL;
    return l;
}

void free_list(list l){
    if(l->head){
        for(node i = l->head->next; i->next!=NULL;){
            node temp = i;
            i = i->next;
            free_node(temp);
        }
        free_node(l->tail);
    }
    free(l);
}

bool fprint_list(FILE* file, list l){
    int error = fprintf(file, "[");
    for(node i = l->head; error >= 0 && i!=NULL; i=i->next){
        error = fprintf(file, "%d, ", i->x);
    }
    fprintf(file, l->head ? "\b\b]\n" : "]\n");
    return error >= 0;
}

bool trivial_list(list l){
    return l->head == l->tail;
}

void push_list(list l, node n){
    if(l->tail){
        if(l->ttail){
            l->ttail->nnext = n;
            l->ttail = l->tail;
        }else{
            l->ttail = l->head;
        }
        l->tail = l->tail->next = n;
    }else{
        l->head = l->tail = n;
    }
}

node pop_list(list l){
    node n = l->head;
    if(l->head == l->ttail){
        l->ttail = NULL;
    }
    if(trivial_list(l)){
        l->head = l->tail = NULL;
    }else{
        l->head = l->head->next;
        n->next = NULL;
    }
    return n;
}

list min_list(list a, list b){
    return a->head && b->head ? (a->head->x < b->head->x ? a : b) :
           a->head ? a :
           b->head ? b :
           NULL;
}

list split_list(list l){
    list k = make_list();
    node i = l->head, j = l->head->next;
    while(j != NULL && j->next != NULL){
        i = i->next;
        j = j->next->next;
    }
    k->tail = l->tail;
    l->tail = i;
    k->head = i->next;
    l->tail->next = NULL;
    return k;
}

list merge_list(list a, list b){
    list c = make_list(), t;
    while((t = min_list(a, b)))
        push_list(c, pop_list(t));
    free_list(a); free_list(b);
    return c;
}

list sort_list(list l){
    if(!trivial_list(l)){
        list k = split_list(l);
        l = sort_list(l); k = sort_list(k);
        l = merge_list(l, k);
    }
    return l;
}

void flip(node n){
    n->flag = !n->flag;
}

int main(int argc, char *argv[]){
    int N, S;
    if(argc != 3 || sscanf(argv[1], "%d", &N) != 1 || sscanf(argv[2], "%d", &S) != 1){
        fprintf(stderr, "usage: %s N S\n", argv[0]);
        return EXIT_FAILURE;
    }
    list l = make_list();
    srand(S);
    int *array = malloc(N * sizeof(*array));
    for(int i=0; i<N; i++){
        array[i] = i;
    }
    if(S) shuffle(array, N);
    for(int i=0; i<N; i++)
        push_list(l, make_node(array[i]));

    #ifdef PRINT
    fprint_list(stdout, l);
    l = sort_list(l);
    fprint_list(stdout, l);
    #else
    l = sort_list(l);
    #endif

    struct rusage start, end;

    getrusage(RUSAGE_SELF, &start);
    node j = l->head;
    for(int i=0; j != NULL && i<N; i++, j = j->next){
        if(j->nnext){
            flip(j->nnext);
        }
        if(j->x != i){
            fprintf(stderr, "error: final array unsorted at %d\n", i);
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

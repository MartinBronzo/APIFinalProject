#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#definde DIM 1025

typedef struct tree_node_s{
    int n;
    char line [DIM];
    bool color;
    struct poli_s *root;
    struct poli_s *left;
    struct poli_s *right;
} tree_node_t;

typedef struct progression_list_s{
    int strt;
    int end;
    char *lines [DIM];
    struct progression_list_s *pre;
    struct progression_list_s *nxt;
    struct progression_list_s *p; /*padre*/
    struct progression_list_s *l;  /*ultimo*/
} progression_list_t;

int main(int argc, char* argv[])
{
    char op;
    int strt;
    int end;
    
    scanf ("%d,%c", &strt, &op);
    if (op == ',') {
        scanf ("%d,%c", &end, &op);
    }
    printf("%d,%c,%d", &strt, &op, &end)
    
    while (op != 'q') {
        if (op == 'd') {
            /*delete operations*/
        } else if (op == 'c') {
            /*change operations*/
        } else if (op == 'p') {
            /*print operations*/
        } else if (op == 'u') {
            /*undo operations*/
        } else if (op == 'r') {
            /*delete operations*/
        }
        
        scanf ("%d,%c", &strt, &op);
        if (op == ',') {
            scanf ("%d,%c", &end, &op);
        }
        printf("%d,%c,%d", &strt, &op, &end)
    }
    
    /*libero quello che devo liberare*/
     
    return 0;
}

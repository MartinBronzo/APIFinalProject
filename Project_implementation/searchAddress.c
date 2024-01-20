#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#define DIM 1025
#define MAX_COMMAND_LENGHT 100
#define CHANGE 1
#define DELETE 0

typedef struct line_node_s{
    char *line;
    struct line_node_s *prev;
    struct line_node_s *next;
} line_node_t;

typedef struct progression_list_s{
    int strt; /*starting line in editor*/
    int nline;  /*numbers of lines in *lines */
    int nnodes;
    bool op; /*true = change, false = delete*/
    line_node_t *h; /*when allocating I create space for DIM*nline char: each line has a distance from del following of DIM char from his first char, each line end with \0 */
    line_node_t *t;
    struct progression_list_s *prev;
    struct progression_list_s *next;
} progression_list_t;


line_node_t *print (line_node_t *h, int ind1, int crrntline, int nline, int lstline);
line_node_t *change(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, int *ind1,  int *ind2, progression_list_t **h_ru);
line_node_t *delete(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, int *ind1,  int *ind2, progression_list_t **h_ru);
line_node_t *redoundo(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, bool *flag, int nop, progression_list_t **h_ru);
line_node_t *redo(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, bool *flag, int nop, progression_list_t **h_ru);
line_node_t *undo(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, bool *flag, int nop, progression_list_t **h_ru);
line_node_t *insertRU(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, progression_list_t **ruNode); /*opposite process to delete*/
line_node_t *changeRU(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, progression_list_t **ruNode);
line_node_t *deleteRU(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, progression_list_t **ruNode);
line_node_t *freeList(line_node_t *h);
progression_list_t *freeAfterRU(progression_list_t *h);
progression_list_t *freeProgressionList(progression_list_t *h);

line_node_t *searchAddrs(line_node_t *h, int *currentline, int *lastline, int *ind1, line_node_t **ind1_addr,  int *ind2, line_node_t **ind2_addr);
line_node_t *searchAddrsDef(line_node_t *h, line_node_t *t, line_node_t *p, int *currentline, int *lastline, int *ind1, line_node_t **ind1_addr,  int *ind2, line_node_t **ind2_addr);

int main(int argc, char* argv[])
{
	char commd[MAX_COMMAND_LENGHT];
	char accptd_commds[]="pcdurq";
	char *token;
    char op;
    char prevop; /*usefull for sequential ru*/
    int ind1; /*start line*/
    int ind2;
	int nop;
    int ruop;
    bool flagRU;
    int currentnop; /*after redo/undo op before change or delete*/
    int lastline; /*track number of the last line in the editor*/
    int currentline; /*track the last line accessed*/
    line_node_t *h, *t;
    line_node_t *p;
    progression_list_t *h_ru_list;
    
    ind1=0;
    ind2=0;
    prevop='0';
	nop=0; /*noperations useful for redo e undo*/
    ruop=0; /*redo,undo operations useful to see if there are more redo/undo shifts than possible*/
    currentnop=0;
    lastline=0;
    currentline=0;
    
    h=t=NULL;
    p=NULL;
    h_ru_list=NULL;
    
	fgets(commd, MAX_COMMAND_LENGHT, stdin);
	op = *strpbrk(commd, accptd_commds);
    /*delete is or isn't a useless starting operation?*/
	if (op=='c' || op=='p' /*|| op=='d'*/) {  /*these are the only possible operations at the beginning*/
		token = strtok(commd,",");
		ind1=atoi(token);
		token = strtok(NULL,"cpd");
		ind2=atoi(token);
	}
    
    while (op != 'q') {
        if ((op!='u' && op!='r') && (prevop=='u' || prevop=='r')) {
            /*terminating redo/undo operation, free part of the progression list except if op = print*/
            
            
            if (op != 'p') {
                if (nop==ruop) {
                    h_ru_list=freeProgressionList(h_ru_list);
                    h=freeList(h);
                    p=NULL;
                    t=NULL;
                    currentline=0;
                    lastline=0;
                } else {
                    p=redoundo(&h, &t, p, &currentline, &lastline, &flagRU, ruop-currentnop, &h_ru_list);
                    h_ru_list=freeAfterRU(h_ru_list);
                }
                nop-=ruop;
                ruop=0;
                currentnop=0;
                flagRU=0;
            } else
                p=redoundo(&h, &t, p, &currentline, &lastline, &flagRU, ruop-currentnop, &h_ru_list);
                currentnop=ruop;
            /*nop=ruop; we found the new end in the progression line*/
        }
        
        
        if (op == 'd') {
            /*delete operations*/
            p=delete(&h, &t, p, &currentline, &lastline, &ind1, &ind2, &h_ru_list);
            prevop=op;
            nop++;
        } else if (op == 'c') {
            /*change operations*/
            p=change(&h, &t, p, &currentline, &lastline, &ind1, &ind2, &h_ru_list);
            prevop=op;
            nop++;
            
        } else if (op == 'p') {
            /*print operations*/
            print(p, ind1, currentline, ind2-ind1+1, lastline);
            /*print don't count as a real operation*/
        } else if (op == 'r') {
            /*continuing redo operations*/
            ruop-=ind1;
            if (ruop<0) {
                ruop=0;
            }
            prevop=op;
            /*redo/undo moves in the progression list, they don't count as real operations*/
        } else if (op == 'u') {
            /*continuing undo operations*/
            ruop+=ind1;
            if (ruop>nop) {
                ruop=nop;
            }
            prevop=op;
            /*redo/undo moves in the progression list, they don't count as real operations*/

        }
        
        
        fgets(commd, MAX_COMMAND_LENGHT, stdin);
        op = *strpbrk(commd, accptd_commds);
        if (op=='c' || op=='p' || op=='d') {
            token = strtok(commd,",");
            ind1=atoi(token);
            token = strtok(NULL,"cpd");
            ind2=atoi(token);
        }else if (op=='r' || op=='u'){ /*useless for first command*/
            token = strtok(commd,"ru");
            ind1=atoi(token);
        }
        
    }
    /*free what there is to free*/
    h=freeList(h);
    h_ru_list=freeProgressionList(h_ru_list);
    
    return 0;
}


line_node_t *print (line_node_t *h, int ind1, int crrntline, int nline, int lstline){
    int i;
    line_node_t *p;
    p=h;
    
    if (ind1>lstline){
        for (i=0; i<nline; i++)
            printf(".\n");
        return p;
    }
        
    if(crrntline <= ind1){
        for (; crrntline < ind1; crrntline++)
            p=p->next;
    } else {
        for (; crrntline > ind1; crrntline--)
            p=p->prev;
    }
    
    
    i=0;
    while (i<nline && p) {
        fputs(p->line, stdout);
        i++;
        p=p->next;
    }
    for (; i<nline; i++) {
        printf(".\n");
    }
    
    return p;
}


line_node_t *searchAddrsDef(line_node_t *h, line_node_t *t, line_node_t *p, int *currentline, int *lastline, int *ind1, line_node_t **ind1_addr,  int *ind2, line_node_t **ind2_addr){
    int a;
    int b;
    int c;
    line_node_t *searcher1, *searcher2;
    int start1, start2;
    int linetracker;
    
    if(!h || !(*ind1)){ /*case: 0 lines in editoe*/
        p=NULL;
        *ind1_addr=NULL;
        *ind2_addr=NULL;
        *currentline=1;
    } else if(*ind1 > *lastline){
        *ind1_addr=t;
        *ind2_addr=NULL;
        p=NULL;
        *currentline=*lastline+1;
    } else {
        a=abs(*ind1 -1);
        b=abs(*ind1-*currentline);
        c=abs(*ind1-*lastline);
        
        if (a<=b && a<=c) {
            start1=1;
            searcher1=h;
        } else if (b<=c && b<=a){
            start1=*currentline;
            searcher1=p;
        } else{
            start1=*lastline;
            searcher1=t;
        }
        
        
        a=abs(*ind2 -1);
        b=abs(*ind2-*currentline);
        c=abs(*ind2-*lastline);
        
        if (a<=b && a<=c) {
            start2=1;
            searcher2=h;
        } else if (b<=c && b<=a){
            start2=*currentline;
            searcher2=p;
        } else{
            start2=*lastline;
            searcher2=t;
        }
        
        if (searcher1==searcher2) {
            p=searchAddrs(searcher1, &start1, lastline, ind1, ind1_addr, ind2, ind2_addr);
            *currentline=start1;
        } else {
            linetracker = *ind2+1;
            if(start2 <= linetracker){
                for (; start2 < linetracker  && searcher2; start2++)
                    searcher2=searcher2->next;
            } else {
                for (; start2 > linetracker; start2--)
                    searcher2=searcher2->prev;
            }
            *ind2_addr=searcher2;
            
            linetracker = *ind1;
            if(start1 <= linetracker){
                for (; start1 < linetracker  && searcher1; start1++)
                    searcher1=searcher1->next;
            } else {
                for (; start1 > linetracker; start1--)
                    searcher1=searcher1->prev;
            }
            p=searcher1;
            *ind1_addr=p->prev;
            *currentline=start1;
        }
    }
    
    
    return p;
}


line_node_t *searchAddrs(line_node_t *h, int *currentline, int *lastline, int *ind1, line_node_t **ind1_addr,  int *ind2, line_node_t **ind2_addr){
    
    line_node_t *p;
    int linetracker;
    int currentline_copy;
    
    if(!h || !(*ind1)){ /*case: 0 lines in editoe*/
        p=NULL;
        *ind1_addr=NULL;
        *ind2_addr=NULL;
        *currentline=1;
    } else if(*ind1 > *lastline){ /*case: reference to non existing lines*/
        for (p=h; p->next; p=p->next);
        *ind1_addr=p;
        *ind2_addr=NULL;
        p=NULL;
        *currentline=*lastline+1;
    } else if (*currentline < *ind1) { /*case: haddr<ind1<=ind2*/
        for (p=h, linetracker = *ind1 -1; *currentline < linetracker; (*currentline)++){
            p=p->next;
        }
        *ind1_addr=p;
        currentline_copy=*currentline;
        if (*ind2<*lastline) {
            for (; *currentline <= *ind2; (*currentline)++){
                p=p->next;
            }
            *ind2_addr=p;
        } else {
            *ind2_addr=NULL;
        }
        p=(*ind1_addr)->next;
        *currentline=currentline_copy+1;
    } else if (*currentline > *ind2) { /*case: haddr>ind2>ind1*/
        for (p=h, linetracker = *ind2 +1; *currentline > linetracker; (*currentline)--){
            p=p->prev;
        }
        *ind2_addr=p;
        
        for (; *currentline > *ind1; (*currentline)--){
            p=p->prev;
        }
        if (*ind1==1) {
            *ind1_addr=NULL;
        } else {
            *ind1_addr=p->prev;
        }
        /***************************************************************************/
        /*maybe there is a bug: what happens when ind1=ind2=currentline=lastline */
        /**************************************************************************/
    } else { /*case: ind1<=haddr<=ind2*/
        if (*ind2>*lastline) {
            *ind2_addr=NULL;
        } else {
            for (p=h, currentline_copy=*currentline; *currentline < *ind2; (*currentline)++){
                p=p->next;
            }
            *ind2_addr=p->next;
            *currentline=currentline_copy;
        }
        p=h;
        if (*ind1==1) {
            while (p->prev) {
                p=p->prev;
            }
            *ind1_addr=NULL;
            *currentline=1;
        } else {
            for (linetracker = *ind1; *currentline > linetracker; (*currentline)--){
                p=p->prev;
            }
            *ind1_addr=p->prev;
        }
        
    }
    
    return p;
}

line_node_t *change(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, int *ind1,  int *ind2, progression_list_t **h_ru){
    line_node_t *ind1_addr, *ind2_addr;
    line_node_t *new_node;
    progression_list_t *new_node_ru;
    int n;
    char finaldot[3];
    
    if( (new_node_ru=(progression_list_t*)malloc(sizeof(progression_list_t))) ){
        n = *ind2 - *ind1 + 1;
        new_node_ru->nline=n;
        new_node_ru->strt=*ind1;
        new_node_ru->prev=NULL;
        new_node_ru->next=*h_ru;
        new_node_ru->op=CHANGE;
        if (*h_ru) {
            (*h_ru)->prev=new_node_ru;
        }
        (*h_ru)=new_node_ru;
        p=searchAddrs(p, currentline, lastline, ind1, &ind1_addr, ind2, &ind2_addr);
        (*h_ru)->h=p;
        if (p) {
            p->prev=NULL;
        }
        if( (new_node=(line_node_t*)malloc(sizeof(line_node_t))) ){
            if( (new_node->line=(char*)malloc(DIM*sizeof(char))) ){
                if (ind1_addr) {
                    ind1_addr->next=new_node;
                } else {
                    *h=new_node;
                }
                p=new_node;
                p->prev=ind1_addr;
                fgets(p->line, DIM, stdin);
                p->line=(char*)realloc(p->line, (strlen(p->line)+1)*sizeof(char));
                if (!(p->line)) {
                    printf("change: error reallocating line in new node");
                    exit(1);
                }
                n--;
                while (n>0) {
                    if( (new_node=(line_node_t*)malloc(sizeof(line_node_t))) ){
                        if( (new_node->line=(char*)malloc(DIM*sizeof(char))) ){
                            p->next=new_node;
                            new_node->prev=p;
                            fgets(new_node->line, DIM, stdin);
                            p=new_node;
                            p->line=(char*)realloc(p->line, (strlen(p->line)+1)*sizeof(char));
                            if (!(p->line)) {
                                printf("change: error reallocating line in new node");
                                exit(1);
                            }
                            n--;
                            (*currentline)++;
                        } else {
                            printf("change: error allocating new line in new node");
                        }
                    } else {
                        printf("change: error allocating new node");
                        exit(1);
                    }
                }
                p->next=ind2_addr;
                if (ind2_addr) {
                    (*h_ru)->t = ind2_addr->prev;
                    ind2_addr->prev=p;
                    (*h_ru)->t->next=NULL;
                    new_node_ru->nnodes=new_node_ru->nline;
                } else {
                    new_node_ru->nnodes=*lastline-*ind1+1;
                    if (new_node_ru->nnodes<0) {
                        new_node_ru->nnodes=0;
                    }
                    (*h_ru)->t = *t;
                    if ((*h_ru)->t) {
                        (*h_ru)->t->next=NULL;
                    }
                    *t=p;
                    *lastline=*ind2;
                }
                fgets(finaldot, 3, stdin);
            } else{
                printf("change: error allocating new line in new node");
            }
        } else {
            printf("change: error allocating new node");
        }
    } else {
        printf("change: error allocating progression list");
    }
    
    return p;
}

line_node_t *delete(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, int *ind1,  int *ind2, progression_list_t **h_ru){
    line_node_t *ind1_addr, *ind2_addr;
    progression_list_t *new_node_ru;
    
    if( (new_node_ru=(progression_list_t*)malloc(sizeof(progression_list_t))) ){
        new_node_ru->strt=*ind1;
        new_node_ru->prev=NULL;
        new_node_ru->next=*h_ru;
        new_node_ru->op=DELETE;
        if (*h_ru) {
            (*h_ru)->prev=new_node_ru;
        }
        (*h_ru)=new_node_ru;
        p=searchAddrs(p, currentline, lastline, ind1, &ind1_addr, ind2, &ind2_addr);
        (*h_ru)->h=p;
        if (p) {
            p->prev=NULL;
            if (ind1_addr) {
                ind1_addr->next=ind2_addr;
            } else {
                *h=ind2_addr;
            }
            if (ind2_addr) {
                (*h_ru)->t = ind2_addr->prev;
                (*h_ru)->t->next=NULL;
                ind2_addr->prev=ind1_addr;
                new_node_ru->nline=*ind2 - *ind1 + 1;
                p=ind2_addr;
            } else {
                (*h_ru)->t=*t;
                if ((*h_ru)->t) {
                    (*h_ru)->t->next=NULL;
                }
                *t=ind1_addr;
                new_node_ru->nline=*lastline - *ind1 + 1;
                if (!ind1_addr) {
                    *currentline=0;
                    p=NULL;
                } else {
                    (*currentline)--;
                    p=ind1_addr;
                }
            }
            *lastline -= new_node_ru->nline;
        } else {
            new_node_ru->nline=0;
            (*h_ru)->t=NULL;
            p = *h;
            if (p) {
                *currentline=1;
            } else {
                *currentline=0;
            }
        }
        if (*lastline<0) {
            *lastline=0;
        }
    } else {
        printf("delete: error allocating progression list");
    }
    
    return p;
}

line_node_t *freeList(line_node_t *h){
    line_node_t *p;
    while (h) {
        p=h;
        h=h->next;
        if (p->line) {
            free(p->line);
        }
        free(p);
    }
    return h;
}

progression_list_t *freeAfterRU(progression_list_t *h){
    progression_list_t *p, *new_h;
    
    if (!h) {
        return NULL;
    } else {
        new_h=h->prev;
    }
    while (new_h) {
        p=new_h;
        new_h=new_h->prev;
        freeList(p->h);
        free(p);
    }
    return h;
}

progression_list_t *freeProgressionList(progression_list_t *h){
    progression_list_t *p;
    if (h) {
        freeAfterRU(h);
    }
    while (h) {
        p=h;
        h=h->next;
        if (p->h) {
            freeList(p->h);
        }
        free(p);
    }
    return h;
}

line_node_t *redoundo(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, bool *flag, int nop, progression_list_t **h_ru){
    
    if (nop>0) {
      //  printf("i'm starting undo\n");
        p=undo(h, t, p, currentline, lastline, flag, nop, h_ru);
    } else if (nop<0) {
     //   printf("i'm starting redo\n");
        p=redo(h, t, p, currentline, lastline, flag, -nop, h_ru);
    }
    
    return p;
}

line_node_t *undo(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, bool *flag, int nop, progression_list_t **h_ru){
    
    /*if head = NULL means nop=0 -> no problem*/
    /*if head = final node -> problem*/
    while (nop>0) {
        if ((*h_ru)->op==DELETE) {
        //    printf("i'm starting insert undo\n");
            p=insertRU(h, t, p, currentline, lastline, h_ru);
        } else {
        //    printf("i'm starting change undo\n");
            p=changeRU(h, t, p, currentline, lastline, h_ru);
        }
        if ( ((*h_ru)->next) ) {
            (*h_ru)=(*h_ru)->next;
        } else {
            *flag=1;
        }
        nop--;
    }
    return p;
}

line_node_t *redo(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, bool *flag, int nop, progression_list_t **h_ru){
    
    if ((*flag)) {
        *flag=0;
    } else {
        (*h_ru)=(*h_ru)->prev;
    }
    while (nop>0) {
        if ((*h_ru)->op==DELETE) {
          //  printf("i'm starting delete redo\n");
            p=deleteRU(h, t, p, currentline, lastline, h_ru);
        } else {
          //  printf("i'm starting change redo\n");
            p=changeRU(h, t, p, currentline, lastline, h_ru);
        }
        nop--;
        if (nop) {
            (*h_ru)=(*h_ru)->prev;
        }
    }
    return p;
}

line_node_t *insertRU(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, progression_list_t **ruNode){
    
    line_node_t *ind1_addr, *ind2_addr;
    int ind;
    
    if ((*ruNode)->h) {
        ind = (*ruNode)->strt;
        p=searchAddrs(p, currentline, lastline, &ind, &ind1_addr, &ind, &ind2_addr);
        if (ind1_addr) {
            ind1_addr->next = (*ruNode)->h;
        } else {
            (*h)=(*ruNode)->h;
        }
        (*ruNode)->h->prev = ind1_addr;
        if (p) {
            p->prev = (*ruNode)->t;
        } else {
            (*t)=(*ruNode)->t;
        }
        if ((*ruNode)->t) {
            (*ruNode)->t->next = p;
        }
        (*ruNode)->t=NULL;
        
        p=(*ruNode)->h;
        *currentline = (*ruNode)->strt;
        *lastline += (*ruNode)->nline;
        (*ruNode)->h = NULL;
    }
    
    return p;
}

line_node_t *deleteRU(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, progression_list_t **ruNode){
    line_node_t *ind1_addr, *ind2_addr;
    int ind1, ind2;
    
    ind1 = (*ruNode)->strt;
    ind2 = (*ruNode)->nline + ind1 - 1;
    ind1_addr = NULL;
    ind2_addr = NULL;
    p=searchAddrs(p, currentline, lastline, &ind1, &ind1_addr, &ind2, &ind2_addr);
    (*ruNode)->h=p;
    if (p) {
        p->prev=NULL;
        if (ind1_addr) {
            ind1_addr->next=ind2_addr;
        } else {
            *h=ind2_addr;
        }
        if (ind2_addr) {
            (*ruNode)->t = ind2_addr->prev;
            (*ruNode)->t->next=NULL;
            ind2_addr->prev=ind1_addr;
            p=ind2_addr;
        } else {
            (*ruNode)->t = *t;
            *t=ind1_addr;
            if (!ind1_addr) {
                *currentline=0;
                p=NULL;
            } else {
                (*currentline)--;
                p=ind1_addr;
            }
        }
        *lastline -= (*ruNode)->nline;
    } else {
        (*ruNode)->t=NULL;
        p = *h;
        if (p) {
            *currentline=1;
        } else {
                *currentline=0;
        }
    }
    if (*lastline<0) {
        *lastline=0;
    }
    
    return p;
}

line_node_t *changeRU(line_node_t **h, line_node_t **t, line_node_t *p, int *currentline, int *lastline, progression_list_t **ruNode){
    line_node_t *ind1_addr, *ind2_addr;
    int ind1, ind2;
    int c;
    
    ind1 = (*ruNode)->strt;
    ind2 = (*ruNode)->nline + ind1 - 1;
    ind1_addr = NULL;
    ind2_addr = NULL;
    p=searchAddrs(p, currentline, lastline, &ind1, &ind1_addr, &ind2, &ind2_addr);
    if (ind1_addr) {
        ind1_addr->next=(*ruNode)->h;
    } else {
        *h=(*ruNode)->h;
    }
    if ((*ruNode)->h)
        (*ruNode)->h->prev=ind1_addr;
    
    if ((*ruNode)->t) {
        (*ruNode)->t->next=ind2_addr;
    }
        
    (*ruNode)->h=p;
    if ((*ruNode)->h) { /*it refers to the new head in runode, it = NULL if ind1>lastline*/
        (*ruNode)->h->prev=NULL;
    }
        
    if (ind2_addr) {
        p=ind2_addr->prev;
        ind2_addr->prev=(*ruNode)->t;
        (*ruNode)->t=p;
        (*ruNode)->t->next=NULL;
        p=ind2_addr;
        *currentline= ind2 +1;
    } else {
        c=*lastline;
        *lastline=ind1+(*ruNode)->nnodes-1;
        (*ruNode)->nnodes=c-ind1+1;
        p=*t;
        *t=(*ruNode)->t;
        (*ruNode)->t = p;
        p=*t;
        *currentline=*lastline;
    }

    
    
    return p;
}

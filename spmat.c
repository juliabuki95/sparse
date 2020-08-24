
#include <stdio.h>
#include "spmat.h"
#include <stdlib.h>


typedef struct _Node {

    double data;
    int col;
    struct _Node *next;

} Node;

typedef struct _Arr {
    double *value;
    int *coli;
    int *rowptr;

}Arr;

Node* list_append(Node *last, double val, int skip)
{

    /* Create new node */
    Node *node = (Node *) malloc(sizeof(Node));
    node->data = val;
    node->col  = last->col + 1 + skip;
    node->next = NULL;

    /* Add as last element */
    last->next = node;
    return node;

}


void list_add_row(struct _spmat *A, const double *row, int i)
{
    /* Create row of non zero values */
    int j, n, skip = 0;
    Node *tail;

    n = A->n;
    tail = ((Node *) A->private) + i;

    for (j = 0; j < n; ++j) {
        if(row[j] != 0.) {
            tail = list_append(tail, row[j], skip);
            skip = 0;
        }
        else ++skip;
    }

}
void array_add_row(struct _spmat *A, const double *row, int i) {
    int j,count=0,place;
    Arr *arr;
    arr=A->private;
    if(i==0){
        arr->rowptr[0]=0;
    }
    place=arr->rowptr[i];
    for (j=0;j<A->n;j++){
        if(row[j]!=0.0){
            count++;
            arr->coli[place]=j;
            arr->value[place]=row[j];
            place++;
        }
    }
    arr->rowptr[i+1] = count + arr->rowptr[i];

}


double list_row_vector_product(Node *head, const double *v)
{
    double result = 0.0;

    /*skip -1 col*/
    head = head->next;
    while (head != NULL) {
        result += head->data * v[head->col];
        head = head->next;
    }
    return result;
}



void list_mult (const struct _spmat *A, const double *v, double *result)
{
    int i;
    Node *head =  (Node *) A->private;

    for(i = 0; i < A->n; ++i) {
        result[i] = list_row_vector_product(head, v);
        ++head;
    }

}

void	array_mult(const struct _spmat *A, const double *v, double *result){
    Arr *row;
    int i,j,count;
    double sum,a,b;
    count=0;
    sum=0.0;
    row=A->private;

    for(i=0;i<A->n;i++){
        for(j=0;j<row->rowptr[i+1]-row->rowptr[i];j++){
            a=v[row->coli[count]];
            b=row->value[count];
            sum+=a*b;
            count++;
        }
        result[i]=sum;
        sum=0.0;
    }
}


void list_free(struct _spmat *A)
{
    int i;
    Node *curr, *tmp, *head;

    head = (Node *) A->private;
    for (i = 0; i < A->n; ++i) {
        curr = head->next;
        while(curr != NULL) {
            tmp = curr->next;
            free(curr);
            curr = tmp;
        }
        ++head;
    }
    free(A->private);
}

void	array_free(struct _spmat *A){
    Arr *arr;
    arr=A->private;
    free(arr->coli);
    free(arr->value);

    free(arr->rowptr);
    free(A->private);

}

spmat* spmat_allocate_list(int n)
{
    int i;
    Node *rows;

    /* Assign Linked list implementation */
    spmat *matrix = (spmat *) malloc(sizeof(spmat));

    matrix->n        = n;
    matrix->private  = (Node *) malloc(n * sizeof(Node)); /* array of Nodes */
    matrix->add_row  = list_add_row;
    matrix->mult     = list_mult;
    matrix->free     = list_free;

    rows = (Node *) matrix->private;

    for (i = 0; i < n; ++i) {
        rows[i].col = -1;
        rows[i].next = NULL;
    }

    return matrix;

}

spmat* spmat_allocate_array(int n, int nnz){

    Arr *arr;
    spmat *mat;
    mat=(spmat *) malloc(sizeof(spmat));
    mat->n=n;
    mat->private=(Arr *) malloc(sizeof(Arr)); ;
    mat->add_row = array_add_row;
    mat->mult = array_mult;
    mat->free = array_free;
    arr = (Arr *) mat->private;

    arr->coli=(int *) malloc(nnz*sizeof(double));
    arr->rowptr=(int *) malloc((n+1)*sizeof(double));
    arr->value=(double *) malloc(nnz*sizeof(double));

    return mat;

}

void list_print_matrix(spmat *A)
{
    int i;
    Node *ptr, *head =  (Node *) A->private;

    for  (i = 0; i < A->n ; ++i){
        ptr = head->next;
        while(ptr != NULL){
            printf("%f ", ptr->data);
            ptr = ptr->next;
        }
        printf("\n");
        ++head;
    }
}


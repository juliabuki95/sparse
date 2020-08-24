#include <stdio.h>
#include "spmat.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>
/*babababababa*/
spmat   *create_list_matrix(FILE *file, int n);
spmat   *creat_array_matrix(FILE *file, int n, int nnz);
void    list_print_matrix(spmat *A);
void    print_vector(double *v, int n);
void    standardize(double arr[], int n);
void    swap(double **a, double **b);
void    write_result(char *name, int n, double *b);
int     find_nnz(FILE *file, int n);
double  max_diff(double *u, const double *v, int n);
double  dot_product(const double v[], const double u[], int n);
double  *power_iteration(spmat *A, double *b);
double  *create_random_vector(int n);
double  *read_b0(char *file_name);


int main(int argc, char* argv[])
{
    clock_t start, end;
    int n,nnz;
    char *implementation, *output_file_name;
    double *b;
    FILE* input_file;
    spmat *matrix;

    nnz=0;
    start = clock();
    assert(argc == 4 || argc == 5);

    /* Open file and read size*/
    input_file = fopen(argv[1], "r");
    fread(&n, sizeof(int), 1, input_file);
    fread(&n, sizeof(int), 1, input_file);


    if (argc == 4) {
        /* b0 not provided*/
        output_file_name = argv[2];
        implementation = argv[3];
        b = create_random_vector(n);
    }
    else {
        /* b0  provided*/
        output_file_name = argv[3];
        implementation = argv[4];
        b = read_b0(argv[2]);
    }

    /* Allocate matrix according to implementation */
    if(strcmp("-list", implementation) == 0) {
        matrix = create_list_matrix(input_file, n);
    }
    else{
        nnz= find_nnz(input_file,n);
        matrix=creat_array_matrix(input_file,n,nnz);
    }

    /* Do power iteration*/
    b = power_iteration(matrix, b);

    /* Write result to file*/
    write_result(output_file_name, n, b);

    /* print_vector(b, n);
  */
    /* Free resources*/
    free(b);
    matrix->free(matrix);

    end = clock();
    printf("Runtime: %f\n", ((double) end - start) / CLOCKS_PER_SEC);
    return 0;


}

spmat* create_list_matrix(FILE *file, int n)
{
    int i;
    double *row;
    spmat *matrix;

    matrix = spmat_allocate_list(n);
    row = calloc(n, sizeof(double));

    /*read matrix rows*/
    for (i = 0; i < n; i++) {
        fread(row, sizeof(double), n, file);
        matrix->add_row(matrix, row, i);
    }

    free(row);
    return matrix;

}
spmat* creat_array_matrix(FILE *file, int n, int nnz){
    double *row;
    int i;
    spmat *mat;

    mat=spmat_allocate_array(n,nnz);
    row=calloc(n,sizeof(double));

    for(i=0;i<n;i++){
        fread(row,sizeof(double),n,file);
        mat->add_row(mat,row,i);
    }
    free(row);
    return mat;
}

int find_nnz(FILE *file, int n){

    double *rowbuff;
    int j,k,nnz;

    nnz=0;
    rowbuff= (double*)malloc(n*sizeof(double));
    for(j=0;j<n;j++){

        fread(rowbuff, sizeof(double), n, file);
        for(k=0;k<n;k++){
            if(rowbuff[k]!=0){
                nnz++;
            }
        }
    }
    free(rowbuff);
    rewind(file);
    fread(&n, sizeof(int), 1, file);
    fread(&n, sizeof(int), 1, file);
    return nnz;
}


double *create_random_vector(int n) {

    int i;
    double *b;
    srand(time(NULL));
    b = calloc(n, sizeof(double));

    for (i = 0; i < n; i++) {
        b[i] =  (double) rand();
    }
    return b;
}


double *power_iteration(spmat *A, double *b)
{
    int n;
    double const EPSILON = 0.00001;
    double diff;
    double *tmp;

    n = A->n;

    tmp = calloc(A->n, sizeof(double));
    do {
        /*calculate Ab_k / ||Ab_k|| */
        A->mult(A, b, tmp);

        /*calculate b_k+1*/
        standardize(tmp, n);
        diff = max_diff(b, tmp, n);
        swap(&b, &tmp);

    } while (diff > EPSILON);

    free(tmp);
    return b;
}


void standardize(double arr[], int n)
{
    double norm;
    int i;
    norm = sqrt(dot_product(arr, arr, n));
    for(i = 0; i < n; i++) {
        arr[i] = arr[i] / norm;
    }
    assert(norm >= 0.0);
}


double max_diff(double *u, const double *v, int n)
{
    double diff = 0.0, x;
    int i;
    for(i = 0; i < n; i++) {
        x = fabs(u[i] - v[i]);
        diff = x < diff ?  diff : x;
    }
    return diff;
}


void swap(double **a, double **b)
{
    double *tmp = *a;
    *a = *b;
    *b = tmp;
}


double dot_product(const double v[], const double u[], int n)
{
    double res = 0.0;
    int i;
    for (i = 0; i < n; i++) {
        res += v[i] * u[i];
    }
    return res;
}

double *read_b0(char *file_name)
{
    int n;
    FILE* file;
    double *b;

    file = fopen(file_name, "rb");
    fread(&n, sizeof(int), 1, file);
    fread(&n, sizeof(int), 1, file);

    b = calloc(n, sizeof(double));
    fread(b, sizeof(double), n, file);
    fclose(file);

    return b;

}

void write_result(char *name, int n, double *b)
{
    FILE *file;
    int dim[2];

    dim[0] = 1;
    dim[1] = n;
    file = fopen(name, "wb");
    fwrite(dim , sizeof(int), 2, file);
    fwrite(b , sizeof(double), n, file);
    fclose(file);
}

void print_vector(double *v, int n) {
    int i;
    for (i = 0; i < n; ++i) {
        printf("%f\n", v[i]);
    }

}

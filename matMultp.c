#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int** readMat (char* file);

// int* a[];
// int* b[];
int** a;
int** b;

void main(){
    // a=malloc(sizeof(int)*400);
    // b=malloc(sizeof(int)*400);
    // char ar[6]="ahmed";
    // printf(strcat(ar,".txt"));
    // int a[][];
    a=malloc(sizeof(*a)*20*20);
    a = readMat("a");
    for (int i=0;i<3;i++){
        for (int j=0;j<5;j++){
            printf("%d ",a[i][j]);
        }
        printf("\n");
    }
    // printf(*a);
}

int** readMat (char* file){

    char* path = malloc (strlen(file)+4);
    strcpy (path,file);
    strcat(path,".txt");
    FILE *f = fopen(file, "r");
    if (f==NULL){
        printf("The File \"%s\" doeasn't Exist.\n",file);
        return -1;
    } 

    int rows=0,cols=0;
    int ind=0;
    char dim[50];
    fgets(dim,50,f);
    // printf(dim);
    char* parse = strtok(dim," =");
    while (parse!=NULL){
        if (ind==0 && strncmp(parse,"row",3)!=0) return -2;
        else if (ind==2 && strncmp(parse,"col",3)!=0) return -2;
        else if (ind==1) rows=atoi(parse);
        else if (ind==3) cols = atoi(parse);
        ind++;
        // printf("%s\n",parse);
        parse = strtok (NULL," =");
    }
    
    if (rows==0 || cols==0) return -2;
    int mat[rows][cols];
    int** m;
    m=malloc(sizeof (*m) *rows);
    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            fscanf(f,"%d",&mat[i][j]);
        }
        *(m+i) = mat[i];
    }

    // for (int i=0;i<3;i++){
    //     for (int j=0;j<5;j++){
    //         printf("%d ",m[i][j]);
    //     }
    //     printf("\n");
    // }

    // matrix = malloc( sizeof *matrix * rows );
    // matrix=malloc(sizeof(int)*rows*cols);
    // for (int i=0;i<rows;i++){
    //     for (int j=0;j<cols;j++){
    //         *(matrix+i)[j] = mat[i][j];
    //         // printf ("%d",(*(matrix+i)));
    //     }
    //     printf("\n");
    // }

    // for (int i=0;i<rows+cols;i++){
    //     printf ("%d\n",(*(matrix+i)));
    // }

    fclose (f);
    return m;
    
}

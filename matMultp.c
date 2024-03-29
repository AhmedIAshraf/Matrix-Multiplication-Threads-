#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct matrix
{
    int rows;
    int cols;
    int** m;
}mat;

typedef struct elemThrPatam{
    int row;
    int col;
}param;

typedef struct Information{
    char* mat1;
    char* mat2;
    char* dest;
}info;

mat readMat (char* file);
info parseInput (char* input);
long unsigned SolveNoThreads();
long unsigned SolveRowThreads();
long unsigned SolveElementThreads();
void writeFile(int n);
void* computeRow (void* ind);
void* computeElement (param* ptr);
void Error(char* msg);

mat a,b,c;
char* dest;

void main(){
    
    char line[150];
    char* mat1,*mat2;
    scanf("%[^\n]s", line);
    info recieve=parseInput(line);
    mat1=recieve.mat1, mat2=recieve.mat2, dest=recieve.dest;

    a = readMat(mat1);
    b = readMat(mat2);
    if (a.cols!=b.rows) Error("Dimentions can't be Correct");

    c.rows=a.rows; c.cols=b.cols;
    c.m=(int**) malloc (c.rows*sizeof(int*));
    for (int i=0;i<c.rows;++i) {
        c.m[i]=(int*)malloc(c.cols*sizeof(int));
    }
    
    long unsigned time1=SolveNoThreads();
    long unsigned time2=SolveRowThreads();
    long unsigned time3=SolveElementThreads();
    
    printf("--------Comparison---------\n");
    printf("[1] Method : Single Thread, Time Taken in Ms = %lu\n",time1);
    printf("[2] Method : Thread per Row, Threads Used = %d, Time Taken in Ms = %lu\n",a.rows,time2);
    printf("[3] Method : Thread per Element, Threads Used = %d, Time Taken in Ms = %lu\n",c.cols*c.rows,time3);
    
    free(a.m), free(b.m), free(c.m);
}

long unsigned SolveNoThreads(){
    
    struct timeval stop, start;
    gettimeofday(&start, NULL);

    for (int i=0;i<a.rows;i++){
        for (int j=0;j<b.cols;j++){
            long res=0;
            for (int k=0;k<b.rows;k++){
                res += (a.m[i][k]*b.m[k][j]);
            }
            c.m[i][j]=res;
        }
    }
    
    gettimeofday(&stop, NULL);
    writeFile(1);
    return stop.tv_usec - start.tv_usec;
}

long unsigned SolveRowThreads(){
    
    struct timeval stop, start;
    pthread_t threads[a.rows];
    gettimeofday(&start, NULL);

    for (int i=0;i<a.rows;i++){
        int res= pthread_create(&threads[i],NULL,&computeRow,(void*) i);
        if(res!=0) Error("Something Happened While Creating Thread");
    }
    for (int i=0;i<a.rows;i++){
        if(pthread_join(threads[i],NULL)!=0) Error("Threads got an Error while Joining Threads");
    }

    gettimeofday(&stop, NULL);
    writeFile(2);
    return stop.tv_usec - start.tv_usec;
}

void* computeRow (void* ind){
    int row = ind;
    for (int j=0;j<b.cols;j++){
        c.m[row][j]=0;
        for (int k=0;k<b.rows;k++){
            c.m[row][j] += (a.m[row][k]*b.m[k][j]);
        }
    }
}

long unsigned SolveElementThreads(){
    
    struct timeval stop, start;
    pthread_t threads[c.rows][c.cols];
    param* ptr = (param*) malloc(sizeof(param));
    gettimeofday(&start, NULL);

    for (int i=0;i<c.rows;i++){
        for (int j=0;j<c.cols;j++){
            ptr->row=i; ptr->col=j;
            int res= pthread_create(&threads[i][j],NULL,&computeElement,ptr);
            if(res!=0) Error("Something Happened While Creating Thread");
        }
    }
    for (int i=0;i<c.rows;i++){
        for (int j=0;j<c.cols;j++){
            if(pthread_join(threads[i][j],NULL)!=0) Error("Threads got an Error while Joining Threads"); 
        }
    }

    gettimeofday(&stop, NULL);
    writeFile(3);
    free(ptr);
    return stop.tv_usec - start.tv_usec;
}

void* computeElement (param* ptr){
    c.m[ptr->row][ptr->col]=0;
    for (int k=0;k<b.rows;k++){
        c.m[ptr->row][ptr->col] += (a.m[ptr->row][k]*b.m[k][ptr->col]);
    }
}

void writeFile(int n){
    char* name= malloc(strlen(dest));
    strcpy(name,dest);
    char* method;
    if (n==1) {
        name = (char *) realloc(name, strlen(dest)+strlen("_per_matrix.txt"));
        strcat(name,"_per_matrix.txt");
        method="Method: A thread per matrix";
    }else if (n==2) {
        name = (char *) realloc(name, strlen(dest)+strlen("_per_row.txt"));
        strcat(name,"_per_row.txt");
        method="Method: A thread per row";
    }else if (n==3) {
        name = (char *) realloc(name, strlen(dest)+strlen("_per_element.txt"));
        strcat(name,"_per_element.txt");
        method="Method: A thread per element";
    }
     
    FILE *f = fopen(name,"w");
    fprintf(f,"%s\nrow=%d col=%d\n",method,c.rows,c.cols);
    for (int i=0;i<c.rows;i++){
        for (int j=0;j<c.cols;j++){
            fprintf(f,"%d ",c.m[i][j]);
        }
        fprintf(f,"\n");
    }
    free(name);
    fclose(f);
}

info parseInput (char* input){
    char* mat1="a", *mat2="b", *dest="c";
    char* parse = strtok(input," ");
    for (int i=0;i<4&&parse!=NULL;i++){
        // if(i==0&&strcmp(parse,"./matMultp")!=0) Error("Wrong Command");
        if(i==0){
            int n = chdir(parse);
            if (n!=0) Error("No Such File or Directory");
        }
        else if(i==1) mat1=parse;
        else if(i==2) mat2=parse;
        else if(i==3) dest=parse;
        parse = strtok(NULL," ");
    }
    info toReturn={.mat1=mat1 , .mat2=mat2 , .dest=dest};
    return toReturn;
}  

mat readMat (char* file){

    char* path = malloc (strlen(file)+4);
    strcpy (path,file);
    strcat(path,".txt");
    FILE *f = fopen(file, "r");
    if (f==NULL) Error("No File with Such Name");
    free(path);

    int rows=0,cols=0;
    int ind=0;
    char dim[50];
    fgets(dim,50,f);
    
    char* parse = strtok(dim," =");
    while (parse!=NULL){
        if (ind==0 && strncmp(parse,"row",3)!=0) Error("a Word doesn't Match the Standard Syntax of the Input");
        else if (ind==2 && strncmp(parse,"col",3)!=0) Error("a Word doesn't Match the Standard Syntax of the Input");
        else if (ind==1) rows=atoi(parse);
        else if (ind==3) cols = atoi(parse);
        ind++;
        parse = strtok (NULL," =");
    }
    
    if (rows==0 || cols==0) Error("Matrix can't Have 0 Rows or Columns");
    int** m=(int**) malloc (rows*sizeof(int*));
    for (int i=0;i<rows;++i) {
        m[i]=(int*)malloc(cols*sizeof(int));
    }
    for (int i=0;i<rows;i++){
        for (int j=0;j<cols;j++){
            fscanf(f,"%d",&m[i][j]);
        }
    }

    fclose (f);
    mat final={.rows =rows, .cols =cols, .m = m};
    return final;
}

void Error(char* msg){
    printf("Error! %s.\n",msg);
    printf("The Program will Terminate, Fix the Error and Run it again.\n");
    exit(-1);
}

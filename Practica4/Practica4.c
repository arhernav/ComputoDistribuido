#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <stdbool.h>
#define TAG_ARR 1

void inicializa()
{
	time_t t;
	srand(time(&t));
}

int* selectionSort(int* arr)
{
    int min;
    int aux;
    for(int i = 0; i < 3; i++)
    {
        min = i;
        for(int j = i+1; j < 3; j++)
        {
            if(arr[j] < arr[min])
            {
                min = j;
            }
        }
        aux = arr[i];
        arr[i] = arr[min];
        arr[min] = aux;
    }
    return arr;
}

//Implementación de merge sort
int main(int argc, char** argv){
    int size, rank;
    MPI_Init(&argc, &argv);
    //Se ejecuta por todos los nodos
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    inicializa();
    int* arr;
    if(rank == 0)
    {
        arr = (int*)malloc(sizeof(int)*(3*size));
        //Se genera el arreglo a ordenar
        for(int i = 0; i < 3*size; i++)
        {
            arr[i] = rand()%100;
        }
        //Se imprime en terminal el arreglo original
        printf("Arreglo original:\n[");
        for(int i = 0; i < 3*size; i++)
        {
            printf(" %d",arr[i]);
            if(i != (3*size)-1)
            {
                printf(",");
            }
        }
        printf(" ]\n");
        int* subarr = (int*)malloc(sizeof(int)*3);
        //Se envian a los demás nodos el subarreglo a ordenar
        for(int i = 3; i < 3*size; i++)
        {
            subarr[i%3] = arr[i];
            if(i%3 == 2)
            {
                MPI_Send(subarr, 3, MPI_INT, (i-2)/3, TAG_ARR, MPI_COMM_WORLD);
            }
        }
        //Ordena su subarreglo con selection sort
        arr = selectionSort(arr);
        int* resultado = (int*)malloc(sizeof(int)*(3*size));
        for(int i = 1; i < size; i++)
        {
            //Se recive el subarreglo ordenado del nodo i
            MPI_Recv (subarr, 3, MPI_INT, i, TAG_ARR, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int j = 0;
            int k = 0;
            //Se mezclan los subarreglos
            while(j < 3*i && k < 3)
            {
                if(arr[j] < subarr[k])
                {
                    resultado[j+k] = arr[j];
                    j++;
                }else
                {
                    resultado[j+k] = subarr[k];
                    k++;
                }
            }
            while(k < 3)
            {
                resultado[j+k] = subarr[k];
                k++;
            }
            while(j < 3*i)
            {
                resultado[j+k] = arr[j];
                j++;
            }
            //Se copia el arreglo resultante de la mezcla en arr
            for(int l = 0; l < 3*(i+1); l++)
            {
                arr[l] = resultado[l];
            }
        }
        //Se imprime en terminal el arreglo ordenado
        printf("Arreglo ordenado:\n[");
        for(int i = 0; i < 3*size; i++)
        {
            printf(" %d",resultado[i]);
            if(i != (3*size)-1)
            {
                printf(",");
            }
        }
        printf(" ]\n");
    }else
    {
        //Reciven el subarreglo a ordenar
        arr = (int*)malloc(sizeof(int)*3);
        MPI_Recv (arr, 3, MPI_INT, 0, TAG_ARR, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //Ordena su subarreglo con selection sort
        arr = selectionSort(arr);
        MPI_Send(arr, 3, MPI_INT, 0, TAG_ARR, MPI_COMM_WORLD);
    }
}

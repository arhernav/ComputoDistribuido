#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <stdbool.h>
#define TAG_REPORT 1

void inicializa(){
	time_t t;
	srand(time(&t));
}

//Implementación del Algoritmo del Abusón
int main(int argc, char** argv){
    int size, rank;
    MPI_Init(&argc, &argv);
    //Se ejecuta por todos los nodos
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    inicializa(rank);
    int lider = -1;
    int caido = 0;
    //Los nodos caidos son el penúltimo y antepenultimo
    if(rank < size-1 && rank > size-4)
    {
        caido = 1;
    }
    //Se elige el nodo no caido para convocar una elección
    int nodoEleccion = rand()%size;
    while(nodoEleccion < size-1 && nodoEleccion > size-4)
    {
        nodoEleccion = rand()%size;
    }
    while (true)
    {
        //Los nodos mayores al nodo que convoca la elección envian
        //a este nodo si son nodos caidos o no
        if(rank > nodoEleccion)
        {
            MPI_Send(&caido, 1, MPI_INT, nodoEleccion, TAG_REPORT, MPI_COMM_WORLD);
        }
        if(rank == nodoEleccion)
        {
            int reportes [size];
            //El nodo que convoco la elección recive los reportes
            for(int i = nodoEleccion+1 ; i < size; i++)
            {
                MPI_Recv(&reportes[i], 1, MPI_INT, i, TAG_REPORT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            //El que convoco elección verifica si hay nodos mayores no caidos
            int hayMayores = -1;
            for(int i = nodoEleccion+1 ; i < size; i++)
            {
                if(reportes[i] == 0)
                {
                    hayMayores = -2;
                    break;
                }
            }
            for(int i = 0; i < size; i++)
            {
                if(i != rank)
                {
                    //Envia a los demás nodos que se debe de hacer otra elección
                    if(hayMayores == -2)
                    {
                        MPI_Send(&hayMayores, 1, MPI_INT, i, TAG_REPORT, MPI_COMM_WORLD);
                    }else
                    //Envia a los demás nodos que es el nodo mayor
                    {
                        MPI_Send(&rank, 1, MPI_INT, i, TAG_REPORT, MPI_COMM_WORLD);
                    }
                }
            }
            if(hayMayores == -1)
            {
                lider = rank;
                break;
            }
        }
        if(rank != nodoEleccion)
        {
            MPI_Recv(&lider, 1, MPI_INT, nodoEleccion, TAG_REPORT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //Caso en el que el nodo que hizo la elección es el mayor
            if(lider != -2)
            {
                break;
            }
        }
        int nodoAux = rand()%size;
        //Se elige un nodo mayor que no este caido para realizar una nueva eleccion
        while(nodoAux < size-1 && nodoAux > size-4 && nodoAux > nodoEleccion){
            nodoAux = rand()%size;
        }
        nodoEleccion = nodoAux;
    }
    if(caido == 0)
    {
        printf("Nodo %d no caido. El lider es el nodo %d\n",rank, lider);
    }else
    {
        printf("Nodo %d caido. El lider es el nodo %d\n",rank, lider);
    }
}

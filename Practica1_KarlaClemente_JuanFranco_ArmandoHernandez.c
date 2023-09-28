#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#define TAG_DISTANCE 1

void inicializa(){
	time_t t;
	srand(time(&t));
}

//Implementación de Dijkstra
int main(int argc, char** argv){
    int size, rank;
    MPI_Init(&argc, &argv);
    //Se ejecuta por todos los nodos
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int* distancias = (int*)malloc(sizeof(int)*size);
		int* distanciasSend = (int*)malloc(sizeof(int)*size);
    //Se inicializa las distancias
    for(int i = 0; i < size; i++)
    {
        if(i == rank)
        {
            distancias[i] = 0;
        }else
        {
            distancias[i] = -1;
        }
    }
    //Se establece para todo vertice i, P[i] = a
    int* vertices = (int*)malloc(sizeof(int)*size);
    vertices[rank] = rank;
    //Se obtiene la distancia de i a los otros vecinos
    for(int i = 0; i < size; i++)
    {
        if(i != rank)
        {
            int random = rand()%1000+1;
            MPI_Send(&random, 1, MPI_INT, i, TAG_DISTANCE, MPI_COMM_WORLD);
						MPI_Recv(&distancias[i], 1, MPI_INT, i, TAG_DISTANCE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    //Se obtiene el vértice s tal que no existe otro vértice w tal que D[w] < D[s]
    int minimo = 0;
    for(int i = 0; i < size; i++)
    {
        if(distancias[i] < distancias[minimo])
        {
            minimo = i;
        }
    }

		for (int i = 0; i < size; i++) {
			printf("process %d: retraso en %d: %d\n",rank,i, distancias[i]);
		}
    MPI_Finalize();
	return 0;
}

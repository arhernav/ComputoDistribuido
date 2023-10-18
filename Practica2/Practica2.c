#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <stdbool.h>
#define TAG_REPORT 1

//Semilla distinta para cada uno de los nodos
void inicializa1(int rank){
	time_t t;
	srand(time(&t)+rank);
}

void inicializa2(){
	time_t t;
	srand(time(&t));
}

//El número máximo de rondas de comunicación
int maxRondas = 20;

//Implementación del Algoritmo del Rey
int main(int argc, char** argv){
    int size, rank;
    MPI_Init(&argc, &argv);
    //Se ejecuta por todos los nodos
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int numTraidores = (size-1)/4;
    int minVotos = (size/2)+numTraidores+1;
    inicializa1(rank);
    int reportes [rank];
    int ataca = rand()%2;
    for(int i = 0; i < maxRondas; i++)
    {
        int acuerdo [rank];
        bool terminar = false;
        reportes[rank] = ataca;
        //Se envia a los otros nodos la desición de si se ataca o retira
        for (int j = 0; j < size; j++)
        {
            if(j != rank)
            {
                //Caso cuando es un traidor
                if(rank < numTraidores){
                    ataca = rand()%2;
                }
                MPI_Send(&ataca, 1, MPI_INT, j, TAG_REPORT, MPI_COMM_WORLD);
            }
        }
        //Recive las desiciones de los demás nodos
        for(int j = 0 ; j < size; j++)
        {
            if(j != rank)
            {
                MPI_Recv(&reportes[j], 1, MPI_INT, j, TAG_REPORT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        int numVotosRetirada = 0;
        //Se cuentan los votos
        for(int j = 0; j < size; j++)
        {
            if(reportes[j] == 0)
            {
                numVotosRetirada++;
            }
        }
        //Caso cuando se llega a un acuerdo
        if(numVotosRetirada >= minVotos || (size - numVotosRetirada) >= minVotos)
        {
            acuerdo[rank] = 1;
        }
        //Se envia a los otros nodos si llego a un acuerdo
        for (int j = 0; j < size; j++)
        {
            if(j != rank)
            {
                MPI_Send(&acuerdo[rank], 1, MPI_INT, j, TAG_REPORT, MPI_COMM_WORLD);
            }
        }
        //Recive si llegaron a un acuerdo los demás nodos
        for(int j = 0 ; j < size; j++)
        {
            if(j != rank)
            {
                MPI_Recv(&acuerdo[j], 1, MPI_INT, j, TAG_REPORT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
        //Caso en el que todos llegaron a un acuerdo
        for(int j = 1; j < size; j++)
        {
            if(acuerdo[j-1] != acuerdo[j])
            {
                break;
            }
            if(j == size-1)
            {
                terminar = true;
            }
        }
        if(terminar)
        {
            break;
        }
        //Se elije al rey
        inicializa2();
        int rey = rand()%size;
        if(rank == rey)
        {
            //Manda el rey la desición
            for (int j = 0; j < size; j++)
            {
                if(j != rank)
                {
                    //Caso cuando es un traidor
                    if(rank < numTraidores){
                        ataca = rand()%2;
                    }
                    MPI_Send(&ataca, 1, MPI_INT, j, TAG_REPORT, MPI_COMM_WORLD);
                }
            }
        }else
        {
            MPI_Recv(&ataca, 1, MPI_INT, rey, TAG_REPORT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    if(ataca == 0)
    {
        if(rank < numTraidores)
        {
            printf("Nodo %d era traidor y eligio Retirada\n",rank);
        }else
        {
            printf("Nodo %d era leal y eligio Retirada\n",rank);
        }
    }else
    {
        if(rank < numTraidores)
        {
            printf("Nodo %d era traidor y eligio Atacar\n",rank);
        }else
        {
            printf("Nodo %d era leal y eligio Atacar\n",rank);
        }
    }
    MPI_Finalize();
    return 0;
}

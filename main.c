#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int Min(int a,int b);

int Low(int rank, int sizee, int n, int m);

int High(int rank, int sizee, int n, int m);

int Lenght(int rank, int sizee, int n, int m);

int main(int argc, char** argv){

    int     count,count2,aux;
    double  elapsed_time;
    int     first,global_count,global_count2,high_value,i,my_rank ,index,low_value,n,m,my_size,prime;
    int     size,first_value_index,prime_doubled,koren,p;
    int     num_per_block,block_low_value,block_high_value;
    char*   marked,*primes;
	double maxtime = 0,tm,s,max;

    MPI_Init(&argc, &argv);

    /* start the timer */
    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &my_size);

    if (argc != 3)    {
        if (my_rank == 0)
            printf("Range need: %s <n> <m>\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }

    m = atoi(argv[1]);
    aux = m;
    if(m == 1)m--;
    n = atoi(argv[2]);

    low_value  = 1 + Low(my_rank, my_size, n - 1,m)  * 2;
    high_value = 1 + High(my_rank, my_size, n - 1,m) * 2;
    size       = Lenght(my_rank, my_size, n - 1,m);

    koren = sqrt(n);
    primes = (char*)calloc(koren + 1, 1);
    for (p = 2; p <= koren; p += 2)    {
        primes[p] = 1;

    }

    for (prime = 3; prime <= koren; prime += 2)    {
        if (primes[prime] == 1)
            continue;

        for (p = prime * 2; p <= koren; p += prime)    {
            primes[p] = 1;
        }
    }

    marked = (char*)calloc(size * sizeof(char), 1);
    marked[1] = 0;
    num_per_block    = 1048576;
    block_low_value  = low_value;
    block_high_value = Min(high_value,low_value + num_per_block * 2);

//    printf("rank : %d\n",my_rank);
    printf("%d %d\n",low_value,high_value);
    //printf("s : %d\n",size);


    for (i = 0;i < size; i += num_per_block){
        for (prime = 3; prime <= koren; prime++){

            if (primes[prime] == 1)
                continue;

            if (prime * prime > block_low_value){
                first = prime * prime;
            }
           else{
                if (!(block_low_value % prime))    {
                    first = block_low_value;
                }
                else    {
                    first = prime - (block_low_value % prime) +
                            block_low_value;
                }
           }

           if ((first + prime) & 1) // is odd
              first += prime;


           first_value_index = (first - 1) / 2 - Low(my_rank, my_size, n - 1,m);
           prime_doubled     = prime * 2 ;

           for (i = first; i <= high_value; i += prime_doubled)   {
			   marked[first_value_index] = 1;
               first_value_index += prime;
           }
        }

        block_low_value += num_per_block * 2;
        block_high_value = Min(high_value,block_high_value + num_per_block * 2);

    }




	char file[] = "rankX.bin";
	file[4] = my_rank+'0';
    FILE * f = fopen(file,"a+b");
    count = 0;
    count2 = 0;

    if(my_rank == 0){
        if(2 >=m && 2 <=n){
                fprintf(f,"%d\n",2);
                 count++;}

    }
    int pos;

    if(my_rank == my_size -1)
        size++;

    for (i = 0; i < size; i++){
            pos = (i+Low(my_rank, my_size, n - 1,m))*2+1;
            printf("rank - %d primo- %d\n",my_rank,pos);
        if (!marked[i]){
            if(pos <= m)
                count2++;
             if(pos >=m && pos <=n && pos != 1){
                count++;
                fprintf(f,"%d\n",pos);

                }
            }
    }
    printf("rank - %d p- %d\n",my_rank,count);
    fclose(f);
    MPI_Reduce(&count, &global_count, 1, MPI_INT,MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&count2, &global_count2, 1, MPI_INT,MPI_SUM, 0, MPI_COMM_WORLD);

    tm = MPI_Wtime();
    elapsed_time += tm;
	if(maxtime < tm)
		maxtime = tm;

    //printf("Max time: %10.6fs\n",elapsed_time);

    MPI_Reduce(&elapsed_time, &s, 1, MPI_DOUBLE,MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&elapsed_time, &max, 1, MPI_DOUBLE,MPI_MAX, 0, MPI_COMM_WORLD);


    if (my_rank == 0)   {
        //global_count += 1;
        //global_count2 += 1;
       printf("%d primes are less than or equal to %d\n",global_count, n);

        printf("there are %d number between %d and %d\n",global_count, aux,n);
        printf("Total elapsed time: %10.6fs\n",s);
        printf("Max time: %10.6fs\n",max);

        char file[] = "time";
        FILE * g = fopen(file,"a+b");
                    fprintf(f,"%lf %lf\n",s,max);

        fclose(g);

    }

    MPI_Finalize();

    return 0;
}



int Min(int a,int b){
    return (a<b)?a:b;
}
int Low(int rank, int sizee, int n, int m){
    return (m+ rank*(n-m)/sizee +1)/2;
}
int High(int rank, int sizee, int n, int m){
    return Low(rank+1,sizee,n,m)-1;
}
int Lenght(int rank, int sizee, int n, int m){
    return Low(rank+1,sizee,n,m)-Low((rank), sizee, n,m);
}

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <string.h>

typedef struct {
    int M;
    int N;
    int P;
} MatrixDims;

MPI_Datatype create_matrix_dims_type() {
    MPI_Datatype dtype;
    int blen[3] = {1,1,1};
    MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint displ[3];
    
    MatrixDims dummy;
    MPI_Get_address(&dummy.M, &displ[0]);
    MPI_Get_address(&dummy.N, &displ[1]);
    MPI_Get_address(&dummy.P, &displ[2]);
    
    displ[1] -= displ[0];
    displ[2] -= displ[0];
    displ[0] = 0;

    MPI_Type_create_struct(3, blen, displ, types, &dtype);
    MPI_Type_commit(&dtype);
    return dtype;
}

void initialize_matrix(double *mat, int rows, int cols) {
    for(int i=0; i<rows*cols; i++)
        mat[i] = (double)rand()/RAND_MAX;
}

void print_result(double *mat, int rows, int cols) {
    for(int i=0; i<rows; i++) {
        for(int j=0; j<cols; j++)
            printf("%8.2f", mat[i*cols+j]);
        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    MPI_Datatype dims_type = create_matrix_dims_type();
    
    if(argc == 4) {
        MatrixDims dims;
        if(rank == 0) {
            dims.M = atoi(argv[1]);
            dims.N = atoi(argv[2]);
            dims.P = atoi(argv[3]);
            if(dims.M<128||dims.M>2048||dims.N<128||dims.N>2048||dims.P<128||dims.P>2048) {
                fprintf(stderr, "Input out of valid range [128,2048]\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
        MPI_Bcast(&dims, 1, dims_type, 0, MPI_COMM_WORLD);
        
        double *A=NULL, *B=NULL, *C=NULL, *B_buf;
        double t_start;
        if(rank == 0) {
            A = (double*)malloc(dims.M * dims.N * sizeof(double));
            B = (double*)malloc(dims.N * dims.P * sizeof(double));
            C = (double*)malloc(dims.M * dims.P * sizeof(double));
            initialize_matrix(A, dims.M, dims.N);
            initialize_matrix(B, dims.N, dims.P);
            t_start = MPI_Wtime();
        }
        
        B_buf = (double*)malloc(dims.N*dims.P*sizeof(double));
        if(rank == 0) memcpy(B_buf, B, dims.N*dims.P*sizeof(double));
        MPI_Bcast(B_buf, dims.N*dims.P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        int rows_per_proc = dims.M/size;
        int rem = dims.M%size;
        int local_rows = rows_per_proc + (rank<rem?1:0);
        double *local_A = (double*)malloc(local_rows*dims.N*sizeof(double));
        double *local_C = (double*)malloc(local_rows*dims.P*sizeof(double));
        
        int *sendcounts = (int*)malloc(size*sizeof(int));
        int *displs = (int*)malloc(size*sizeof(int));
        int offset = 0;
        for(int i=0; i<size; i++) {
            sendcounts[i] = (rows_per_proc + (i<rem?1:0)) * dims.N;
            displs[i] = offset;
            offset += sendcounts[i];
        }
        MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE,
                    local_A, local_rows*dims.N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        for(int i=0; i<local_rows; i++) {
            for(int j=0; j<dims.P; j++) {
                local_C[i*dims.P+j] = 0;
                for(int k=0; k<dims.N; k++)
                    local_C[i*dims.P+j] += local_A[i*dims.N+k] * B_buf[k*dims.P+j];
            }
        }
        
        for(int i=0; i<size; i++) 
            sendcounts[i] = (rows_per_proc + (i<rem?1:0)) * dims.P;
        MPI_Gatherv(local_C, local_rows*dims.P, MPI_DOUBLE,
                   C, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        if(rank == 0) {
            printf("\nTime: %.2f ms\n", (MPI_Wtime()-t_start)*1000);
            if(dims.M <= 8) {
                printf("Matrix A:\n"); print_result(A, dims.M, dims.N);
                printf("\nMatrix B:\n"); print_result(B, dims.N, dims.P);
                printf("\nMatrix C:\n"); print_result(C, dims.M, dims.P);
            }
            free(A); free(B); free(C);
        }
        free(sendcounts); free(displs);
        free(local_A); free(local_C); free(B_buf);
    } 
    else {
        int sizes[] = {128,256,384,512,640,768,896,1024,1152,1280,1408,1536,1664,1792,1920,2048};
        const int NUM_RUNS = 5;
        
        if(rank == 0) 
            printf("Processes: %d\n", size);
        
        for(int s=0; s<16; s++) {
            if(rank == 0)
                printf("Size: %4d | Runs: ", sizes[s]);
            
            MatrixDims dims = {sizes[s], sizes[s], sizes[s]};
            MPI_Bcast(&dims, 1, dims_type, 0, MPI_COMM_WORLD);
            
            int rows_per_proc = dims.M/size;
            int rem = dims.M%size;
            int *sendcounts = (int*)malloc(size*sizeof(int));
            int *displs = (int*)malloc(size*sizeof(int));
            int offset = 0;
            for(int i=0; i<size; i++) {
                sendcounts[i] = (rows_per_proc + (i<rem?1:0)) * dims.N;
                displs[i] = offset;
                offset += sendcounts[i];
            }
            
            double total_time = 0;
            for(int run=0; run<NUM_RUNS; run++) {
                double *A=NULL, *B=NULL, *C=NULL, *B_buf;
                double t_start;
                
                if(rank == 0) {
                    A = (double*)malloc(dims.M*dims.N*sizeof(double));
                    B = (double*)malloc(dims.N*dims.P*sizeof(double));
                    C = (double*)malloc(dims.M*dims.P*sizeof(double));
                    initialize_matrix(A, dims.M, dims.N);
                    initialize_matrix(B, dims.N, dims.P);
                    t_start = MPI_Wtime();
                }
                
                B_buf = (double*)malloc(dims.N*dims.P*sizeof(double));
                if(rank == 0) memcpy(B_buf, B, dims.N*dims.P*sizeof(double));
                MPI_Bcast(B_buf, dims.N*dims.P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
                
                int local_rows = rows_per_proc + (rank<rem?1:0);
                double *local_A = (double*)malloc(local_rows*dims.N*sizeof(double));
                double *local_C = (double*)malloc(local_rows*dims.P*sizeof(double));
                
                MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE,
                            local_A, local_rows*dims.N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
                
                for(int i=0; i<local_rows; i++) {
                    for(int j=0; j<dims.P; j++) {
                        local_C[i*dims.P+j] = 0;
                        for(int k=0; k<dims.N; k++)
                            local_C[i*dims.P+j] += local_A[i*dims.N+k] * B_buf[k*dims.P+j];
                    }
                }
                
                MPI_Gatherv(local_C, local_rows*dims.P, MPI_DOUBLE,
                           C, sendcounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
                
                if(rank == 0) {
                    printf("%8.2f ms\t", MPI_Wtime());
                    total_time += (MPI_Wtime() - t_start)*1000;
                    free(A); free(B); free(C);
                }
                free(local_A); free(local_C); free(B_buf);
            }
            
            if(rank == 0)
                printf(" | Avg Time: %8.2f ms\n", total_time/NUM_RUNS);
            
            free(sendcounts); free(displs);
        }
    }
    
    MPI_Type_free(&dims_type);
    MPI_Finalize();
    return 0;
}

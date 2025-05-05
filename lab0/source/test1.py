import random
import time

def generate_matrix(N):
    return [random.random() for _ in range(N*N)]

def matmul_basic(A, B, N):
    C = [0.0] * (N * N)
    for i in range(N):
        for j in range(N):
            sum_val = 0.0
            for k in range(N):
                sum_val += A[i*N + k] * B[k*N + j]
            C[i*N + j] = sum_val
    return C

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: python test1.py <N>")
        sys.exit(1)

    N = int(sys.argv[1])
        
    A = generate_matrix(N)
    B = generate_matrix(N)
    
    start = time.perf_counter()
    C = matmul_basic(A, B, N)
    end = time.perf_counter()
    
    print(f"Time (Python): {(end - start) * 1000:.0f} ms")

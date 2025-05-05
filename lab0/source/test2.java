package lab0;

import java.util.Random;

public class test2 {
    public static void main(String[] args) {
        if (args.length != 1) {
            System.err.println("Usage: java -cp . lab0.test2 <N>");
            System.exit(1);
        }
        int N = Integer.parseInt(args[0]);
        
        double[] A = new double[N * N];
        double[] B = new double[N * N];
        
        generateMatrix(A, N);
        generateMatrix(B, N);
        
        long start = System.nanoTime();
        double[] C = matmulBasic(A, B, N);
        long end = System.nanoTime();
        
        double durationMs = (end - start) / 1e6;
        System.out.printf("Time (Java): %.0f ms\n", durationMs);
    }

    public static void generateMatrix(double[] mat, int N) {
        Random rand = new Random();
        for (int i = 0; i < N * N; i++) {
            mat[i] = rand.nextDouble();
        }
    }
    
    public static double[] matmulBasic(double[] A, double[] B, int N) {
        double[] C = new double[N * N];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                double sum = 0.0;
                for (int k = 0; k < N; k++) {
                    sum += A[i * N + k] * B[k * N + j];
                }
                C[i * N + j] = sum;
            }
        }
        return C;
    }
}

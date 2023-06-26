/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 * 
 * Name: GJX
 * ID: 521021910197
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int x, y, z, a, b, c, d, e, f, g, h;
    if(M == 32 && N == 32)
    {
        for(x = 0; x < 32; x += 8)
        for(y = 0; y < 32; y += 8)
        {
            /*8 * 8 block move each line to B[][col] */
            for(z = x; z < x + 8; z++)
            {
                a = A[z][y];
                b = A[z][y+1];
                c = A[z][y+2];
                d = A[z][y+3];
                e = A[z][y+4];
                f = A[z][y+5];
                g = A[z][y+6];
                h = A[z][y+7];
                B[y][z] = a;
                B[y+1][z] = b;
                B[y+2][z] = c;
                B[y+3][z] = d;
                B[y+4][z] = e;
                B[y+5][z] = f;
                B[y+6][z] = g;
                B[y+7][z] = h;
            }
        }
    }
    /*8 * 8 block 4 * 4 each*/
    else if(M == 64 && N == 64)
    {
        for (x = 0; x < 64; x += 8)
        for (y = 0; y < 64; y += 8)
        {
            /*When row = col*/
            if(x == y)
            {
                /*4 * 8 move to B[1] B[2] wrong place but low miss*/
                for(z = x; z < x + 4; z++)
                {
                    a = A[z][y];
                    b = A[z][y + 1];
                    c = A[z][y + 2];
                    d = A[z][y + 3];
                    e = A[z][y + 4];
                    f = A[z][y + 5];
                    g = A[z][y + 6];
                    h = A[z][y + 7];
                    B[y][z] = a;
                    B[y + 1][z] = b;
                    B[y + 2][z] = c;
                    B[y + 3][z] = d;
                    B[y][z + 4] = e;
                    B[y + 1][z + 4] = f;
                    B[y + 2][z + 4] = g;
                    B[y + 3][z + 4] = h;
                }
                /*4 * 4 move B[2] to B[3] to right place*/
                for(z = y; z < y + 4; z++)
                {
                    a = B[z][x + 4];
                    b = B[z][x + 5];
                    c = B[z][x + 6];
                    d = B[z][x + 7];
                    B[z + 4][x] = a;
                    B[z + 4][x + 1] = b;
                    B[z + 4][x + 2] = c;
                    B[z + 4][x + 3] = d;
                }
                /*4 * 4 A[3] to B[2] each time 2 * 4*/
                /*4 * 4 A[4] to B[4] each time 2 * 4*/
                for(z = x + 4; z < x + 8; z+=2)
                {
                    a = A[z][y];
                    b = A[z][y + 1];
                    c = A[z][y + 2];
                    d = A[z][y + 3];
                    e = A[z + 1][y];
                    f = A[z + 1][y + 1];
                    g = A[z + 1][y + 2];
                    h = A[z + 1][y + 3];
                    B[y][z] = a;
                    B[y + 1][z] = b;
                    B[y + 2][z] = c;
                    B[y + 3][z] = d;
                    B[y][z + 1] = e;
                    B[y + 1][z + 1] = f;
                    B[y + 2][z + 1] = g;
                    B[y + 3][z + 1] = h;
                    
                    a = A[z][y + 4];
                    b = A[z][y + 5];
                    c = A[z][y + 6];
                    d = A[z][y + 7];
                    e = A[z + 1][y + 4];
                    f = A[z + 1][y + 5];
                    g = A[z + 1][y + 6];
                    h = A[z + 1][y + 7];
                    B[y + 4][z] = a;
                    B[y + 5][z] = b;
                    B[y + 6][z] = c;
                    B[y + 7][z] = d;
                    B[y + 4][z + 1] = e;
                    B[y + 5][z + 1] = f;
                    B[y + 6][z + 1] = g;
                    B[y + 7][z + 1] = h;
                }
            }
            else
            {
                /*4 * 8 move to B[1] B[2] wrong place but low miss*/
                for (z = x; z < x + 4; z++)
                {
                    a = A[z][y];
                    b = A[z][y + 1];
                    c = A[z][y + 2];
                    d = A[z][y + 3];
                    e = A[z][y + 4];
                    f = A[z][y + 5];
                    g = A[z][y + 6];
                    h = A[z][y + 7];
                    B[y][z] = a;
                    B[y + 1][z] = b;
                    B[y + 2][z] = c;
                    B[y + 3][z] = d;
                    B[y][z + 4] = e;
                    B[y + 1][z + 4] = f;
                    B[y + 2][z + 4] = g;
                    B[y + 3][z + 4] = h;
                }
                /*no conflict so B[2] to B[3] and A[3] to B[2]*/
                for (z = y; z < y + 4; z++)
                {
                    a = B[z][x + 4];
                    b = B[z][x + 5];
                    c = B[z][x + 6];
                    d = B[z][x + 7];
                    B[z][x + 4] = A[x + 4][z];
                    B[z][x + 5] = A[x + 5][z];
                    B[z][x + 6] = A[x + 6][z];
                    B[z][x + 7] = A[x + 7][z];
                    B[z + 4][x] = a;
                    B[z + 4][x + 1] = b;
                    B[z + 4][x + 2] = c;
                    B[z + 4][x + 3] = d;
                }
                /*A[4] to B[4]*/
                for (z = x + 4; z < x + 8; z += 2)
                {
                    a = A[z][y + 4];
                    b = A[z][y + 5];
                    c = A[z][y + 6];
                    d = A[z][y + 7];
                    e = A[z + 1][y + 4];
                    f = A[z + 1][y + 5];
                    g = A[z + 1][y + 6];
                    h = A[z + 1][y + 7];
                    B[y + 4][z] = a;
                    B[y + 5][z] = b;
                    B[y + 6][z] = c;
                    B[y + 7][z] = d;
                    B[y + 4][z + 1] = e;
                    B[y + 5][z + 1] = f;
                    B[y + 6][z + 1] = g;
                    B[y + 7][z + 1] = h;
                }
            }
            
        }
    }
    else if(M == 61 && N == 67)
    {
        int j;
    	for(x = 0; x < N - 16; x+= 16)
    	for(y = 0; y < M - 16; y+= 16)
    	{
    	    /*16 * 16 until no block*/
    	    for(z = x; z < x + 16; z++)
    	    {
                a = A[z][y];
    	    	b = A[z][y + 1];
                c = A[z][y + 2];
                d = A[z][y + 3];
                e = A[z][y + 4];
                f = A[z][y + 5];
                g = A[z][y + 6];
                h = A[z][y + 7];
                B[y][z] = a;
                B[y + 1][z] = b;
                B[y + 2][z] = c;
                B[y + 3][z] = d;
                B[y + 4][z] = e;
                B[y + 5][z] = f;
                B[y + 6][z] = g;
                B[y + 7][z] = h;

                a = A[z][y + 8];
                b = A[z][y + 9];
                c = A[z][y + 10];
                d = A[z][y + 11];
                e = A[z][y + 12];
                f = A[z][y + 13];
                g = A[z][y + 14];
                h = A[z][y + 15];
                B[y + 8][z] = a;
                B[y + 9][z] = b;
                B[y + 10][z] = c;
                B[y + 11][z] = d;
                B[y + 12][z] = e;
                B[y + 13][z] = f;
                B[y + 14][z] = g;
                B[y + 15][z] = h;
    	    }
    	}
    	/*resolve others*/
        for(z = 0; z < x; z += 1)
        {
            for(j = y; j < M; j++)
            {
                B[j][z] = A[z][j];
            }
        }
        for(z = x; z < N; z++)
        {
            for(j = 0; j < M; j++)
            {
                B[j][z] = A[z][j];
            }
        }
    }
}
/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 
    
    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


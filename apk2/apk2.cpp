#include <time.h>
#include <iostream>
#define size 1000
#define count 10000
void randMatrix(int** matrix);
void printMatrix(int** matrix);
void initMatrix(int*** matrix);
void algorithm(int** matrix, int* sumArray);
void optimization(int** matrix, int* sumArray);
void withoutOptimization(int** matrix, int* sumArray);
int main()
{
    int** matrix = NULL;
    int sumArray[size];
    initMatrix(&matrix);
    randMatrix(matrix);
    int start = clock();
    for (int c = 0; c < count; c++) {
        optimization(matrix, sumArray);
    }
    int c_end = clock();
    printf("C with optimization - %lf\n", (c_end - start) / 1000.0);
    int startWithoutOpt = clock();
    for (int c = 0; c < count; c++) {
        withoutOptimization(matrix, sumArray);
    }
    int CWithoutOptEnd = clock();
    printf("C without optimization - %lf\n", (CWithoutOptEnd - startWithoutOpt) / 1000.0);
    for (int c = 0; c < count; c++) {
        _asm
        {
            mov esi, matrix
            xor edi, edi
            mov eax, 0
            mov ebx, 0
            loop1:
            cmp eax, size
                je loop1Exit
                mov sumArray[edi], 0
                loop3 :
                cmp ebx, size
                je loop3Exit
                mov edx, [esi + eax * 4]
                mov edx, [edx + ebx * 4]

                add sumArray[edi], edx
                add ebx, 1
                jmp loop3

                loop3Exit :
            add edi, 4
                add eax, 1
                mov ebx, 0
                jmp loop1

                loop1Exit :
        }
    }
    int asm_end = clock();
    printf("ASM-%lf\n", (asm_end - CWithoutOptEnd) / 1000.0);
    for (int c = 0; c < count; c++) {
        _asm
        {
            mov esi, matrix
            xor edi, edi
            mov eax, 0
            mov ebx, 0
            loop1MMX:
            cmp eax, size
                je loop1MMXExit
                mov sumArray[edi], 0
                loop3MMX :
                cmp ebx, size
                je loop3MMXExit
                mov edx, [esi + eax * 4]
                movq mm0, [edx + ebx * 4]

                movq mm1, mm0
                psrlq mm0, 32
                PADDD mm1, mm0
                movd edx, mm1
                add sumArray[edi], edx

                add ebx, 2
                jmp loop3MMX

                loop3MMXExit :
            add edi, 4
                add eax, 1
                mov ebx, 0
                jmp loop1MMX

                loop1MMXExit :
            emms
        }
    }
    int mmx_end = clock();
    printf("MMX-%lf\n", (mmx_end - asm_end) / 1000.0);
    return 0;

}
void optimization(int** matrix, int* sumArray)
{
    algorithm(matrix, sumArray);
}
#pragma optimize( "", off )
void withoutOptimization(int** matrix, int* sumArray)
{
    algorithm(matrix, sumArray);
}
#pragma optimize( "", on )
void algorithm(int** matrix, int* sumArray)
{
    for (int i = 0; i < size; i++) {
        sumArray[i] = 0;
        for (int j = 0; j < size; j++) {
            sumArray[i] += matrix[i][j];
        }
    }
}
void randMatrix(int** matrix)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int** matrix)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}
void initMatrix(int*** matrix)
{
    *matrix = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        (*matrix)[i] = (int*)malloc(size * sizeof(int));
    }
}

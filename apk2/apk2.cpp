#include <iostream>
#include <time.h>
#define size 1500
void randMatrix(int **matrix);
void printMatrix(int **matrix);
void initMatrix(int*** matrix);
int main()
{   
        srand(time(0));
        int* buffer;
        int** matrix1=NULL , **matrix2=NULL;
        initMatrix(&matrix1);
        initMatrix(&matrix2);
        randMatrix(matrix1);
        randMatrix(matrix2);
        int sum = 0;
        int mult = 0;
        int **matrix3=NULL;
        initMatrix(&matrix3);
        unsigned int start_time = clock();
        for (int i = 0; i < size; i++) {
            for (int k = 0; k < size; k++) {
                sum = 0;
                for (int j = 0; j < size; j++) {
                    sum += matrix1[i][j] * matrix2[j][k];
                }
                matrix3[i][k] = sum;
            }
        }
        printf("C - %lf\n",(clock()-start_time)/1000.0);
        unsigned int start_time_assembler = clock();
        _asm
        {
            mov esi, matrix1
            mov edi, matrix2
            mov eax, 0
            mov ecx, 0
            mov ebx, 0
            loop1:
            cmp eax,size
            je loop1Exit
            mov buffer,eax
                loop2:
                    cmp ecx, size
                    je loop2Exit
                    mov sum, 0
                        loop3 :
                        cmp ebx, size
                        je loop3Exit
                        mov edx, [esi + eax * 4]
                        mov edx, [edx + ebx * 4]
                        mov eax,edx
                        mov edx, [edi + ebx * 4]
                        mov edx, [edx + ecx * 4]
                        mul edx
                        add sum,eax
                        add ebx,1
                        mov eax,buffer
                        jmp loop3

                    loop3Exit :
                        mov esi, matrix3
                        mov esi, [esi + eax * 4]
                        mov edx,sum
                        mov [esi + ecx * 4],edx
                        mov esi, matrix1

                    add ecx,1
                    mov ebx,0
                    jmp loop2
                loop2Exit:
                    add eax,1
                    mov ecx,0
                    jmp loop1
                loop1Exit :
        }
        printf("Assembler - %lf\n", (clock() - start_time_assembler) / 1000.0);
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++) {
                int buffer = matrix2[i][j];
                matrix2[i][j] = matrix2[j][i];
                matrix2[j][i] = buffer;
            }
        }
        time_t start_mmx = clock();
        _asm
        {
            mov eax, 0
            mov ecx, 0
            mov ebx, 0
            mov edi, matrix2
            mov esi,matrix1
            loop1MMX:
            cmp eax, size
            je finish
            loop2MMX:
            cmp ebx,size
            je loop2Finish 
            mov sum, 0
            movq mm3, [sum]
            xor edx,edx
            loop4:
            cmp ecx,size
            je loop4End
            mov edx, [esi + eax * 4]
            movq mm0,[edx+ecx*4]
            mov edx, [edi + ebx * 4]
            movq mm1,[edx+ecx*4]
            PMULLW mm0, mm1
            movq mm3, mm0
            psrlq mm0, 32
            PADDD mm0, mm3
            movd edx,mm0
            add sum, edx
            add ecx,2
            jmp loop4
            loop4End:
            push edi
            mov edi,matrix3
            mov edi,matrix3
            mov edi, [edi + eax * 4]
            mov edx, sum
            mov[edi + ebx * 4], edx
            add ebx, 1
            mov ecx, 0
            pop edi
            jmp loop2MMX 
            loop2Finish :
            add eax, 1
            mov ebx, 0
            jmp loop1MMX
            finish:
            emms
        }
        printf("MMX - %lf\n",(clock()-start_mmx)/1000.0);
        return 0;

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

#include "stdio.h"
#include "windows.h"
#include "time.h"
#include <stdlib.h>

void rand_arr(int *arr, int size)//���������� ����������� ������� ���������� �������
{
	for (int i = 0; i < size; i++)
		arr[i] = rand() % 10;
}

void main(void)
{
	system("chcp 1251");
	system("cls");

	srand(time(NULL));//������� ��� ���������� ������������� ��������� �����

	int arr_one[4][4];//������ �������
	int arr_two[4][4];//������ �������
	int result_arr[4][4];//�������������� �������

	time_t start, end;//���������� ��� ������� �������
	int times = 1000000;//���������� ��������

	int vector[4];//������ �� ������� ����� �������� �������

	for (int i = 0; i < 4; i++)//��������� ������ ������� ���������� �������
		rand_arr(arr_one[i], 4);

	for (int i = 0; i < 4; i++)//��������� ������ ������� ���������� �������
		rand_arr(arr_two[i], 4);

	rand_arr(vector, 4);//��������� ������ ���������� �������

	start = clock();
	for (int k = 0; k < times;k++)//���������� ��� ������ ������ MMX
	{
		_asm
		{
			mov ecx, 8	//�������� � ������� ���������� ��������
			xor esi, esi // �������� ��������
			xor ebx,ebx

			loop3:
			movq MM0, vector[ebx] //���������� ������ 64���� ������� � �������
			movq MM2, arr_two[esi]//���������� ������ 64 ���� ������� � �������
			pmullw MM2, MM0 //�������� ���������� �������� MM2 �� ���������� �������� MM0

			movq MM3, arr_one[esi]//���������� ������ 64���� ������� � �������

			paddd  MM3, MM2 //��������� ���������� �������� MM2 � ����������� �������� MM3
			movq result_arr[esi], MM3 //�������� ���������� �������� � �������������� �������

			add esi, 8 //���������� �� 8 ����
			
			cmp ebx,8 //���� ������� ��� ����� 8,�� �������� ���
			je xor_ebx

			add ebx,8 // ���� ������� �� ����� 8,�� ����������� ��� �� 8 ����

			loop loop3//���� ������������ ���� ecx �� ����� ����
			
			jmp _end //����� ������������ � �����

			xor_ebx://����� ��� ���������� ebx
			xor ebx,ebx
			dec ecx

			cmp ecx, 0//���� ecx ��� ����� ���� ��� ������ ����������� � ����
			je _end

			jmp loop3//���� ecx ��� �� ����� ���� ������������ � ����
			
			_end:
			emms //��������� � ���������� ������� ������ � ��������� �������
		}
	}

	end = clock();

	for (int i = 0; i < 4; i++)//������� �������
	{
		putchar('\n');
		for (int j = 0; j < 4; j++)
			printf("%d ", result_arr[i][j]);
	}

	printf("\n\n����� ���������� �� ���������� � ��������� MMX = %f s\n", (double)(end - start) / CLOCKS_PER_SEC);

	start = clock();

	for (int k = 0; k < times;k++)
	{
		_asm
		{
			xor     esi, esi// �������� ��������
			xor     ebx, ebx

			mov di,4 //���������� �������� ��� ������ ������

			loop1 :
				xor si, si// �������� ��������
				xor ax, ax

				mov ecx, 4 //���������� �������� ��� ������ ������
					
				loop2 :
				mov ax, arr_two[ebx]//���������� ������� �������
				imul ax, vector[esi]//�������� ���� ������� �� �������� � �������

				add ax, arr_one[ebx]//��������� ������� ������ �������
				mov result_arr[ebx], ax//��������� ��������� ���������� ��� ������ �������� � �������������� �������

				add esi, 4 //���������� �� ����
				add ebx, 4 //���������� �� ����
				loop loop2

				dec di 
				cmp di,0//���� di �� ����� ���� ��������� �� ��������� ������ �������
				ja loop1 
		}
	}

	end = clock();

	for (int i = 0; i < 4; i++)//������� �������
	{
		putchar('\n');
		for (int j = 0; j < 4; j++)
			printf("%d ", result_arr[i][j]);
	}

	printf("\n\n����� ���������� �� ���������� ��� ������ MMX = %f s\n", (double)(end - start) / CLOCKS_PER_SEC);
	
	start = clock();

	for (int k = 0; k<times;k++)// ���������� �� ��
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				result_arr[i][j] = arr_one[i][j] + (arr_two[i][j] * vector[j]);
	
	end = clock();

	for (int i = 0; i < 4; i++)//������� �������
	{
		putchar('\n');
		for (int j = 0; j < 4; j++)
		printf("%d ", result_arr[i][j]);
	}
		
	
	printf("\n\n����� ���������� �� �� = %f s\n", (double)(end - start) / CLOCKS_PER_SEC);
	system("pause");
}

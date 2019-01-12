#include "stdio.h"
#include "windows.h"
#include "time.h"
#include <stdlib.h>

void rand_arr(int *arr, int size)//заполнения одномерного массива случайными числами
{
	for (int i = 0; i < size; i++)
		arr[i] = rand() % 10;
}

void main(void)
{
	system("chcp 1251");
	system("cls");

	srand(time(NULL));//функция для вычисления действительно случайных чисел

	int arr_one[4][4];//первая матрица
	int arr_two[4][4];//вторая матрица
	int result_arr[4][4];//результирующая матрица

	time_t start, end;//переменные для расчета времени
	int times = 1000000;//количество итераций

	int vector[4];//вектор на который будем умножать матрицу

	for (int i = 0; i < 4; i++)//заполняем первую матрицу случайными числами
		rand_arr(arr_one[i], 4);

	for (int i = 0; i < 4; i++)//заполняем вторую матрицу случайными числами
		rand_arr(arr_two[i], 4);

	rand_arr(vector, 4);//заполняем вектор случайными числами

	start = clock();
	for (int k = 0; k < times;k++)//вычисление при помощи команд MMX
	{
		_asm
		{
			mov ecx, 8	//помещаем в регистр количество итераций
			xor esi, esi // обнуляем регистры
			xor ebx,ebx

			loop3:
			movq MM0, vector[ebx] //записываем первые 64бита вектора в регистр
			movq MM2, arr_two[esi]//записываем первые 64 бита матрицы в регистр
			pmullw MM2, MM0 //умножаем содержимое регистра MM2 на содержимое регистра MM0

			movq MM3, arr_one[esi]//записываем первые 64бита матрицы в регистр

			paddd  MM3, MM2 //добавляем содержимое регистра MM2 к содержимому регистра MM3
			movq result_arr[esi], MM3 //помещаем содержимое регистра в результирующую матрицу

			add esi, 8 //сдвигаемся на 8 байт
			
			cmp ebx,8 //если регистр уже равен 8,то обнуляем его
			je xor_ebx

			add ebx,8 // если регистр не равен 8,то увеличиваем его на 8 байт

			loop loop3//цикл продолжается пока ecx не равен нулю
			
			jmp _end //сразу перемещаемся в конец

			xor_ebx://метка где обнуляется ebx
			xor ebx,ebx
			dec ecx

			cmp ecx, 0//если ecx уже равен нулю нет смысла возвращатся в цикл
			je _end

			jmp loop3//если ecx еще не равен нулю возвращаемся в цикл
			
			_end:
			emms //переходим к исполнению обычных команд с плавающей запятой
		}
	}

	end = clock();

	for (int i = 0; i < 4; i++)//выводим матрицу
	{
		putchar('\n');
		for (int j = 0; j < 4; j++)
			printf("%d ", result_arr[i][j]);
	}

	printf("\n\nВремя вычисления на ассемблере с командами MMX = %f s\n", (double)(end - start) / CLOCKS_PER_SEC);

	start = clock();

	for (int k = 0; k < times;k++)
	{
		_asm
		{
			xor     esi, esi// обнуляем регистры
			xor     ebx, ebx

			mov di,4 //количество итераций для каждой строки

			loop1 :
				xor si, si// обнуляем регистры
				xor ax, ax

				mov ecx, 4 //количество итераций для каждой строки
					
				loop2 :
				mov ax, arr_two[ebx]//записываем элемент матрицы
				imul ax, vector[esi]//умножаем этот элемент на значение в векторе

				add ax, arr_one[ebx]//добавляем элемент другой матрицы
				mov result_arr[ebx], ax//добавляем результат вычисления для одного элемента в результирующую матрицу

				add esi, 4 //сдвигаемся на байт
				add ebx, 4 //сдвигаемся на байт
				loop loop2

				dec di 
				cmp di,0//если di не равно нудю переходим на следующую строку матрицы
				ja loop1 
		}
	}

	end = clock();

	for (int i = 0; i < 4; i++)//выводим матрицу
	{
		putchar('\n');
		for (int j = 0; j < 4; j++)
			printf("%d ", result_arr[i][j]);
	}

	printf("\n\nВремя вычисления на ассемблере без команд MMX = %f s\n", (double)(end - start) / CLOCKS_PER_SEC);
	
	start = clock();

	for (int k = 0; k<times;k++)// вычисления на СИ
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				result_arr[i][j] = arr_one[i][j] + (arr_two[i][j] * vector[j]);
	
	end = clock();

	for (int i = 0; i < 4; i++)//выводим матрицу
	{
		putchar('\n');
		for (int j = 0; j < 4; j++)
		printf("%d ", result_arr[i][j]);
	}
		
	
	printf("\n\nВремя вычисления на СИ = %f s\n", (double)(end - start) / CLOCKS_PER_SEC);
	system("pause");
}

//f = sin(x) * tan(x + 5);

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

double input_value(const char *str)//функция для ввода значений
{
	rewind(stdin);
	double value = 0;

	printf("%s", str);
	while (scanf_s("%lf", &value) != 1 || value<0)//проверка ввода
	{
		rewind(stdin);
		printf("Некоректное значение. Повторите ввод:");
	}
	return value;
}

bool exit()//выход из программы или продолжение работы
{
	short int value = 0;
	printf("\nДля повторной работы введите '1'. Для выхода введите '0':");
	while (scanf_s("%hd", &value) != 1 || (value!=0 && value!=1))
	{
		rewind(stdin);
		printf("Некоректное значение. Повторите ввод:");
	}
	return value;
}


int main()
{
	system("chcp 1251");
	bool flag = 0;

	do
	{
		system("cls");

		long double a = 0;//левая граница значений
		long double b = 0;//правая граница значений
		long double d = 0;//шаг

		a = input_value("Введите а:");
		b = input_value("Введите b:");
		while ((d = input_value("Введите d:")) == 0)
		{
			puts("Шаг не может быть равен нулю");
		}

		long double x = a;//значение
		long double f = 0;//результат функции F(x)
		long double time = 0;//время выполнения расчета

		LARGE_INTEGER timerFrequency;//частота счетчика
		LARGE_INTEGER timerStart;//начало времени выполнения расчета
		LARGE_INTEGER timerStop;//конец времени выполнения расчета

		QueryPerformanceCounter(&timerStart);//начало измерения такта(импульса)
		QueryPerformanceFrequency(&timerFrequency);//частота такта

		long double five = 5;

		_asm finit//начало ассемблерной вставки сопроцессора
		while (x <= b)//цикл выполнения расчета
		{
			_asm//начало ассемблерной вставки
			{
				fld x//помещение элемента в стек
				fsin//вычесление синуса элемента в стеке

				fld x//помещение элемента в стек
				fadd five//добавление 5 к элементу в стеке
				fptan//вычесление тангенса элемента в стеке
				fdiv//делем два значения из стека, полученных в прошлой операции

				fmul//умножение синуса элемента в стеке на тангенс элемента в стеке 

				fstp f//удаление элемента из стека и занесение его в f

				fld x//помещение элемента в стек
				fadd d//наращиваем шаг d
				fstp x // удаление элемента из стека и занесение его в x
			}
		}
		_asm fwait//конец ассемблерной вставки сопроцессора

		QueryPerformanceCounter(&timerStop);//конец замера такта(импульса)
		
		time = ((double)timerStop.QuadPart - timerStart.QuadPart) / (double)timerFrequency.QuadPart;
		printf("Время на Assembler %lf в секундах.\n", time);
		printf("sin(x) * tan(x + 5)=%lf\n", f);

		x = a;

		QueryPerformanceFrequency(&timerFrequency);
		QueryPerformanceCounter(&timerStart);

		while (x <= b)//вычесление F(x) с помощью math.h
		{
			f = sin(x) * tan(x + 5);
			x += d;
		}
		QueryPerformanceCounter(&timerStop);
		time = (double)(timerStop.QuadPart - timerStart.QuadPart) / (double)timerFrequency.QuadPart;
		printf("Время на С %lf в секундах.\n", time);
		printf("sin(x) * tan(x + 5)=%lf\n", f);

		flag = exit();//выход или продолжение программы
	
	} while (flag == true);

	return 0;
}
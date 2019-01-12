//f = sin(x) * tan(x + 5);

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

double input_value(const char *str)//������� ��� ����� ��������
{
	rewind(stdin);
	double value = 0;

	printf("%s", str);
	while (scanf_s("%lf", &value) != 1 || value<0)//�������� �����
	{
		rewind(stdin);
		printf("����������� ��������. ��������� ����:");
	}
	return value;
}

bool exit()//����� �� ��������� ��� ����������� ������
{
	short int value = 0;
	printf("\n��� ��������� ������ ������� '1'. ��� ������ ������� '0':");
	while (scanf_s("%hd", &value) != 1 || (value!=0 && value!=1))
	{
		rewind(stdin);
		printf("����������� ��������. ��������� ����:");
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

		long double a = 0;//����� ������� ��������
		long double b = 0;//������ ������� ��������
		long double d = 0;//���

		a = input_value("������� �:");
		b = input_value("������� b:");
		while ((d = input_value("������� d:")) == 0)
		{
			puts("��� �� ����� ���� ����� ����");
		}

		long double x = a;//��������
		long double f = 0;//��������� ������� F(x)
		long double time = 0;//����� ���������� �������

		LARGE_INTEGER timerFrequency;//������� ��������
		LARGE_INTEGER timerStart;//������ ������� ���������� �������
		LARGE_INTEGER timerStop;//����� ������� ���������� �������

		QueryPerformanceCounter(&timerStart);//������ ��������� �����(��������)
		QueryPerformanceFrequency(&timerFrequency);//������� �����

		long double five = 5;

		_asm finit//������ ������������ ������� ������������
		while (x <= b)//���� ���������� �������
		{
			_asm//������ ������������ �������
			{
				fld x//��������� �������� � ����
				fsin//���������� ������ �������� � �����

				fld x//��������� �������� � ����
				fadd five//���������� 5 � �������� � �����
				fptan//���������� �������� �������� � �����
				fdiv//����� ��� �������� �� �����, ���������� � ������� ��������

				fmul//��������� ������ �������� � ����� �� ������� �������� � ����� 

				fstp f//�������� �������� �� ����� � ��������� ��� � f

				fld x//��������� �������� � ����
				fadd d//���������� ��� d
				fstp x // �������� �������� �� ����� � ��������� ��� � x
			}
		}
		_asm fwait//����� ������������ ������� ������������

		QueryPerformanceCounter(&timerStop);//����� ������ �����(��������)
		
		time = ((double)timerStop.QuadPart - timerStart.QuadPart) / (double)timerFrequency.QuadPart;
		printf("����� �� Assembler %lf � ��������.\n", time);
		printf("sin(x) * tan(x + 5)=%lf\n", f);

		x = a;

		QueryPerformanceFrequency(&timerFrequency);
		QueryPerformanceCounter(&timerStart);

		while (x <= b)//���������� F(x) � ������� math.h
		{
			f = sin(x) * tan(x + 5);
			x += d;
		}
		QueryPerformanceCounter(&timerStop);
		time = (double)(timerStop.QuadPart - timerStart.QuadPart) / (double)timerFrequency.QuadPart;
		printf("����� �� � %lf � ��������.\n", time);
		printf("sin(x) * tan(x + 5)=%lf\n", f);

		flag = exit();//����� ��� ����������� ���������
	
	} while (flag == true);

	return 0;
}
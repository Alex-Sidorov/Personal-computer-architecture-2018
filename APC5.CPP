#include <stdio.h>
#include <conio.h>
#include <dos.h>

int counter = 0;

void interrupt far (*int70)(...);//Новый обработчик для задержки
void interrupt far (*int4A)(...);//новый обработчик для будильника 

void set_time(void);//установка времени
void get_time(void);//считывание времени
void set_alarm(void);//установка будильника
void delay(void);//задержка
void wait(int);//ожидание задержки
void input_time(struct TIME &time);//ввод времени

struct VIDEO//структура для видеобуфера
{
        unsigned char symb;
        unsigned char attr;
};

struct TIME//структура для времени
{
	char hour;
	char min;
	char sec;
};

void interrupt newInt70(...)
{
    counter++;
    outp(0x70, 0x0C);//Если регистр 0x0C не будет прочитан после IRQ 8(прерывание RCT), топрерывани не будет
    inp(0x71);

    outp(0x20, 0x20);//конец прерывания master
    outp(0xA0, 0x20);//конец прерывания slave
}

void interrupt newInt4A(...)
{
    VIDEO far* screen = (VIDEO far *)MK_FP(0xB800, 0);

    char temp[]="ALARM";
 
	int i;

        for(i = 0; i < 5; i++)		
        {
                screen->symb = temp[i];
                screen->attr = 0x6E;
                screen++;
        }
    int4A();
}

unsigned bcd1bin(char *bcd)//перевод из BCD в бинарную с\с
{
  return( ((*bcd) & 0x0f) +
    10 * (((*bcd) & 0xf0) >> 4) );
}

void bin1bcd(int bin, char *bcd) //перевод из бинарной с\с в BCD
{
  int i;
  i = bin / 10;
  *bcd = (i << 4) + (bin - (i * 10));
}

void input_time(struct TIME &time)//ввод времени
{
	int value;
	
	fflush(stdin);
	printf("\nhour:");
	while(scanf("%d",&value)!=1 || value<0 ||value>24)
	{
		fflush(stdin);
	}
	bin1bcd(value,&time.hour);

	fflush(stdin);
	printf("\nmin:");
	while(scanf("%d",&value)!=1 || value<0 ||value>60)
	{
		fflush(stdin);
	}
	bin1bcd(value,&time.min);

	fflush(stdin);
	printf("\nsec:");
	while(scanf("%d",&value)!=1 || value<0 ||value>60)
	{
		fflush(stdin);
	}
	bin1bcd(value,&time.sec);

}

void set_time(void)
{
	struct TIME time;
	unsigned char temp;

	input_time(time);
	
	do
	{
		outp(0x70, 0x0A);
	} while( inp(0x71) & 0x80 ); //ожидаем доступа к часам

	outp(0x70, 0x0B);//
	temp=inp(0x71);//
	temp|=0x80;//	отключаем внутренний цикл обновления часов
	outp(0x70, 0x0B);//
	outp(0x71, temp);//

	outp(0x70,0x04);//устанавливаем час
	outp(0x71,time.hour);

	outp(0x70,0x02);//устанавливаем минуты
	outp(0x71,time.min);

	outp(0x70,0x00);//устанавливаем секунды
	outp(0x71,time.sec);

	outp(0x70, 0x0B);//
	temp=inp(0x71);//
	temp-=0x80;//		разрешаем цикл обновления часов
	outp(0x70, 0x0B);//
	outp(0x71, temp);//
}

void get_time()
{
	struct TIME tm;

	outp(0x70,0x00);//считываем секунды
	tm.sec=inp(0x71);//

	outp(0x70,0x02);//
	tm.min=inp(0x71);//считываем минуты

	outp(0x70,0x04);//
	tm.hour=inp(0x71);//считываем час

	printf("\n%02.2d:%02.2d:%02.2d" "\n\n",   bcd1bin(&(tm.hour)),
													bcd1bin(&(tm.min)),
													bcd1bin(&(tm.sec)));
}

void set_alarm()
{
	struct TIME time;
	
	input_time(time);
	
	outp(0x70,0x01);
    outp(0x71, time.sec);
    outp(0x70, 0x03);
    outp(0x71, time.min);
    outp(0x70, 0x05);
    outp(0x71, time.hour);

    outp(0xA1, inp(0xA1) & 0xFE);// размаскируем линии запроса
    outp(0x70, 0x0B);
    outp(0x71, inp(0x71) | 0x20);//разрешаем прерывание от будильника
}

void delay()
{
	disable();	
	int70 = getvect(0x70);//устанавливаем новый обработчик
    setvect(0x70, newInt70);
	enable();
	
	int time =0;
    
    printf("\n\nEnter delay time[0-10000]: ");
	fflush(stdin);
	while(scanf("%d",&time)!=1||time<0||time>10000)
	{
		fflush(stdin);
	}

    outp(0xA1, inp(0xA1) & 0xFE);  // размаскируем линии запроса

    outp(0x70, 0x0B);
    outp(0x71, inp(0x71) | 0x40); // включаем периодическое прерывание

    wait(time);//ожидаем
    setvect(0x70, int70);//восстанавливаем старый обработчик

    outp(0x70, 0x0B);
    outp(0x71, inp(0x71) & 0xBF);   // выключаем периодическое прерывание

    get_time();
    fflush(stdin);
    getch();
}

void wait(int time)
{
    printf("Start of delay:\n");
    get_time();
	printf("\nTime after %d ms of delay:\n", time);
    counter = 0;
    while(counter != time);//ждем окончания задержки
}

int main(void)
{
	disable();
    int4A = getvect(0x4A);//устанавливаем новый обработчик
    setvect(0x4A, newInt4A);
	enable();
	
	int menu=0;
	while(1)
	{
		clrscr();
		printf("1-set time\n2-get time\n3-set alarm\n4-delay\n5-exit\ninput:");
		while(scanf("%d",&menu)!=1||menu>5||menu<1)
		{
			fflush(stdin);
		}

		switch(menu)
		{
			case 1:set_time(); break;
			case 2:get_time(); fflush(stdin); getch();break;
			case 3:set_alarm(); break;
			case 4:delay(); break;
			case 5:
			setvect(0x4A,int4A);//устанавливаем старый обработчик
			outp(0x70, 0x0B);
			outp(0x71, inp(0x71) & 0xDF);//запрещаем прерывание от будильника
			return 0;
		}

	}

}
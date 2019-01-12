#include <stdio.h>
#include <conio.h>
#include <dos.h>

void play_sound()
{
  const int COUNT_FREQUENCY = 9;//количество частот
  const long unsigned BASE = 1193180;//частота базового импульса

  int frequency[COUNT_FREQUENCY] = {392,329,329,392,329,329,392,349,329};//частота ноты
  int longevity[COUNT_FREQUENCY] = {400,400,400,400,400,400,400,400,400};//длительность проигрывания ноты
  
  int kd=0;//переменная для хранения частот

  for(int i=0; i <COUNT_FREQUENCY; i++) //проигрываем все ноты
  {
      outp(0x43,0xB6);//задаем режим работы канала
      kd = BASE/frequency[i];
      outp(0x42,kd%256); //устанавливаем младшие биты
      kd /= 256;
      outp(0x42,kd); //устанавливаем старшие биты

      outp(0x61, inp(0x61)|3);    //включаем громкоговоритель
      delay(longevity[i]);        //играем ноту определенное время 
      outp(0x61, inp(0x61)&0xFC); //выключаем громкоговоритель
    }
}

void division_coefficient()
{
clrscr();

int channel;
int ports[] = {0x40, 0x41, 0x42};
int controlWords[] = {0, 64, 128};//переменные для канала
unsigned kdLow, kdHigh, kd, max;

printf("Division coefficients:\n");
unsigned long j;
for(channel = 0; channel < 3; channel++)
{
  kd = 0;
  max = 0;
  if(channel == 2) outp(0x61, inp(0x61)|3);  //включаем громкоговоритель

  for(j = 0; j < 65535; j++)
  {
    outp(0x43, controlWords[channel]); //записываем значение канала
    kdLow  = inp(ports[channel]);//читаем младшие биты
    kdHigh = inp(ports[channel]);//читаем старшие биты
    kd = kdHigh*256 + kdLow;//объединяем

    if(kd > max) max = kd;//проверяем
  }

  if(channel == 2) outp(0x61, inp(0x61)&0xFC); //выключаем громкоговоритель
  printf("Channel %d - %4X\n", channel, max);
}
  getch();
}

void print_status_word()
{
  clrscr();
  unsigned char state, stateWord[9];
  int channel, i;
  int ports[] = {0x40, 0x41, 0x42};
  
  int controlWords[] = {226, 228, 232};//регистры для каждого канала //226-11 1 0 0 0 1 0
																	 //228-11 1 0 0 1 0 0
																	 //232-11 1 0 1 0 0 0
  
  for(channel = 0; channel < 3; channel++)
  {
    outp(0x43, controlWords[channel]); //выбор канала
    state = inp(ports[channel]);//чтение байта из канала

    for(i = 7; i >= 0; i --)//заполнения слова
    {
      stateWord[i] = state%2 + '0';
      state/= 2;
    }
    stateWord[8] = '\0';
    printf("Channel %d: %s \n", channel, stateWord);
  }
  fflush(stdin);
  getch();
}


void print_random_number()
{
  int menu=0;
  unsigned int limit, numberLow, numberHigh, number;

  clrscr();
  do
  {
    printf("1 - set limit\n2 - get a number\n0 - exit\n");
    fflush(stdin);
    scanf("%d", &menu);

    switch(menu){
    case 1:
      do
      {
	printf("Enter a limit [1;65535]\n");
	fflush(stdin);
      }while(! scanf("%d", &limit));

      outp(0x43,0xB4);//выбираем режим работы
      outp(0x42,limit%256);//устанавливаем младшие биты
      limit /= 256;
      outp(0x42,limit);//устанавливаем старшие биты
      outp(0x61, inp(0x61)|1);    //включаем канал без динамика

      break;

    case 2:
      printf("Random number:\n");

      outp(0x43, 128); //выбираем второй канал
      numberLow = inp(0x42);//читаем младшие биты
      numberHigh = inp(0x42);//читаем старшие биты
      number = numberHigh*256 + numberLow;//формируем число

      printf("Number %u\n", number);

      break;
    }
  }while(menu != 0);

  outp(0x61, inp(0x61)&0xFC); //выключаем канал
}

int main()
{
int menu=0;

do
{
clrscr();
printf("1 - Play sound\n2 - Print status Word\n3 - Division coefficient\n4 - Print random number\n0 - Exit\n");
fflush(stdin);
scanf("%d", &menu);

  switch(menu){
  case 1: play_sound(); break;
  case 2: print_status_word(); break;
  case 3: division_coefficient(); break;
  case 4: print_random_number(); break;
  }

}while(menu != 0);
return 0;
}
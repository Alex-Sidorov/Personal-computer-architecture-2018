#include <dos.h>
#include <stdio.h>

struct VIDEO
{
        unsigned char symb;
        unsigned char attr;
};
void get_reg();
unsigned char attr=0x01;

void interrupt (*old08)(...);//описываем старые обработчики
void interrupt (*old09)(...);
void interrupt (*old0A)(...);
void interrupt (*old0B)(...);
void interrupt (*old0C)(...);
void interrupt (*old0D)(...);
void interrupt (*old0E)(...);
void interrupt (*old0F)(...);

void interrupt (*old60)(...);
void interrupt (*old61)(...);
void interrupt (*old62)(...);
void interrupt (*old63)(...);
void interrupt (*old64)(...);
void interrupt (*old65)(...);
void interrupt (*old66)(...);
void interrupt (*old67)(...);

void interrupt new08(...){get_reg();old08();}//описываем новые обработчики 
void interrupt new09(...){get_reg();attr += 0x01;old09();}//здесь обрабатываем ввод с клавиатуры, т.е. меняем цвет
void interrupt new0A(...){get_reg(); old0A();}
void interrupt new0B(...){get_reg(); old0B();}
void interrupt new0C(...){get_reg(); old0C();}
void interrupt new0D(...){get_reg(); old0D();}
void interrupt new0E(...){get_reg(); old0E();}
void interrupt new0F(...){get_reg(); old0F();}

void interrupt new60(...){get_reg(); old60();}
void interrupt new61(...){get_reg(); old61();}
void interrupt new62(...){get_reg(); old62();}
void interrupt new63(...){get_reg(); old63();}
void interrupt new64(...){get_reg(); old64();}
void interrupt new65(...){get_reg(); old65();}
void interrupt new66(...){get_reg(); old66();}
void interrupt new67(...){get_reg(); old67();}

void print(int val, int offset)//отображаем состояние регистров
{
        VIDEO far* screen = (VIDEO far *)MK_FP(0xB800, offset);

        for(int i = 7; i >= 0; i--)	
        {
                screen->symb = val % 2 + '0';
                screen->attr = attr;
                screen++;
				val /= 2;
        }
}
void get_reg()//получаем состояние регистров
{
        outp(0x20, 0x0A); //считывем регистр запросов ведущего контроллера
        print(inp(0x20),0);//выводим его
        outp(0xA0, 0x0A);//считывем регистр запросов ведомого контроллера
        print(inp(0xA0), 16);//выводим его
        outp(0x20, 0x0B);//считывем регистр обслуживания ведущего контроллера
        print(inp(0x20), 32);//выводим его
        outp(0xA0, 0x0B);//считывем регистр обслуживания ведомого контроллера
        print(inp(0xA0), 48);//выводим его


        print(inp(0x21), 64);//выводим регистр масок ведущего контроллера
        print(inp(0xA1), 80);//выводим регистр масок ведомого контроллера
}

void init()
{

        old08 = getvect(0x08);//считывем старые обработчики
        old09 = getvect(0x09);
        old0A = getvect(0x0A);
        old0B = getvect(0x0B);
        old0C = getvect(0x0C);
        old0D = getvect(0x0D);
        old0E = getvect(0x0E);
        old0F = getvect(0x0F);
		
        old60 = getvect(0x70);
        old61 = getvect(0x71);
        old62 = getvect(0x72);
        old63 = getvect(0x73);
        old64 = getvect(0x74);
        old65 = getvect(0x75);
        old66 = getvect(0x76);
        old67 = getvect(0x77);
		
		setvect(0x08, new08);//записываем свои обработчики
        setvect(0x09, new09);
        setvect(0x0A, new0A);
        setvect(0x0B, new0B);
        setvect(0x0C, new0C);
        setvect(0x0D, new0D);
        setvect(0x0E, new0E);
        setvect(0x0F, new0F);
		
        setvect(0x60, new60);
        setvect(0x61, new61);
        setvect(0x62, new62);
        setvect(0x63, new63);
        setvect(0x64, new64);
        setvect(0x65, new65);
        setvect(0x66, new66);
        setvect(0x67, new67);

        _disable();//запрещаем прерывание
   
        // MASTER
        outp(0x20, 0x11);		// ICW1
        outp(0x21, 0x08);		// ICW2
        outp(0x21, 0x04);		// ICW3
        outp(0x21, 0x01);		// ICW4

		//SLAVE
        outp(0xA0, 0x11);		// ICW1
        outp(0xA1, 0x60);		// ICW2
        outp(0xA1, 0x02);		// ICW3
        outp(0xA1, 0x01);		// ICW4

        _enable();//разрешаем прерывание
}

int main()
{
        unsigned far *fp;
        init();

        FP_SEG (fp) = _psp;// получаем сегмент	
        FP_OFF (fp) = 0x2c;	//и смещение сегмента данных  
        _dos_freemem(*fp); //чтобы освободить память

        _dos_keep(0,(_DS -_CS)+(_SP/16)+1);//оставляем резидентной
        return 0;
}


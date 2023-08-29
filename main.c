#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define RS_H (PORTC |= 0x01)		// RS=1
#define RS_L (PORTC &= 0xFE)		// RS=0
#define RW_H (PORTC |= 0x02)		// RW=1
#define RW_L (PORTC &= 0xFD)		// RW=0
#define ENABLE (PORTC |= 0x04)		// ENABLE
#define DISABLE (PORTC &= 0xFB)		// DISABLE
#define STX 0x02 
#define ETX 0x03   

void UART_init();
void UART_TX_CH(char c);
void UART_TX_STR(char *s);
ISR(USART_RX_vect);
void LCD_command(char command);
void LCD_data(char data);
void LCD_clear();
void LCD_init();
void LCD_main();
void LCD_string(char command, char *string);
void Pass_equal(char a[], char b[]);
void Pass_empty(char a[]);
void Pass_check();
void TIMER_init();
void LED_init();
void LED_control();

char rx_buf[20], rx_cnt = 0;
char tx_buf[20], tx_cnt = 0;
char in_pass[4], pass_cnt = 0;
char in_pass_buf[4];
char rxdata;
char led_flag;
char emp[]={' '};
volatile char rx_flag = 0;

char string1[16] = "Password : ";
char success[16] = "Success !";
char fail[16] = "Failure !";
char pass[4] = {'7','1','9','3'};

int success_flag = 0;
int fail_flag = 0;
int check_flag = 0;

void UART_init(){
	UCSR0A = 0;
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
	UBRR0H = 0;
	UBRR0L = 103;
}

void UART_TX_CH(char c){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

void UART_TX_STR(char *s) {
	while(*s) {
		UART_TX_CH(*s++);
	}
}

ISR(USART_RX_vect) {
	int i;
	char rxdata;
	rxdata = UDR0;
	if(!rx_flag) {
		if(rxdata == STX) {
			rx_cnt = 0;
			pass_cnt = 0;
		}
		else if(rxdata == ETX) {
			for(i=0; (i<rx_cnt) && (i<10); i++) {
				tx_buf[i] = rx_buf[i];
				rx_buf[i] = ' ';
			}
			rx_cnt = 0;
			pass_cnt = 0;
			rx_flag = 1;
		}
		else {
			if(pass_cnt==4) {
				pass_cnt = 0;
				check_flag = 1;
			}
			else{
				if(rxdata == '0' || rxdata == '1' || rxdata == '2' || rxdata == '3' || rxdata == '4' || rxdata == '5' || rxdata == '6'
				|| rxdata == '7' || rxdata == '8' || rxdata == '9' || rxdata == '*' || rxdata == '#')
				{
					in_pass[pass_cnt] = rxdata;
					pass_cnt++;
				}
			}
		}
	}
}

void LCD_command(char command){
	PORTD = (command&0xF0);			// send High nibble
	RS_L; RW_L;						// RS=0 RW=0
	ENABLE;							// E1 : L->H
	_delay_us(1);
	DISABLE;						// E1 : H->L

	PORTD = (command & 0x0F)<<4;	// send Low nibble
	RS_L; RW_L;						// RS=0 RW=0
	ENABLE;							// E1 : L->H
	_delay_us(1);
	DISABLE;						// E1 : H->L
}

void LCD_data(char data){
	_delay_us(100);
	PORTD = (data&0xF0);			// send High nibble
	RS_H; RW_L;						// RS=1 RW=0
	ENABLE;							// E1 : L->H
	_delay_us(1);
	DISABLE;							// E1 : H->L

	PORTD = (data&0x0F)<<4;			// send Low nibble
	RS_H; RW_L;						// RS=1 RW=0
	ENABLE;							// E1 : L->H
	_delay_us(1);
	DISABLE;						// E1 : H->L
}

void LCD_clear()
{
	LCD_command(0x01);
}

void LCD_init(void){
	DDRC = 0xFF;
	DDRD = 0xFF;

	PORTC = 0x00;
	PORTD = 0x00;

	_delay_ms(40);
	LCD_command(0x28);		// DL=0(4bit) N=1(2Line) F=0(5x7)
	_delay_us(50);

	LCD_command(0x0C);		// LCD ON, Cursor X, Blink X
	_delay_us(50);

	LCD_command(0x01);		// LCD Clear
	_delay_ms(2);

	LCD_command(0x06);		// Entry Mode
	_delay_us(50);
}

void LCD_string(char command, char *string){
	LCD_command(command);
	while(*string != '\0'){
		LCD_data(*string);
		string++;
	}
}

void LCD_main()
{
	LCD_string(0x80, string1);
	_delay_ms(2);
	LCD_string(0x8B,in_pass);
	_delay_ms(2);
}

void Pass_equal(char a[], char b[])
{
		check_flag = 0;
		if(a[0] == b[0] && a[1] == b[1] && a[2] == b[2] && a[3] == b[3])
		{
			success_flag = 1;
		}
		else if(a[0] != b[0] || a[1] != b[1] || a[2] != b[2] || a[3] != b[3])
		{
			fail_flag = 1;
		}
		else ;
}

void Pass_empty(char a[])
{
	for(int i = 0; i < 4; i++)
	{
		a[i] = ' ';
	}
}

void Pass_check()
{
	if(check_flag) Pass_equal(pass, in_pass);
	
	if(success_flag == 1)
	{
		*in_pass = 0;
		success_flag = 0;
		LCD_string(0xC0, success);
		_delay_ms(2);
		OCR1A = 330;
		_delay_ms(2000);
		OCR1A = 635;
		LCD_clear();
		Pass_empty(in_pass);
	}
	else if(fail_flag == 1)
	{
		*in_pass = 0;
		fail_flag = 0;
		LCD_string(0xC0, fail);
		_delay_ms(2000);
		UART_TX_CH('c');
		_delay_ms(2);
		UART_TX_CH('b');
		LCD_clear();
		Pass_empty(in_pass);
	}
	else ;
}

void TIMER_init()
{
	TCCR1A = 0x82;
	TCCR1B = 0x1b;
	ICR1 = 4000;
	OCR1A = 635;
	DDRB |= 0x02;
	PORTB |= 0x02;
}

void LED_init()
{
	DDRB |= 0x10;
	PORTB |= 0x00;
}

void LED_control()
{
	if(led_flag) PORTB = 0x10;
	else PORTB = 0x00;
}

int main(void){
	LCD_init();
	UART_init();
	TIMER_init();
	LED_init();
	sei();
	
	while(1){
		LCD_main();
		Pass_check();
		LED_control();
	}
	return 0;
}



                                                                                          

#include <p18f2550.h> 
//#include <timers.h>


#define BUTTON1 PORTCbits.RC0 //down arrow
#define BUTTON2 PORTCbits.RC1 //up arrow
#define BUTTON3 PORTCbits.RC2
#define BUTTON4 PORTAbits.RA5

#define NINE PORTCbits.RC7
#define TEN  PORTCbits.RC6
#define ELEVEN PORTCbits.RC5


//shift register
#define Data PORTAbits.RA4
#define Clock PORTAbits.RA3

#define SEG_F   0b00000001
#define SEG_DOT 0b00000010
#define SEG_C   0b00000100
#define SEG_B   0b00001000
#define SEG_D   0b00010000
#define SEG_A   0b00100000
#define SEG_E   0b01000000
#define SEG_G   0b10000000

    char NumberSystem[10] = {SEG_A|SEG_B|SEG_E|SEG_D|SEG_F|SEG_G,//0
							 SEG_A | SEG_B, //1
							 SEG_E|SEG_A|SEG_C|SEG_F|SEG_G,//2
							 SEG_E|SEG_A|SEG_C|SEG_B|SEG_G,//3
							 SEG_D|SEG_A|SEG_C|SEG_B,//4
							 SEG_E|SEG_D|SEG_C|SEG_B|SEG_G,//5
							 SEG_E|SEG_D|SEG_C|SEG_B|SEG_G|SEG_F,//6
							 SEG_E|SEG_A|SEG_B,//7
							 SEG_D|SEG_E|SEG_A|SEG_C|SEG_F|SEG_G|SEG_B,//8
							 SEG_D|SEG_E|SEG_A|SEG_C|SEG_B|SEG_G};//9
	
		/*
			                 {SEG_A|SEG_B|SEG_E|SEG_D|SEG_F|SEG_G,//0
                             SEG_A|SEG_B,//1
                             
                             
                             
                             
                             
                             
                             
                             |
      */
      /*
     ---------------
           E
     ---------------
|    |              |  A |
| D  |              |    |
|    |              |    |
     ---------------
          C
     ---------------
|    |              |    |
|  F |              |  B |
|    |              |    |
     ---------------
         G
     ---------------

*/



void GiveSegments(unsigned char input){
	PORTB = PORTB &         0b11100000;
	PORTB = PORTB |  (input&0b00011111);

	PORTCbits.RC7 = (input&0b10000000)!=0;

    PORTA = PORTA &         0b11111001;
	PORTA = PORTA |  ((input>>4)&0b00000110);
	
	//	PORTC = !PORTC;

}	


#define CLOCKPULSE Clock = 0;\
                   Clock = 1;
                   
                   
void GiveShifter(unsigned char input){
	Data = ((input & 0b00000001) != 0);
	CLOCKPULSE;
	Data = ((input & 0b00000010) != 0);
	CLOCKPULSE;
	Data = ((input & 0b00000100) != 0);
	CLOCKPULSE;
	Data = ((input & 0b00001000) != 0);
	CLOCKPULSE;
	Data = ((input & 0b00010000) != 0);
    CLOCKPULSE;
	Data = ((input & 0b00100000) != 0);
	CLOCKPULSE;
	Data = ((input & 0b01000000) != 0);
	CLOCKPULSE;
}
	



	void Delay(){
	unsigned char i,j;
	for(i=0; i< 2; i++){
	  for(j=0; j< 120; j++){
	      _asm
	      nop
	      nop
	      nop
          _endasm
	}
	}	
	
}





void SetPair(unsigned char input, unsigned char dec){
	unsigned char Ones;
	unsigned char Tens;
	
	Ones = input % 10;
	Tens = (input-Ones)/10;
	
	 GiveSegments(0);
	 CLOCKPULSE;
	 Data=1;
	 GiveSegments(NumberSystem[Ones] | (dec?SEG_DOT:0));
	 Delay(255);
	 GiveSegments(0);
	 CLOCKPULSE;
	 if(dec && Tens==0){
		 	GiveSegments(0);
	} else { 
	 	GiveSegments(NumberSystem[Tens]);
	} 
	 Delay(255);
}	


unsigned char Year    = 0;
unsigned char Day     = 0;
unsigned char Hour    = 11;
unsigned char Minute  = 58;
unsigned char Second  = 0;
unsigned char Csecond = 0;


void IncrementTime(){
	Csecond++;
	if(Csecond > 99){
		Csecond = 0;
		Second++;
		if(Second>59){
		   Second = 0;
		   Minute++;
		   if(Minute>59){
			   Minute=0;
			   Hour++;
			   if(Hour>12){
				   Hour=1;
				  }
			  } 
		}   
	}	
	
}
	

//#define (Required time)/(4 * Timer clock * prescale value)
//     = 1 / (4  * (1/32000000) * 256)
//     = 31250
     





#pragma code
#pragma interrupt InterruptHandlerHigh

void InterruptHandlerHigh()
{
 if (INTCONbits.TMR0IF)
 {
	   WriteTimer0(0xFFFF-2480); //2485 too slow. 2475 too fast
	    INTCONbits.TMR0IF=0;         //clear interrupt flag
   IncrementTime();

  
 }
}

#pragma code InterruptVectorHigh = 0x08
void IntereuptVectorHigh(void)
{
_asm
	goto InterruptHandlerHigh
_endasm	
}


#pragma code
void main(void){
	
	
    unsigned char counter1=0;
	//	OSCCON = 0x40; //For 1 MHz
  //  OSCTUNE = 0x40; //enable PLL
	
//	OSCCON = 0b11111100; //set for 8 MHz operation 
	//PLLEN = 1; //enable PLL 
   
	TRISC = 0b00000111;
	TRISB = 0b00000000;
    TRISA = 0b00100000;



		
	PORTC = 0b00000111;
	PORTB = 0b00000000;
	PORTA = 0b00100000;
	
	 //   OpenTimer0( TIMER_INT_ON &
       //         T0_16BIT &
         //       T0_SOURCE_INT &
           //     T0_PS_1_256);

    //Write Timer
   WriteTimer0(0);

    INTCON=0x20;
    INTCON2=0x04;
    RCONbits.IPEN = 1;        //enable high interrupts
   
  
   T0CON=0x82; //prescaler 1:8
   INTCONbits.GIEH = 1;

	while(1){
		counter1++;



		Data=0;
		SetPair(Csecond,0);
		SetPair(Second,1);
		SetPair(Minute,0);
		SetPair(Hour,1);
	//	IncrementTime();
	
	
	
		
		
		
		if(BUTTON3){//index button
			counter1++;
			if(counter1 > 20){
				counter1=17;
			if(BUTTON1){//down arrow
		  		 Minute--;
		  		 if(Minute ==255){
		     		 Minute = 59;
		     		 Hour--;
		     		 if(Hour ==0)
		     		 	Hour = 12;
		     	}		 
		 	}    	 
		
		
			if(BUTTON2){//up arrow
		  		Minute++;
		   		if(Minute == 60){
		     	   Minute = 0;
		      	   Hour++;
		           if(Hour == 13)
		      	      Hour = 1;
		   		}   
			}
		
		}
		} else {
			counter1=0;
		}
		
			PORTA = PORTA | 0b00100000;
		
		if(BUTTON4){//enter button //#define BUTTON4 PORTAbits.RA5
		   Minute--;
		   if(Minute == 255)
		      Minute = 60;
		}
		
		PORTC = PORTC | 0b00010000;
		Delay();
	}	
	
	
	
}	
#include "main.h"
#include <string.h>
#include <stdlib.h>

#include <avr/sleep.h>
#include <avr/wdt.h>

uint8_t buf[16];

void
myputs (char *buf)
{
  int i = 0;

  while (1)
    {
      if (buf[i] == 0)
	break;
      while (!(USR & (1 << UDRE)));

      UDR = buf[i];
      i++;

    }
}


ISR (INT1_vect)
{
  cli ();
}

#define LED_PORT PORTB
#define LED_DDR  DDRB
#define LED_BITS        0xff

#define Reset_AVR() wdt_enable(WDTO_30MS); while(1) {}

void
PutHexChar (const int val)
{
  while (!(USR & (1 << UDRE)));

  if (val > 9)
    {
      UDR = ((char) ('A' - 10) + val);
    }
  else
    {
      UDR = ((char) '0' + val);
    }
}


void
PutHex (unsigned int val)
{
  unsigned int i;
  for (i = 0; i < 4; i++)
    {
      PutHexChar ((val & 0xF000) >> 12);
      val <<= 4;
    }
}



void
PutCharHex (unsigned char val)
{
  PutHexChar ((val & 0xF0) >> 4);
  PutHexChar ((val & 0x0F));
}



int
main (void)
{
  long int i; 
  unsigned int b = 0;

  UBRR = 12;

  UCR |= (1 << TXEN);
  UCR |= (1 << RXEN);
  _delay_ms (100.0);


  myputs ("\r\r\n\nsystem start\n\r");
  myputs ("disable reset\n\r");

  DDRD = 0x00;

  DDRB |= LED_BITS;

  while (1)
    {
      PORTB = 0;

      // 50 = 10 sec ( 50 x 200ms)
#define X 60 
// 60
      for (i = 50; i > 0; i--)
	{
	  PutHex (i);
	  myputs (" ");

	  if (i > 0)
	    LED_PORT |= 0x4;
	  else
	    LED_PORT &= ~0x4;

	  if (i > 21 * X * 5)
	    LED_PORT |= 0x8;
	  else
	    LED_PORT &= ~0x8;

	  if (i > 41 * X * 5)
	    LED_PORT |= 0x10;
	  else
	    LED_PORT &= ~0x10;

	  PutCharHex (PIND);

	  if ((PIND & 0x8) == 0 && ( i < ( X * 60 * 5 + 1)) && b == 0)
	    {
	      b = 3;
	      i = i + ( X * 20 * 5);  
	      myputs ("+\n\r");
	    }

	  if (b > 0)
	    b--;
	  _delay_ms (200.0);
	  myputs ("\n\r");
	}

      LED_PORT = 0;

      MCUCR = 0x30;
      GIMSK = 0x80;
      sei ();

      myputs ("sleep now\n\r");
      sleep_mode ();
      _delay_ms (100.0);
      myputs ("wakeup\n\r");

      Reset_AVR ();
    }

}

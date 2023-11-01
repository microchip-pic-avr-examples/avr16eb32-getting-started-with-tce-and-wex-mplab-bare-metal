/* Used by util/delay.h */
#define F_CPU (3333333UL)

/* Pattern of complementary 0 and 1 for the WEX's outputs */
#define COMPLEMENTARY_PATTERN_EXAMPLE    (0x55)

#include <util/delay.h>
#include <avr/io.h>


void PORT_Initialize(void)
{   
    /* Route the pins on the default PORT A */
    PORTMUX.TCEROUTEA = 0x0;

    /* Set pins 0-7 from PORT A as outputs */
    PORTA.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
}

void WEX0_Initialize(void)
{    
    /* Enables pattern generation mode */
    WEX0.CTRLA   = WEX_PGM_bm;

     /* Enables WEX's outputs to be overriden in pattern generation mode */
    WEX0.PGMOVR  = WEX_PGMOVR0_bm | WEX_PGMOVR1_bm | WEX_PGMOVR2_bm | WEX_PGMOVR3_bm | WEX_PGMOVR4_bm | WEX_PGMOVR5_bm | WEX_PGMOVR6_bm | WEX_PGMOVR7_bm; 

    /* Sets the pattern each output will take in pattern generation, each output has a one-to-one relationship
     * with the bits from PGMOUT register */
    WEX0.PGMOUT  = COMPLEMENTARY_PATTERN_EXAMPLE;

    /* Enables WEX's outputs */
    WEX0.OUTOVEN = WEX_OUTOVEN0_bm | WEX_OUTOVEN1_bm | WEX_OUTOVEN2_bm | WEX_OUTOVEN3_bm | WEX_OUTOVEN4_bm | WEX_OUTOVEN5_bm | WEX_OUTOVEN6_bm | WEX_OUTOVEN7_bm;
}

void TCE0_Start(void)
{
    /* Start TCE in order to get the peripheral clock for the WEX peripheral */
    TCE0.CTRLA = TCE_ENABLE_bm;
}

void Complementary_Pattern_Set(void)
{
    WEX0.PGMOUT = COMPLEMENTARY_PATTERN_EXAMPLE;
        
    _delay_us(25);

    /* Complementary signals pattern */
    for(uint8_t i = 0; i < 9; i++)
    {    
        /* Toggle the pattern for each of the WEX's output */
        WEX0.PGMOUT = ~WEX0.PGMOUT;
        /* Software delay added in order for the toggle to be visible */
        _delay_us(25);
    }

    /* Put all signals in low '0' logic and wait 1ms to see the transition from one of the 
     * complementary patterns to the stairs pattern*/
    WEX0.PGMOUT = 0x00;
}

void Stairs_Pattern_Set(void)
{
    /* Each of the signals switch from low to high one at a time in increasing order*/
    WEX0.PGMOUT = WEX_PGMOUT7_bm;
    for(uint8_t step = 0; step < 8; step ++)
    {
        /* Software delay added in order for the increment to be visible */
        _delay_us(5);
        /* Generate a stairs increment pattern*/
        WEX0.PGMOUT = WEX0.PGMOUT>>1;
    }

    /* Each of the signals switch from low to high one at a time in increasing order*/
    WEX0.PGMOUT = WEX_PGMOUT1_bm;
    for(uint8_t step = 0; step < 8; step ++)
    {
        /* Software delay added in order for the increment to be visible */
        _delay_us(5);
        /* Generate a stairs decrement pattern*/
        WEX0.PGMOUT = WEX0.PGMOUT<<1;
    }

    /* Put all signals in low '0' logic and wait 1ms to see the transition from the 
     * stairs pattern to one of the complementary patterns*/
    WEX0.PGMOUT = 0x00;
}

int main(void) {
    
    PORT_Initialize();
    WEX0_Initialize();
    TCE0_Start();
    
    while (1) 
    {
        Complementary_Pattern_Set();
        _delay_us(250);
        Stairs_Pattern_Set();
        _delay_us(250);
    }
}

/* Used by util/delay.h */
#define F_CPU (20000000UL)

/* Calculated values for Period, CMP, Amplitude and OFFSET registers */
#define TCE_PERIOD                      (0xFA0)
#define DUTY_CYCLE_20_PERCENT           (0x1999)
#define DUTY_CYCLE_40_PERCENT           (0x3333)
#define DUTY_CYCLE_60_PERCENT           (0x4CCC)
#define DUTY_CYCLE_80_PERCENT           (0x660C)
#define AMPLITUDE_MAX_DCY_50_PERCENT    (0x4000)
#define AMPLITUDE_MAX_DCY_75_PERCENT    (0x6000)
#define AMPLITUDE_MAX_DCY_100_PERCENT   (0x8000)
#define AMPLITUDE_MAX_DCY_150_PERCENT   (0xC000)
#define TCE_OFFSET (0x00)

#include <util/delay.h>
#include <avr/io.h>


void CLKCTRL_Initialize(void)
{
    /* Disable the clock prescaler, set the clock prescaler B to DIV4*/
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, (CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm) | CLKCTRL_PBDIV_DIV4_gc);
    
    /* Select source division for PLL to DIV4, set PLL multiplication factor to 16 */
    _PROTECTED_WRITE(CLKCTRL.PLLCTRLA, CLKCTRL.PLLCTRLA | 0x13);
    
    /* Set PLL as clock source */
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL.MCLKCTRLA | 0x4);
}

void PORT_Initialize(void)
{
    /* Route the pins on the default PORT A */
     PORTMUX.TCEROUTEA = 0x0;
     
    /* Set pins 0-3 from PORT A as outputs */
    PORTA.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm;
    
    /* Set pins 5 from PORT D as output */
    PORTD.DIRSET = PIN5_bm;
}

void TCE0_Initialize(void)
{
    /* Set clock source (sys_clk/1) */
    TCE0.CTRLA = TCE_CLKSEL_DIV1_gc;
    
    /* Set the timer to count down (decrementing) */
    TCE0.CTRLECLR = TCE_DIR_bm;
    
    /* Enable the 4 compare channels that will be used to generate PWM signals */
    TCE0.CTRLB |= (TCE_CMP0EN_bm | TCE_CMP1EN_bm | TCE_CMP2EN_bm | TCE_CMP3EN_bm);
    
    /* Set waveform generation mode to dual slope with overflow on top and bottom */
    TCE0.CTRLB |= TCE_WGMODE_DSBOTH_gc;
    
    /* Set the counter to start from 0 */
    TCE0.CNT = 0;
    
    /* Enable amplitude, offset, high resolution increased by 2 bits, scaling and set the compare
     * values scaling from bottom 0% duty cycle up to 100% duty cycle */
    TCE0.CTRLD = TCE_HREN_4X_gc | TCE_SCALE_bm | TCE_AMPEN_bm | TCE_SCALEMODE_BOTTOM_gc;
     
    /* Set amplitude (compare values are scaled using amplitude and offset) */
    TCE0.AMP = AMPLITUDE_MAX_DCY_100_PERCENT;
    
    /* Set offset */
    TCE0.OFFSET = TCE_OFFSET;
    
    /* Set the period to 100us*/
    TCE0.PER = TCE_PERIOD;
    
    /* Set duty cycle to compare channel 0 to 20% - 20us*/
    TCE0.CMP0 = DUTY_CYCLE_20_PERCENT;
    
    /* Set duty cycle to compare channel 0 to 40% - 40us*/
    TCE0.CMP1 = DUTY_CYCLE_40_PERCENT;
    
    /* Set duty cycle to compare channel 0 to 60% - 60us*/
    TCE0.CMP2 = DUTY_CYCLE_60_PERCENT;
    
    /* Set duty cycle to compare channel 0 to 80% - 80us*/
    TCE0.CMP3 = DUTY_CYCLE_80_PERCENT;
   
    /* Start timer */
    TCE0.CTRLA |= TCE_ENABLE_bm;
}

void Amplitude_Value_Set(uint16_t value)
{
    /* Set the value of amplitude */
    TCE0.AMP = value;
    
    /* Rewrite the value in CMP0BUF register so that the duty cycle get scaled according to the new value of amplitude */
    TCE0.CMP0BUF = DUTY_CYCLE_20_PERCENT;
    
    /* Rewrite the value in CMP1BUF register so that the duty cycle get scaled according to the new value of amplitude */
    TCE0.CMP1BUF = DUTY_CYCLE_40_PERCENT;
    
    /* Rewrite the value in CMP2BUF register so that the duty cycle get scaled according to the new value of amplitude */
    TCE0.CMP2BUF = DUTY_CYCLE_60_PERCENT;
    
    /* Rewrite the value in CMP3BUF register so that the duty cycle get scaled according to the new value of amplitude */
    TCE0.CMP3BUF = DUTY_CYCLE_80_PERCENT;
    
    /* Toggle that signals when the value of amplitude has changed */
    PORTD.OUTTGL = PIN5_bm;
}

int main(void) 
{
    
    CLKCTRL_Initialize();
    PORT_Initialize();
    TCE0_Initialize();
    
    while (1) 
    {  
        
        Amplitude_Value_Set(AMPLITUDE_MAX_DCY_50_PERCENT);
        _delay_ms(10);
        Amplitude_Value_Set(AMPLITUDE_MAX_DCY_75_PERCENT);
        _delay_ms(10);
        Amplitude_Value_Set(AMPLITUDE_MAX_DCY_100_PERCENT);
        _delay_ms(10);
        Amplitude_Value_Set(AMPLITUDE_MAX_DCY_150_PERCENT);
        _delay_ms(10);
    }
}

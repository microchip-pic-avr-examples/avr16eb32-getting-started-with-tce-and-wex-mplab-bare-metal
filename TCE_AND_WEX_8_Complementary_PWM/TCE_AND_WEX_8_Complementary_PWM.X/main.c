/* Used by util/delay.h */
#define F_CPU                                   (20000000UL)

/* Calculated values for Period, Maximum Duty Cycle and Dead Time */
#define TCE_PERIOD                              (0x3E7)
#define MAX_DUTY_CYCLE                          (0x3DE)
#define DEAD_TIME_LS                            (0x03)
#define DEAD_TIME_HS                            (0x05)

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>


void CLKCTRL_Initialize(void)
{
    /* Disable the clock prescaler */
    _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLKCTRL.MCLKCTRLB & ~CLKCTRL_PEN_bm);
}

void PORT_Initialize(void)
{   
    /* Route the pins on the default PORT A */
    PORTMUX.TCEROUTEA = 0x0;
     
    /* Set pins 0-7 from PORT A as outputs */
    PORTA.DIRSET = PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm;
}

/* Compare 0 ISR */
ISR(TCE0_CMP0_vect)
{
    /* Clear the interrupt flag */
    TCE0.INTFLAGS = TCE_CMP0_bm;
    
    static uint16_t duty_cycle = 0;
    
    /* Duty cycle update in interrupt */
    duty_cycle += 5;
    if(duty_cycle >= MAX_DUTY_CYCLE)
    duty_cycle = 0;
    TCE0.CMP0BUF = duty_cycle;
}

/* Compare 1 ISR */
ISR(TCE0_CMP1_vect)
{
   /* Clear the interrupt flag */
    TCE0.INTFLAGS = TCE_CMP1_bm;
    
    static uint16_t duty_cycle = 0;
    
    /* Duty cycle update in interrupt */
    duty_cycle += 15;
    if(duty_cycle >= MAX_DUTY_CYCLE)
    duty_cycle = 0;
    TCE0.CMP1BUF = duty_cycle;
}

/* Compare 2 ISR */
ISR(TCE0_CMP2_vect)
{
   /* Clear the interrupt flag */
    TCE0.INTFLAGS = TCE_CMP2_bm;
    
    static uint16_t duty_cycle = 0;
    
    /* Duty cycle update in interrupt */
    duty_cycle += 25;
    if(duty_cycle >= MAX_DUTY_CYCLE)
    duty_cycle = 0;
    TCE0.CMP2BUF = duty_cycle;
}

/* Compare 3 ISR */
ISR(TCE0_CMP3_vect)
{
   /* Clear the interrupt flag */
    TCE0.INTFLAGS = TCE_CMP3_bm;
    
    static uint16_t duty_cycle = 0;
    
    /* Duty cycle update in interrupt */
    duty_cycle += 35;
    if(duty_cycle >= MAX_DUTY_CYCLE)
    duty_cycle = 0;
    TCE0.CMP3BUF = duty_cycle;
}

/* WEX fault ISR */
ISR(WEX0_FDFEVA_vect)
{
    /* Clear the interrupt flag and fault event flag */
    WEX0.INTFLAGS = WEX_FDFEVA_bm | WEX_FAULTDET_bm;
}

void WEX0_Initialize(void)
{    
    /* Select the output matrix for direct routing from the TCE outputs and enable 
     * dead-time insertion for each compare channel */
    WEX0.CTRLA = WEX_INMX_DIRECT_gc | WEX_DTI0EN_bm | WEX_DTI1EN_bm | WEX_DTI2EN_bm | WEX_DTI3EN_bm; 
    
    /* Enable event input A */
    WEX0.EVCTRLA = WEX_FAULTEI_bm;
    
     /* Set fault restart mode to cycle - by - cycle */
    WEX0.FAULTCTRL = WEX_FDMODE_LATCHED_gc;
    
    /* Drive all pins to low '0' logic when a fault is detected */
    WEX0.FAULTCTRL |= WEX_FDACT_LOW_gc;
    
    /* Set dead-time on low side to be 3 clock ticks */
    WEX0.DTLS = DEAD_TIME_LS;
    
    /* Set dead-time on high side to be 5 clock ticks */
    WEX0.DTHS = DEAD_TIME_HS;
    
     /* Enables WEX's outputs */
    WEX0.OUTOVEN = WEX_OUTOVEN0_bm | WEX_OUTOVEN1_bm | WEX_OUTOVEN2_bm | WEX_OUTOVEN3_bm | WEX_OUTOVEN4_bm | WEX_OUTOVEN5_bm | WEX_OUTOVEN6_bm | WEX_OUTOVEN7_bm;
    
    /* Enable fault detection*/
    WEX0.INTCTRL = WEX_FAULTDET_bm;
}

void TCE0_Initialize(void)
{
    /* Set clock source (sys_clk/1) */
    TCE0.CTRLA = TCE_CLKSEL_DIV1_gc;
    
    /* Set the timer to count down (decrementing) */
    TCE0.CTRLECLR = TCE_DIR_bm;
    
    /* Enable the 4 compare channels that will be used to generate PWM signals */
    TCE0.CTRLB = (TCE_CMP0EN_bm | TCE_CMP1EN_bm | TCE_CMP2EN_bm | TCE_CMP3EN_bm);
    
    /* Set waveform generation mode to single slope with overflow on top */
    TCE0.CTRLB |= TCE_WGMODE_SINGLESLOPE_gc;
    
    /* Set the counter to start from 0 */
    TCE0.CNT = 0x00;
    
    /* Set the period to 50us*/
    TCE0.PER = TCE_PERIOD;
    
    /* Set initial duty cycle to compare channel 0 */
    TCE0.CMP0 = 0x00;
    
    /* Set initial duty cycle to compare channel 1 */
    TCE0.CMP1 = 0x00;
    
    /* Set initial duty cycle to compare channel 2 */
    TCE0.CMP2 = 0x00;
    
    /* Set initial duty cycle to compare channel 3 */
    TCE0.CMP3 = 0x00;
    
    /* Enable interrupts for every compare channel match with the counter */
    TCE0.INTCTRL = (TCE_CMP0_bm | TCE_CMP1_bm | TCE_CMP2_bm | TCE_CMP3_bm);
   
    /* Start timer */
    TCE0.CTRLA |= TCE_ENABLE_bm;
}

void EVSYS_Initialize(void)
{
    /* Set the WEX0 peripheral as the user of the event generator from channel 0 */
    EVSYS.USERWEXA = EVSYS_USER_CHANNEL0_gc;
}

void Create_Fault(void)
{
    /* Fault creation, repeat in main loop to see it on Logic Analyzer. This is an event generated using a software command */
    EVSYS.SWEVENTA = EVSYS_SWEVENTA_CH0_gc;
}

void Clear_Fault(void)
{
    /* Clear fault condition using a software command */
    WEX0.CTRLC = WEX_CMD_FAULTCLR_gc;
}

int main(void) {
    
    /* Modules and Port Initialization */
    CLKCTRL_Initialize();
    PORT_Initialize();
    EVSYS_Initialize();
    TCE0_Initialize();
    WEX0_Initialize();
    
    /* Enable global interrupts */
    sei();

    while (1) 
    {
        Create_Fault();
        _delay_us(250);
        Clear_Fault();
        _delay_us(250);
    }
}

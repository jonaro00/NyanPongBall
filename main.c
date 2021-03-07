
#include <stdint.h>
#include <pic32mx.h>
#include "game.h"

int main(){
    /*
        This will set the peripheral bus clock to the same frequency
        as the sysclock. That means 80 MHz, when the microcontroller
        is running at 80 MHz. Changed 2017, as recommended by Axel.
    */
    SYSKEY = 0xAA996655;  /* Unlock OSCCON, step 1 */
    SYSKEY = 0x556699AA;  /* Unlock OSCCON, step 2 */
    while(OSCCON & (1 << 21)); /* Wait until PBDIV ready */
    OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
    while(OSCCON & (1 << 21));  /* Wait until PBDIV ready */
    SYSKEY = 0x0;  /* Lock OSCCON */

    // DISPLAY SETUP
    // Output pins for display signals
    PORTF = 0xFFFF;
    PORTG = (1 << 9);
    ODCF = 0x0;
    ODCG = 0x0;
    TRISFCLR = 0x70;
    TRISGCLR = 0x200;
    // Set up SPI as master
    SPI2CON = 0;
    SPI2BRG = 4;
    // SPI2STAT bit SPIROV = 0;
    SPI2STATCLR = 0x40;
    // SPI2CON bit CKP = 1;
    SPI2CONSET = 0x40;
    // SPI2CON bit MSTEN = 1;
    SPI2CONSET = 0x20;
    // SPI2CON bit ON = 1;
    SPI2CONSET = 0x8000;

    display_init();

    // Configure timer 2
    T2CON = 0x70; // Clear timer 2 registers and set prescaler to 256 (0b111)
    PR2 = 80000000 / 256 / FPS; // Set period register to desired period
    TMR2 = 0; // Reset timer 2
    T2CONSET = 0x8000; // Start timer 2
    IEC(0) = (1 << 8); // Enable interrupts from timer 2
    IPC(2) = 4; // set priority for timer 2

    // EXT INT
    INTCONSET = 0b11111; // rising edge for ext int
    IEC(0) |= (1 << 7); // Enable interrupts from switch 1
    IPC(1) |= (1 << 26); // set priority for SW1
    enable_interrupt();

    // LEDS
    TRISECLR = 0xFF; // set lower 8 bits of port E to output (LEDS)
    PORTE = 0;
    ODCE = 0x0;

    // BTN & SW
    TRISDSET = 0xFE0; // set bits 11-5 of port D to input (btns and switches)
    TRISFSET = 0x2; // set bit 1 of port F to input (btn1)

    // ADC
    AD1PCFG = 0xFFFB; // PCFG2 = input
    TRISBSET = 4;
    AD1CHS = (0x2 << 16);
    AD1CON1 = (0x4 << 8) | (0x7 << 5);
    AD1CON2 = 0x0;
    AD1CON3 |= (0x1 << 15);
    AD1CON1 |= (0x1 << 15); // turn on

    while(1){
        loop();
    }
    return 0;
}

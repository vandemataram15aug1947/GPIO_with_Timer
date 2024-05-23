//#############################################################################
//
// FILE:   GPIO_timer_main.c
//
// TITLE:  GPIO and timer Example
//
// GPIO and timer Bit-Field & Driverlib Example
//
// This program is based on an example empty project setup for Bit-Field and
// Driverlib development.
//
//#############################################################################
// $TI Release: F2837xD Support Library v3.08.00.00 $
// $Release Date: Mon Dec 23 17:32:30 IST 2019 $
// $Copyright:
// Copyright (C) 2013-2019 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"

void GPIO_use_init(void);
__interrupt void cpu_timer0_isr(void);
void initTIMER0(void);
//
// Main
//
void main(void)
{
    // Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    InitSysCtrl();
    // Initialize GPIO:
    // InitGpio - Sets all pins to be muxed to GPIO in input mode with pull-ups enabled.
    InitGpio();
    // Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    DINT;
    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags are cleared.
    InitPieCtrl();
    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    InitPieVectTable();
    // Several control registers are protected from spurious CPU writes
    // by the EALLOW protection mechanism.
    // EALLOW protection means that the write access to the register will be enabled
    // only when the EALLOW instruction has been executed prior to the write access.
    // The complementary EDIS instruction disables access to all registers protected
    // in this way.
    EALLOW; // This is needed to write to EALLOW protected registers
    // Re-map ISR functions,  Assigning the reference on custom function
    // for TIMER0 interrupt
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    EDIS; // This is needed to disable write to EALLOW protected registers
    // Initialize used GPIO:
    GPIO_use_init();
    // Initialize the timer0:
    initTIMER0();
    // Enable global Interrupts and higher priority real-time debug events:
    IER |= (M_INT1);
    // Enable TINT0 in the PIE: Group 1 __interrupt 7
    // (Local Peripheral Interrupt Enable Register)
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    // Enable Global __interrupt INTM
    EINT;
    // Enable Global realtime __interrupt DBGM
    ERTM;
    // Empty loop
    for(;;) {
    }
}

void GPIO_use_init(void)
{
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0;   // Enable pullup on GPIO
    GpioDataRegs.GPASET.bit.GPIO0 = 1;   // Load output latch
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;  // Pin = GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;   // GPIO = output

    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;   // Enable pullup on GPIO
    GpioDataRegs.GPASET.bit.GPIO31 = 1;   // Load output latch
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;  // Pin = GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;   // GPIO = output

    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;   // Enable pullup on GPIO
    GpioDataRegs.GPBSET.bit.GPIO34 = 1;   // Load output latch
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;  // Pin = GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;   // GPIO = output
    EDIS;
}

void initTIMER0(void)
{
    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0, 200, 500000);
    CpuTimer0Regs.TCR.all = 0x4001;
}

__interrupt void cpu_timer0_isr(void)
{
    // Incrementing timer counter for user purposes. Why not?
    CpuTimer0.InterruptCount++;
    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
    static int16 i = 0;
    if (i%3 == 2) {
        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    }
    i++;
    // Acknowledge this __interrupt to receive more __interrupts from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
//
// End of File
//

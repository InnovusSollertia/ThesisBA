
#include "driverlib.h"

//******************************************************************************
// ACLK = 32768Hz, configure MCLK = 8MHz, SMCLK=8MHz
//
//                MSP430FR6989
//             -----------------
//            |                 |--LFXTIN (32768Hz reqd.)
//            |                 |--LFXTOUT
//            |                 |
//       RST -|     P2.0/UCA0TXD|----> irDA module
//            |                 |
//            |                 |
//            |     P2.1/UCA0RXD|<---- irDA module
//            |                 |
//
//
//******************************************************************************


#define UART_TX_PORT                        (GPIO_PORT_P3)
#define UART_TX_PIN                         (GPIO_PIN5)
#define UART_RX_PORT                        (GPIO_PORT_P3)
#define UART_RX_PIN                         (GPIO_PIN4)


uint8_t returnValue = 0;        //Variable to store returned STATUS_SUCCESS or STATUS_FAIL

uint8_t receivedData = 0x00;
uint8_t receiveDataCount = 0x00;

uint8_t Button_p1_1=0X1;


void Port_Init(void);
void clockSystemConfig(void);
void Init_UART_A0(void);
void Init_UART_A1(void);

void Init_IrDA (void);

void main (void)
{
    //Stop WDT
    WDT_A_hold(WDT_A_BASE);


         Port_Init();
         clockSystemConfig();


        Init_UART_A1(); // serial port to pc

        //  Init_UART_A0(); //irda
        Init_IrDA();
        __bis_SR_register(GIE);
        /*
         *  I must press p1.1 to transmit a word through interrupt
         */
                       // Configure button S1 (P1.1) interrupt
                       GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);
                       GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);
                       GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN1);
                       GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);


        while(1)
        {
            /*
             * un comment lines below if you want to test that Irda emits
             * and serial port to Pc has no errors
             */
          //EUSCI_A_UART_transmitData(EUSCI_A1_BASE , 'M');   // for pc
          //  EUSCI_A_UART_transmitData(EUSCI_A0_BASE , 'k');   // alternative port  and irDA
          //  __delay_cycles(80000);
          // EUSCI_A_UART_transmitData(EUSCI_A0_BASE , '#');   // alternative port  and irDA
          //  __delay_cycles(80000);
        }
 }

//******************************************************************************
//
//Function definition .
//
//******************************************************************************


// Port 1 interrupt service routine for push button of the main board
void __attribute__ ((interrupt(PORT1_VECTOR))) PORT1_ISR (void)
{


    P1IFG &= ~BIT1;
    // ********************************************


   EUSCI_A_UART_transmitData(EUSCI_A0_BASE , '#');  // irDA
   Button_p1_1=GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1);


//  ********************************************
    _low_power_mode_off_on_exit();
}

void Port_Init()
{

    // Configure all Ports as output and drive all pins low
        GPIO_setAsOutputPin(GPIO_PORT_P1,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_P1,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setAsOutputPin(GPIO_PORT_P2,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_P2,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setAsOutputPin(GPIO_PORT_P3,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_P3,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setAsOutputPin(GPIO_PORT_P4,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_P4,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setAsOutputPin(GPIO_PORT_P5,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_P5,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setAsOutputPin(GPIO_PORT_P6,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_P6,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );


        GPIO_setAsOutputPin(GPIO_PORT_P7,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_P7,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );


        GPIO_setAsOutputPin(GPIO_PORT_P8,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_P8,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );


        GPIO_setAsOutputPin(GPIO_PORT_P9,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_P9,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setAsOutputPin(GPIO_PORT_P10,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_P10,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setAsOutputPin(GPIO_PORT_PJ,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );

        GPIO_setOutputLowOnPin(GPIO_PORT_PJ,
                            GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 |
                            GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7 );


        // Configure PJ.4 and PJ.5 as input pins for LFXIN and LFXOUT mode

        GPIO_setAsPeripheralModuleFunctionInputPin(
                GPIO_PORT_PJ,
                GPIO_PIN4 | GPIO_PIN5,
                GPIO_PRIMARY_MODULE_FUNCTION
        );



        PM5CTL0 &= ~LOCKLPM5;  // In initialization, the I/Os are configured before unlocking the I/O ports
}

void clockSystemConfig(void)
{

    // Set DCO Frequency to 8 MHz
        CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);

        //configure MCLK = 8MHz, SMCLK=8MHz
        //Set DCO Frequency to 8MHz
        CS_setDCOFreq(CS_DCORSEL_0, CS_DCOFSEL_6);
        //configure MCLK, SMCLK to be source by DCOCLK
        CS_initClockSignal(CS_SMCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);
        CS_initClockSignal(CS_MCLK,CS_DCOCLK_SELECT,CS_CLOCK_DIVIDER_1);

        // ACLK Clock configuration

        //Set external clock frequency to ACLK frequency
        CS_setExternalClockSource(32768,0);


        CS_initClockSignal(CS_ACLK,CS_LFXTCLK_SELECT,CS_CLOCK_DIVIDER_1);
        CS_turnOnLFXT(CS_LFXT_DRIVE_3);


        // Disable the GPIO power-on default high-impedance mode to activate
        // previously configured port settings

        PMM_unlockLPM5();

}



void Init_UART_A0(void)
{
/**********UART 1 --> EUSCI_A0__________Start*****************/
/*
 *    FOR IRDA
 *    Configure UART 57600
 */
    // Configure USCI_A1 for UART mode
    // Configure UART
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 8;
    param.firstModReg = 10;
    param.secondModReg = 247;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling =1;   // EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;
    if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &param)) {
    return;
    }
    EUSCI_A_UART_enable(EUSCI_A0_BASE);

    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt



    //P2.0,1 = USCI_A0 TXD/RXD
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P2,
        GPIO_PIN0 + GPIO_PIN1,GPIO_PRIMARY_MODULE_FUNCTION
        );

    //Enable UART module for operation
    EUSCI_A_UART_enable(EUSCI_A0_BASE);

    //---------Rx_Interrupt-----------------
        //Enable Receive Interrupt
        EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE,
        EUSCI_A_UART_RECEIVE_INTERRUPT
        );
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE,
        EUSCI_A_UART_RECEIVE_INTERRUPT
        );
        //------------------------------------

/**********UART 1 --> USCI_A0___________End******************/

}

void Init_UART_A1(void){

/**********UART 2 --> USCI_A1__________Start*****************/
    // FOR PC
    // Configure UART 57600

      // Configure the selected eUSCI UART (UCAx) via DriverLib calls
      // Enable UART receive interrupts.  Bytes received over UART go
      // into the UART Rx Queue (g_ReceiveQueue);
      GPIO_setAsPeripheralModuleFunctionInputPin(
              UART_TX_PORT,
              UART_TX_PIN,
              GPIO_PRIMARY_MODULE_FUNCTION);

      GPIO_setAsPeripheralModuleFunctionInputPin(
              UART_RX_PORT,
              UART_RX_PIN,
              GPIO_PRIMARY_MODULE_FUNCTION);


      // Configure UART
      EUSCI_A_UART_initParam param = {0};
      param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
      param.clockPrescalar = 8;
      param.firstModReg = 10;
      param.secondModReg = 247;
      param.parity = EUSCI_A_UART_NO_PARITY;
      param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
      param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
      param.uartMode = EUSCI_A_UART_MODE;
      param.overSampling =1;   // EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;
      if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A1_BASE, &param)) {
      return;
      }
      EUSCI_A_UART_enable(EUSCI_A1_BASE);


      UCA1CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
      UCA1IE |= UCRXIE;                         // Enable USCI_A1 RX interrupt


        //Enable UART module for operation
    EUSCI_A_UART_enable(EUSCI_A1_BASE);

        //---------Rx_Interrupt-----------------
        //Enable Receive Interrupt
        EUSCI_A_UART_clearInterrupt(EUSCI_A1_BASE,
        EUSCI_A_UART_RECEIVE_INTERRUPT
        );
    EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE,
        EUSCI_A_UART_RECEIVE_INTERRUPT
        );
        //------------------------------------


/**********UART 2 --> USCI_A1__________End*****************/

}


void Init_IrDA (void)
{
        //P2.0,1 = USCI_A0 TXD/RXD
    GPIO_setAsPeripheralModuleFunctionInputPin(
        GPIO_PORT_P2,
        GPIO_PIN0 + GPIO_PIN1,GPIO_PRIMARY_MODULE_FUNCTION
        );

        // Enable IrDA
    UCA0IRCTL |= UCIREN;

        // Disable eUSCI_A
    EUSCI_A_UART_disable(EUSCI_A0_BASE);                        // Set UCSWRST bit

        // IrDA encoder/decoder enabled
    HWREG16(EUSCI_A0_BASE + OFS_UCAxIRTCTL) |= UCIREN;

        // Transmit pulse clock source BITCLK16 (16 * 1/16 clock fractions)
   // HWREG16(USCI_A1_BASE + OFS_UCAxMCTLW) |= UCOS16;           // Oversampling mode enabled
    HWREG16(EUSCI_A0_BASE + OFS_UCAxIRTCTL) |= UCIRTXCLK;       // Select BITCLK16


        // Transmit pulse length 3/16 bit period (6 half clock cycles)
    // Set UCIRTXPLx to 5
    HWREG16(EUSCI_A0_BASE + OFS_UCAxIRTCTL) |= UCIRTXPL2 | UCIRTXPL0;    // b101


    // Configure UART 57600
    EUSCI_A_UART_initParam param = {0};
    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;
    param.clockPrescalar = 8;
    param.firstModReg = 10;
    param.secondModReg = 247;
    param.parity = EUSCI_A_UART_NO_PARITY;
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling =1;   // EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION;
    if (STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A0_BASE, &param)) {
    return;
    }
    EUSCI_A_UART_enable(EUSCI_A0_BASE);


    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt


        //---------Rx_Interrupt-----------------
        //Enable Receive Interrupt
        EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE,
        EUSCI_A_UART_RECEIVE_INTERRUPT
        );
    EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE,
        EUSCI_A_UART_RECEIVE_INTERRUPT
        );

}


//******************************************************************************
//
//This is the USCI_A0 interrupt vector service routine.
//
//******************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void USCI_A0_ISR (void)
{
    switch (__even_in_range(UCA0IV,4)){
        //Vector 2 - RXIFG
        case 2:

            //Receive the data from Irda
            receivedData = EUSCI_A_UART_receiveData(EUSCI_A0_BASE);

            //Send data back "echo" to pc serial
            EUSCI_A_UART_transmitData(EUSCI_A1_BASE,
            receivedData
            );

            break;
        default: break;
    }
}


//******************************************************************************
//
//This is the USCI_A1 interrupt vector service routine.
//
//******************************************************************************

// PC PORT

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A1_VECTOR)))
#endif
void USCI_A1_ISR (void)
{
    switch (__even_in_range(UCA1IV,4)){
        //Vector 2 - RXIFG
        case 2:

            //Receive the data
            receivedData = EUSCI_A_UART_receiveData(EUSCI_A1_BASE);

            //Send data back "echo"
            EUSCI_A_UART_transmitData(EUSCI_A1_BASE,
            receivedData
            );

            break;
        default: break;
    }
}

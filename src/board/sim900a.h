
#ifndef __SIM900A_H
#define __SIM900A_H


/*---------------------- Includes ---------------------------------------------*/
// #include <includes.h>

// The Command line buffer can accept a maximum of 556 characters. If the characters 
// entered exceeded this number then none of the Command will executed and 
// TA will return "ERROR".

/*

flow control
There are basically two approaches to achieve data flow control: software flow control and hardware flow control. SIM900 support both two kinds of flow control.
In Multiplex mode, it is recommended to use the hardware flow control.

1)Hardware flow control (RTS/CTS flow control)
2)Software flow control (XON/XOFF flow control)

AT+IFC=1, 1
This setting is stored volatile, for use after restart, AT+IFC=1, 1 should be stored to the user profile with AT&W.

NOTE:
The AT commands listed in the table of AT&W chapter should be stored to user profile with AT&W for use after restart. Most other AT commands in V.25, 07.05, 07.07, GPRS will store parameters automatically and can be used after module restart.

*/

/*---------------------- Definition -------------------------------------------*/
#define GSM_IMEI_LEN       15           // .
#define GSM_SIM_ID_LEN     20           // .



#endif

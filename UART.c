/* Example code demonstrating the use of the hardware UART on the MSP430G2553 to receive
 * and transmit data back to a host computer over the USB connection on the MSP430
 * launchpad.
 * Note: After programming it is necessary to stop debugging and reset the uC before
 * connecting the terminal program to transmit and receive characters.
 * This demo will turn on the Red LED if an R is sent and turn it off if a r is sent.
 * Similarly G and g will turn on and off the green LED
 * It also transmits the received character back to the terminal.
 */

#include <msp430.h>
#include <stdint.h>

void UARTSendArray(uint8_t *TxArray){
 // Send number of bytes Specified in ArrayLength in the array at using the hardware UART 0
 // Example usage: UARTSendArray("Hello", 5);
 // int data[2]={1023, 235};
 // UARTSendArray(data, 4); // Note because the UART transmits bytes it is necessary to send two bytes for each integer hence the data length is twice the array length

while(*TxArray != 0){ // Loop until StringLength == 0 and post decrement
 while(!(IFG2 & UCA0TXIFG)); // Wait for TX buffer to be ready for new data
 UCA0TXBUF = *TxArray; //Write the character at the location specified py the pointer
 TxArray++; //Increment the TxString pointer to point to the next character
 }

while(!(IFG2 & UCA0TXIFG));
UCA0TXBUF = 10;
UCA0TXBUF = 13;
}

void Convert_to_Decimal(uint8_t Digits, uint8_t* Decimal)
{
	int i;

	for(i=0;i<3;i++)
	{
		Decimal[2-i] = Digits%10 + 48;		//Make a number, pull out the little digit
		Digits = Digits/10;					//Reduce by factor of ten for next
	}

}

void Convert_to_Hex(uint8_t* Dest, uint8_t* Payload, uint8_t Length)
{
	uint8_t Byte;
	uint8_t UH = 0;
	uint8_t LH = 0;

	int i;

	for(i=0;i<Length;i++)
	{
		Byte = Payload[i];
		UH = Byte>>4;
		if(UH < 10)
		{
			Dest[3*i] = UH + '0';
		}
		else
		{
			Dest[3*i] = UH + 'A' - 10;
		}

		LH = Byte & 0xF;

		if(LH < 10)
		{
			Dest[3*i + 1] = LH + '0';
		}
		else
		{
			Dest[3*i + 1] = LH + 'A' - 10;
		}
		Dest[3*i+2] = ' ';
	}
	Dest[3*i] = 0;

}


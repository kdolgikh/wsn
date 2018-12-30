/*
 * Clustering.c
 *
 *  Created on: Nov 23, 2015
 *      Author: Bharath
 */

#include <MSP430.h>
#include "pinDefs.h" // Pin defines
#include "Radio.h"
#include "states.h" // Enum for the possible machine states.
#include "sleep_timer.h"

#define Cluster_Stride 10 * _Second
#define Guard_Time _Second/2
#define WaitMultiplier 128

extern uint8_t _MyAddress;
extern uint8_t _ChildList[];
extern uint8_t _ChildCount;
extern uint8_t __MyHead;

// Function to interpret the RSSI values from the radio
int16_t RSSIdB(uint8_t RSSI_dec);

/**
 *\brief Function to flood messages so that nodes can take RSSI measurements of each other.
 *
 * This function makes the node alternate between sending a message and holding the radio in listen mode.
 * A long message of alternating 0's and 1's is sent at some multiple of the nodes address.
 * Using the address as a multiplier helps to spread the messages out and reduce collisions.
 * The idea is that the message transmit time is short compared to the period with which messages are sent,
 * collisions are inherently unlikely and each node is sending 5 messages, so the loss of any individual message is
 * unlikely to be relevant.
 *
 * @return Returns the address of the node with the highest measured RSSI
 */
uint8_t Flood_RSSI() {

	// Lotsa 01's for good RSSI measure
	uint8_t message[51] = { RSSI_Flood, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
			0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
			0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
			0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
			0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };




	volatile struct Listen_Struct D;
	volatile int16_t TopRSSI = 0;
	volatile uint8_t TopAddy = 0;
	volatile uint32_t EndTime;
	volatile uint32_t CurTime;
	volatile uint32_t StartTime;
	volatile uint16_t WaitTime = _MyAddress * WaitMultiplier;
	volatile uint16_t CurRSSI;

	uint8_t Got_Message = 0;	// Set this variable to track whether or not a message was heard at all
	uint8_t i;

	CurTime = Get_Time();	// Measure the current time
	EndTime = CurTime + Cluster_Stride;	// Mark the end time
	StartTime = CurTime + Guard_Time;	// Dont' start trying to transmit until listening for a brief period.


	D = LBT_Listen(StartTime - CurTime);	// Listen for the guard period

	if (D.Status == Message_Recieved) {		// If a message is recieved check if the RSSI is higher than the existing measurement
		CurRSSI = RSSIdB(D.signal);

		if (CurRSSI > TopRSSI) {
			TopRSSI = CurRSSI; //Storing the RSSI values in a array
			TopAddy = D.address;
			Got_Message = 1;	// Set flag indicating a message was received
		}
	}

	for (i = 0; i < 5; i++) {	// Try to send 5 times
		TDMA_Send(0xFF, _MyAddress, message, 51);	// Send the message
		CurTime = Get_Time();	// Get the time
		StartTime = CurTime + WaitTime;	// Mark the time where the next message should be sent

		while (CurTime < StartTime) {	// Keep listening until it's time to send again

			D = LBT_Listen(StartTime - CurTime);	// Listen for alotted time

			if (D.Status == Message_Recieved) {	// Check if a message was received
				CurRSSI = RSSIdB(D.signal);
				if (CurRSSI > TopRSSI) {
					TopRSSI = CurRSSI; 		//Storing the RSSI values in a array
					TopAddy = D.address;
					Got_Message = 1;		// Set the flag if we got a message
				}
			}
			CurTime = Get_Time();	// Update the time measurement
		}
	}

	// All transmissions are done, now just listen until the end of this stage
	while (CurTime < EndTime) {
		D = LBT_Listen(EndTime - CurTime);
		if (D.Status == Message_Recieved) {
			CurRSSI = RSSIdB(D.signal);
			if (CurRSSI > TopRSSI) {
				TopRSSI = CurRSSI;
				TopAddy = D.address;
				Got_Message = 1;
			}
		}
		CurTime = Get_Time();
	}

	// If not a single message was recieved, then turn on the light and spin for debugging
	if (!Got_Message) {
		LED1Dir |= LED1;
		LED1Reg |= LED1;
		while (1) {
		}	// Gonna be here a bit
	}
	return TopAddy;
}

/**
 * \breif This function sends out votes for the optimal cluster head, as chosen by this node.
 *
 * This function works similarly to the Flood_RSSI function. It alternately talks and listens,
 * sending the message 5 times to ensure that the node will hear at least one of them. Multiple
 * votes are still counted, so in deal cases a potential CH will get 5 votes from each node electing it.
 *
 * @param Best_Addy The address of the best choice for cluster head, according to this node.
 * @return Returns the number of times this node was nominated
 */

uint8_t Nominate(uint8_t Best_Addy) {
	uint8_t Nom_Count = 0;
	uint8_t message[2];
	uint32_t StartTime;
	uint32_t CurTime;
	volatile struct Listen_Struct D;
	uint32_t EndTime;
	uint8_t i;
	uint16_t WaitTime = _MyAddress * WaitMultiplier;

	CurTime = Get_Time();	// Record time of entry to this function
	EndTime = CurTime + Cluster_Stride;	// Mark the end time for this step of the clustering process
	StartTime = CurTime + Guard_Time;	// Mark off a start time to allow for some guard buffer

	message[0] = RSSI_Nominate;		// Create the message, we will send an opcode followed by the address we're voting for
	message[1] = Best_Addy;


	D = LBT_Listen(StartTime - CurTime);	// Listen for the guard time
	if (D.Status == Message_Recieved) {
		if (D.payload[1] == _MyAddress && D.payload[0] == RSSI_Nominate) {
			Nom_Count++;	// If we get an appropriate message up the nomination count
		}
	}

	for (i = 0; i < 5; i++) {	// Try to send 5 times
		CurTime = Get_Time();
		StartTime = CurTime + WaitTime;

		TDMA_Send(0xFF, _MyAddress, message, 2);

		while (CurTime < StartTime) {
			D = LBT_Listen(WaitTime);

			if (D.Status == Message_Recieved) {
				if (D.payload[1] == _MyAddress
						&& D.payload[0] == RSSI_Nominate) {
					Nom_Count++;
				}
			}
			CurTime = Get_Time();
		}

	}

	while (CurTime < EndTime) {
		D = LBT_Listen(EndTime - CurTime);
		if (D.Status == Message_Recieved) {
			if (D.payload[1] == _MyAddress && D.payload[0] == RSSI_Nominate) {
				Nom_Count++;
			}
		}
		CurTime = Get_Time();
	}

	return Nom_Count;
}

/**
 * \breif This function compares nomination counts with nearby nodes.
 *
 * Here the node alternates between transmitting and receiving vote counts. It sends it's own vote count
 * and listens to the vote counts of other nodes. If a node hears a vote count higher than its own then
 * it will disqualify itself as a cluster head.
 *
 * @param nom_count The number of times this node was nominated
 * @return	0 if the node is not a cluster head, 1 if it is
 */

uint8_t Compare(uint8_t nom_count) {
	uint8_t message[2];
	struct Listen_Struct D;
	uint32_t EndTime;
	uint32_t CurTime;
	uint32_t StartTime;
	uint16_t WaitTime = _MyAddress * WaitMultiplier;
	uint8_t GotMessage = False;

	uint8_t i;
	uint8_t Selected_Head = 1;

	CurTime = Get_Time();	// Mark the stop time as 2 seconds from now
	EndTime = CurTime + Cluster_Stride * 3;
	StartTime = CurTime + Guard_Time;

	message[0] = Cluster_Compare;
	message[1] = nom_count;

	D = LBT_Listen(StartTime - CurTime);
	if (D.Status == Message_Recieved) {
		if (D.payload[0] == Cluster_Compare) {
			if (D.payload[1] > nom_count) {
				Selected_Head = 0;
			}
			GotMessage = True;
		}
	}

	GotMessage = True;

	for (i = 0; i < 10; i++) {
		TDMA_Send(0xFF, _MyAddress, message, 2);
		D = LBT_Listen(WaitTime);
		if (D.Status == Message_Recieved) {
			if (D.payload[0] == Cluster_Compare) {
				if (D.payload[1] > nom_count) {
					Selected_Head = 0;
				}
				GotMessage = True;
			}
		}
	}

	CurTime = Get_Time();

	while (CurTime < EndTime) {
		D = LBT_Listen(EndTime - CurTime); //Listen for
		if (D.Status == Message_Recieved) {
			if (D.payload[0] == Cluster_Compare) {
				if (D.payload[1] > nom_count) {
					Selected_Head = 0;
				}
				GotMessage = True;
			}
		}

		GotMessage = True;
	}
	CurTime = Get_Time();

	if (!GotMessage) {
		Selected_Head = False;
	}

	return Selected_Head;
}

/**
 * \brief Functions where cluster heads advertise themselves to children
 *
 * Unlike the previous steps, nodes no longer need to alternate between listening and talking.
 * The child nodes are all in listen mode throughout this entire step and the heads broadcast 5
 * times so that children can determine who their cluster head should be.
 */
void CH_Broadcast() {
	uint8_t message = Cluster_Advertise;
	uint8_t i;
	uint32_t EndTime;
	uint32_t CurTime;
	uint16_t WaitTime = _MyAddress * WaitMultiplier;

	Sleep_Timer(Guard_Time);	// Wait a bit before broadcasting

	CurTime = Get_Time();
	EndTime = CurTime + Cluster_Stride;

	for (i = 0; i < 5; i++) {
		TDMA_Send(__MyHead, _MyAddress, &message, 1);
		Sleep_Timer(WaitTime);
	}

	CurTime = Get_Time();

	Sleep_Timer(EndTime - CurTime);
}

/**
 * \brief Gathers a list of the child nodes attached to this cluster head
 *
 * Here the cluster head sits in listen mode waiting for the children to broadcast to it and attach themselves.
 * The head doesn't need to transmit anything at this time, it is simply waiting for messages instructing it to
 * add children to its list.
 *
 * @param out_Children The array to store the child nodes into
 * @return	The number of child nodes loaded into the output array
 */

uint8_t CH_Listen(uint8_t *out_Children) {
	uint32_t EndTime;
	uint32_t CurTime;
	uint8_t ChildCount = 0;
	uint8_t i;
	struct Listen_Struct D;

	CurTime = Get_Time();
	EndTime = CurTime + Cluster_Stride * 3;

	while (CurTime < EndTime) {
		D = LBT_Listen(EndTime - CurTime);
		if (D.Status == Message_Recieved) {
			if (D.payload[0] == Cluster_Join)// If correct message type add child to list
					{
				uint8_t NewChild = 1;
				for (i = 0; i < ChildCount; i++) {
					if (out_Children[i] == D.address) {
						NewChild = 0;
					}
				}

				if (NewChild) {
					out_Children[ChildCount] = D.address;
					ChildCount++;

				}
			}
		}
		CurTime = Get_Time();
	}

	return ChildCount;
}

/**
 * \brief Directs a node to listen for an attach beacon from a cluster head.
 *
 * This function is called after the cluster heads are elected. At this time the cluster heads will
 * begin to send beacons to the nodes so that they can pick the best cluster head for them. The child node
 * listens to all of the cluster head becons it hears, and selects the highest RSSI beacon to be its cluster head.
 *
 * @return The address of the chosen cluster head
 */

uint8_t Child_Listen() {
	uint32_t EndTime;
	uint32_t CurTime;
	uint8_t BestHead = 0;
	uint8_t HeadRSSI = 0;
	struct Listen_Struct D;

	CurTime = Get_Time();
	EndTime = CurTime + Cluster_Stride;

	while (CurTime < EndTime) {
		D = LBT_Listen(EndTime - CurTime);

		if (D.Status == Message_Recieved) {
			if (D.signal > HeadRSSI && D.payload[0] == Cluster_Advertise) {
				HeadRSSI = D.signal;
				BestHead = D.address;
			}
		}
		CurTime = Get_Time();
	}

	return BestHead;
}

/**
 * \brief Sends a beacon to the nodes cluster head so the head can add the node to its list of children
 *
 * This function is called during the last stage of clustering. Here the cluster heads are listening and waiting for
 * child nodes to send a beacon. The child node does not need to listen during this time, it only needs to send
 * a message to the cluster head so that it knows to add the node to its list of children.
 */

void Child_Broadcast() {
	uint8_t message = Cluster_Join;
	uint8_t i;
	uint32_t EndTime;
	uint32_t CurTime;
	uint16_t WaitTime = _MyAddress * WaitMultiplier;

	Sleep_Timer(Guard_Time);	// Wait a bit before broadcasting

	CurTime = Get_Time();
	EndTime = CurTime + Cluster_Stride * 3;

	for (i = 0; i < 10; i++) {
		TDMA_Send(__MyHead, _MyAddress, &message, 1);
		Sleep_Timer(WaitTime);
	}

	CurTime = Get_Time();

	Sleep_Timer(EndTime - CurTime);
}

/**
 * \brief Function to run the clustering sequence up to the point where cluster heads are chosen.
 *
 * The first 3 steps in the clustering process are performed here. This function will return the
 * next state the machine should go to, which will depend on whether or not it is a cluster head.
 *
 * @return The next machine state to move into, either CH_Associate or Associate
 */

Machine_State Cluster_Choose() {
	uint8_t CH_Buf;

	CH_Buf = Flood_RSSI();
	CH_Buf = Nominate(CH_Buf);
	CH_Buf = Compare(CH_Buf);

	if (CH_Buf)		// True if node is a CH
	{
		return CH_Associate;
	} else {
		return Associate;
	}

}

/**
 * \brief Runs the cluster heads through the process of acquiring their nodes.
 *
 * Here the cluster head will first transmit some beacons so that the other nodes can
 * compare it to other cluster heads and pick one to attach to. After exiting this function
 * the two global variables containing the list of child nodes and the count of child nodes
 * will be populated.
 *
 * @return The next machine state, CH_Sense
 */

Machine_State CH_Cluster_Associate() {
	CH_Broadcast();
	_ChildCount = CH_Listen(_ChildList);	// Gets the list of children
	Sleep_Timer(6000);// Make sure the child nodes get ready before the CH does.
	return CH_TDMA_Assignment;
}

/**
 * \brief Runs a child node through attaching to a cluster head
 *
 * Calls the two functions to have the child node listen and pick a cluster head, as
 * well as send a message to the chosen head informing it of its new child.
 *
 * @return The next machine state, Sense
 */

Machine_State Child_Associate() {
	__MyHead = Child_Listen();
	Child_Broadcast();

	return TDMA_Assignment;
}

/**
 * \brief Interprets an RSSI byte appended by the CC110l to the packet.
 *
 * The radio appends an RSSI value to the packets if the appropriate option is set, but
 * the RSSI value requires interpretation. This function converts the given value into
 * a dBm value.
 *
 * @param RSSI_dec The RSSI byte from the radio
 * @return A dBm value representing the RSSI
 */

int16_t RSSIdB(uint8_t RSSI_dec) {
	uint16_t RSSI_dBm;
	uint8_t RSSI_offset = 74;
	//RSSI_dec = rssi;
	if (RSSI_dec >= 128) {
		RSSI_dBm = ((RSSI_dec - 256) / 2) - RSSI_offset;
	} else {
		RSSI_dBm = (RSSI_dec / 2) - RSSI_offset;
	}
	return RSSI_dBm;
}

/*
 * States.h
 *
 *  Created on: Nov 23, 2015
 *      Author: cgoss
 */

/**
 * \file States.h
 * \brief The contains the enumeration of possible states for the main control loop.
 */

#ifndef STATES_H_
#define STATES_H_

typedef enum {
	Waiting_For_Start, ///< Startup state, waiting for the signal to begin localization
	Localizing,        ///< Localizing state performs periodic pings so that the localization beacon can hear them
	Clustering,        ///< Begins the timed clustering process
	CH_Associate,	   ///< Cluster heads advertise to child nodes
	Associate,		   ///< Nodes pick their cluster heads
	CH_TDMA_Assignment,///< Cluster head state in which it assignes TDMA slots to child nodes
	CH_Sensing,        ///< Cluster head state, normal mode of operation
	TDMA_Assignment,   ///< Child node state, waiting for TDMA assignment from cluster head
	Sensing,           ///< Normal operation mode for child nodes.
	broken			   ///< Return this when nothing should be returned.
} Machine_State;

typedef enum{
	ClusterStart = 0x00,			///< Send this to move the nodes out of the waiting state and into clustering
	RSSI_Flood = 0x01,
	RSSI_Nominate = 0x02,
	Cluster_Compare = 0x03,
	Cluster_Advertise = 0x04,
	Cluster_Join = 0x05,
	TDMA = 0x06,
	CH_Poll = 0x07,
	ND_Data = 0x08,
	BS_Data = 0x09
} MessageType ;

#endif /* STATES_H_ */

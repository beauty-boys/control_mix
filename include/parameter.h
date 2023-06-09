/* ==============================================================================
System Name:  PMSM31

File Name:	PARAMETER.H

Description:	Parameters file for the Real Implementation of Sensored  
          		Field Orientation Control for a Three Phase Permanent-Magnet
          		Synchronous Motor (PMSM) using QEP sensor
          		
Originator:		Digital control systems Group - Texas Instruments

Note: In this software, the default inverter is supposed to be DMC1500 board. 
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20
=================================================================================  */

#ifndef PARAMETER_H
#define PARAMETER_H


/*-------------------------------------------------------------------------------
Next, definitions used in main file.
-------------------------------------------------------------------------------*/
#ifndef TRUE
#define FALSE 0
#define TRUE  1
#endif

#define PI 3.14159265358979
#define EPWM1_TIMER_TBPRD 10000
#define EPWM1_MIN_CMPA 500 //��ʼֵ
#define Sample_points 2048

#define Change_Point 0.75
#define Return_Point 0.55
// Define the system frequency (MHz)


// Define the ISR frequency (kHz)
#define ISR_FREQUENCY 20

// This machine parameters are based on 1-hp PacSci PMSM motor
// Define the PMSM motor parameters
#define RS 		0.9               		// Stator resistance (ohm)
#define RR   	0               		// Rotor resistance (ohm) 
#define LS   	0.00435     			// Stator inductance (H) 
#define LR   	0						// Rotor inductance (H) 	
#define LM   	0						// Magnetizing inductance (H)
#define P    	4						// Number of poles 

// Define the base quantites 
#define BASE_VOLTAGE    184.752       // Base peak phase voltage (volt) 
#define BASE_CURRENT    5             // Base peak phase current (amp) 
#define BASE_TORQUE     7.35105194    // Base torque (N.m) 
#define BASE_FLUX       0.79616       // Base flux linkage (volt.sec/rad)
#define BASE_FREQ      	100           // Base electrical frequency (Hz)

#define PWM_TIMER_TBPRD   0x0EA6  // 0xEA6=3750=150M/2/10k/2


#define Angle_pianyi PI/18
#define Angle_width PI/3






#endif  // end of PARAMETER.H definition

//===========================================================================
// No more.
//===========================================================================

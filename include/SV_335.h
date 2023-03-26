/* ==============================================================================
System Name:  PMSM31

File Name:	PMSM3_1.H

Description:	Primary system header file for the Real Implementation of Sensored  
          		Field Orientation Control for a Three Phase Permanent-Magnet
          		Synchronous Motor (PMSM) using QEP sensor

Originator:		Digital control systems Group - Texas Instruments

Note: In this software, the default inverter is supposed to be DMC1500 board. 
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20: Support both F280x and F281x targets 
 04-25-2005 Version 3.21: Move EINT and ERTM down to ensure that all initialization
 						  is completed before interrupts are allowed.
=================================================================================  */

#ifndef SV_335_H
#define SV_335_H



/*-------------------------------------------------------------------------------
Next, Include project specific include files.
-------------------------------------------------------------------------------*/
#include "park.h"       		// Include header for the PARK object 
#include "ipark.h"       		// Include header for the IPARK object 
#include "pid_reg3.h"       	// Include header for the PIDREG3 object 
#include "clarke.h"         	// Include header for the CLARKE object 
#include "svgen_dq.h"       	// Include header for the SVGENDQ object 
#include "az1gen_dp.h"
//#include "speed_fr.h"			// Include header for the SPEED_MEAS_QEP object 
#include "rampgen.h"        	// Include header for the RAMPGEN object 
#include "rmp_cntl.h"       	// Include header for the RMPCNTL object 
#include "angle.h"
//#include "dlog4ch.h" 
//#include "speed_pr.h"			// Include header for the SPEED_MEAS_CAP object 

//#if (DSP_TARGET==F2808)
//#include "f280x_en.h"        	// Include header for the DRIVE object 
//#include "f280xqep.h"        	// Include header for the QEP object 
//#include "f280xileg_vdc.h"      // Include header for the ILEG2DCBUSMEAS object
//#include "f280xpwm.h"        	// Include header for the PWMGEN object 
//#include "f280xpwmdac.h"		// Include header for the PWMDAC object 
//#endif

//#if (DSP_TARGET==F2812)
//#include "f281x_en.h"        	// Include header for the DRIVE object 
//#include "f281xqep.h"        	// Include header for the QEP object 
//#include "f281xileg_vdc.h" 		// Include header for the ILEG2DCBUSMEAS object 
//#include "f281xpwm.h"        	// Include header for the PWMGEN object 
//#include "f281xpwmdac.h"		// Include header for the PWMDAC object 
//#endif




#endif   // end of PMSM3_1.H definition

//===========================================================================
// No more.
//===========================================================================

typedef struct {         	  float32  Angle;	// Intput: angle
				  float32  Sine;        // Output: sin
				  float32  Cosine;	// Outnput: cos 
				  void  (*calc)();	// Pointer to calculation function 
				 } Angle;	            

typedef Angle *Angle_handle;

/*-----------------------------------------------------------------------------
Default initalizer for the IPARK object.
-----------------------------------------------------------------------------*/                     
#define ANGLE_DEFAULTS { 0, \
                          0, \
                          1, \
              			  (void (*)(Uint32))angle_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in IPARK.C
------------------------------------------------------------------------------*/
void angle_calc(Angle_handle);

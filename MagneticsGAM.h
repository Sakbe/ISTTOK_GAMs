#ifndef _MAGNETICSGAM_H
#define	_MAGNETICSGAM_H


//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"

OBJECT_DLL(MagneticsGAM)


class MagneticsGAM : public GAM, public HttpInterface {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		int ADC_magnetic_chopper_fp_0;
		int ADC_magnetic_chopper_fp_1;
		int ADC_magnetic_chopper_fp_2;
		int ADC_magnetic_chopper_fp_3;
		int ADC_magnetic_chopper_fp_4;
		int ADC_magnetic_chopper_fp_5;
		int ADC_magnetic_chopper_fp_6;
		int ADC_magnetic_chopper_fp_7;
		int ADC_magnetic_chopper_fp_8;
		int ADC_magnetic_chopper_fp_9;
		int ADC_magnetic_chopper_fp_10;
		int ADC_magnetic_chopper_fp_11;
		//Add signals from primary,vertical and horizontal currents channels 91,92,93
		float HorizontalCurrent;
		float VerticalCurrent;
		float PrimaryCurrent;
		int usectime;
	};
	struct OutputInterfaceStruct {
		float MagneticProbesR;
		float MagneticProbesZ;
		float MagneticProbesPlasmaCurrent;
		float ADC_magnetic_WO_corrctd_0;
		float ADC_magnetic_WO_corrctd_1;
		float ADC_magnetic_WO_corrctd_2;
		float ADC_magnetic_WO_corrctd_3;
		float ADC_magnetic_WO_corrctd_4;
		float ADC_magnetic_WO_corrctd_5;
		float ADC_magnetic_WO_corrctd_6;
		float ADC_magnetic_WO_corrctd_7;
		float ADC_magnetic_WO_corrctd_8;
		float ADC_magnetic_WO_corrctd_9;
		float ADC_magnetic_WO_corrctd_10;
		float ADC_magnetic_WO_corrctd_11;

	};

	uint saved_usectime;
	uint lasttime;
	float slope;
	float *lastmirnov;
	float *mirnovaccumulator;
	int accumulatorcounter;
	float buff;
	int k;
	int usectime_to_wait_for_starting_operation;
	bool magnetic_radial_bool;
	bool magnetic_vertical_bool;
	bool magnetic_module_correction_bool;
	int NumberOfProbes;
	float *magnetic_Angles;
	float *magnetic_Calibration;
	int NumberOfMeasurements;
	int *ProbeNumbers;
	int NumberOfModules;
	float *magnetic_Offset_slope;
	float *magnetic_Polarity_calibration;

	float *radial_coeficients;
	float *vertical_coeficients;
	float *magnetic_Offset_zero;
	float *ADC_values;
	float *ADC_WO;
	float *corrected_probes;

	float **slopes;
	float *slope_avrg;
	float *ADC_fact;
	float *ADC_WO_Wb;

	// Picked values of magneticflux in databases for substraction
	float *allmirnv_prim;
	float *allmirnv_hor;
	float *allmirnv_vert;
	

	// Arrays with mirnov magneticflux from primary, vertical and horizontal currents (database)
	float *primarydata; //[A]
	float *horizontaldata;
	float *verticaldata;
	float **mirnprim;
	
	float **mirnhor;

	float **mirnvert;
	

		////////////////////////////////////////////////////////////


	float adc18bit_conversion;
	
	float *points_x;
	float *points_y;
	float *m_x;
	float *m_y;
	float y_a;
	float y_b;
	float x_a;
	float x_b;
	float r_a;
	float r_b;
	float m_a;
	float m_b;
	float plasma_current_convertion_factor;

	int n_samples;
	float probe_radius, major_radius, clip_limit;
	float Area, Nvoltas, MAgPerm, ADCconst, Ncoils;
	float radial_position, vertical_position;
//	float rOffset, zOffset;
	float magnetic_field_sum;

	bool view_input_variables;

public:

	// Default constructor
	MagneticsGAM();

	// Destructor
	virtual ~MagneticsGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);
	
	virtual bool ProcessHttpMessage(HttpStream &hStream);
	
	

	OBJECT_DLL_STUFF(MagneticsGAM)
};



#endif	


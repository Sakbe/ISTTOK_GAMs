#ifndef _CONTROLLERGAM_H
#define	_CONTROLLERGAM_H

//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "File.h"
#include "Matrix.h"
#include "HtmlStream.h"
#include "IPID.h"
#include "IWaveform.h"

OBJECT_DLL(ControllerGAM)


class ControllerGAM : public GAM, public HttpInterface {
private:

	DDBInputInterface *SignalsInputInterface;
	DDBOutputInterface *SignalsOutputInterface;

	struct InputInterfaceStruct {
		float PrimaryCurrent;
		float HorizontalCurrent;
		float VerticalCurrent;
        float PrimaryOutputWaveform;
		float HorizontalOutputWaveform;
		float VerticalOutputWaveform;
		float PuffingOutputWaveform;
		float ToroidalOutputWaveform;
		float PlasmaCurrent;
		float PositionR;
		float PositionZ;
        float Density;
		float HAlpha;
		float InterferometryR;
		int32 PrimaryWaveformMode;
		int32 HorizontalWaveformMode;
		int32 VerticalWaveformMode;
		int32 usecTime;
		int32 DischargeStatus;
	};
	struct OutputInterfaceStruct {
		float SendToHorizontalValue;
		float SendToVerticalValue;
		float SendToPrimaryValue;
		float SendToPuffing;
		float SendToToroidal;
	};
		
	int old_PrimaryWaveformMode;
	int old_HorizontalWaveformMode;
	int old_VerticalWaveformMode;
	
	float temp_current;
	float temp_requested_output;

	IPID *horizontal_position_PID;
	IPID *vertical_position_PID;
	IPID *primary_plasma_current_PID;

	int horizontal_lookuptable_size;
	int vertical_lookuptable_size;
	int primary_lookuptable_size;

	float maximum_horizontal_position;		//in mm
	float minimum_horizontal_position;
	float maximum_vertical_position;
	float minimum_vertical_position;
	float maximum_plasma_current;			//in A
	float minimum_plasma_current;
	float maximum_horizontal_current;
	float minimum_horizontal_current;
	float maximum_vertical_current;
	float minimum_vertical_current;
	float maximum_primary_current;
	float minimum_primary_current;
	float maximum_toroidal_current;
	float minimum_toroidal_current;
	float maximum_puffing_output;		// in percentage
	float minimum_puffing_output;
	float maximum_density_halpha_scenario;				//density x10 ^ 18  //Halpha was multiplied by 100
	float minimum_density_halpha_scenario;
	
	float puffing_duration_in_puffing_feedback_in_ms;
	float maximum_idle_time_in_puffing_feedback_in_ms;
	float minimum_idle_time_in_puffing_feedback_in_ms;
	float puffing_feedback_usec_change_percentage_by_cycle;
	int puffing_feedback_last_usectime;
	int puffing_feedback_usectime_to_change;
	bool puffing_feedback_currently_off;				//0 -> puffing feedback on, waiting to turn off, 1 -> puffing feedback off, waiting to turn on
	int puffing_feedback_mode;							//1 -> puffing feedback in Density, 2 -> puffing feedback in HAlpha
	int puffing_feedback_usec_change_per_cycle;
	int puffing_duration_in_puffing_feedback_in_us;
	int maximum_idle_time_in_puffing_feedback_in_us;
	int minimum_idle_time_in_puffing_feedback_in_us;
	int old_DischargeStatus;

	MatrixT<float> A_matrix;
	MatrixT<float> B_matrix;
	MatrixT<float> C_matrix;
	MatrixT<float> D_matrix;
	
	int A_matrix_dims[2];
	int B_matrix_dims[2];
	int C_matrix_dims[2];
	int D_matrix_dims[2];
	int temp_max_dim;
	
	int usecthread_cycle_time;
	float PID_time_constant;
	bool interferometry_radial_control_bool;
	float PID_horizontal_proportional_soft;
	float PID_horizontal_proportional_normal;
	float PID_horizontal_proportional_hard;
	float PID_horizontal_integral_soft;
	float PID_horizontal_integral_normal; 
	float PID_horizontal_integral_hard;
	float PID_horizontal_derivative_soft;
	float PID_horizontal_derivative_normal;
	float PID_horizontal_derivative_hard;
	float PID_vertical_proportional_soft;
	float PID_vertical_proportional_normal;
	float PID_vertical_proportional_hard;
	float PID_vertical_integral_soft;
	float PID_vertical_integral_normal;
	float PID_vertical_integral_hard;
	float PID_vertical_derivative_soft;
	float PID_vertical_derivative_normal;
	float PID_vertical_derivative_hard;
	float PID_primary_proportional_soft;
	float PID_primary_proportional_normal;
	float PID_primary_proportional_hard;
	float PID_primary_integral_soft;
	float PID_primary_integral_normal;
	float PID_primary_integral_hard;
	float PID_primary_derivative_soft;
	float PID_primary_derivative_normal;
	float PID_primary_derivative_hard;

	bool view_input_variables;
	
	int puffing_mode;
	

public:

	// Default constructor
	ControllerGAM();

	// Destructor
	virtual ~ControllerGAM();

	// Initialise the module
	virtual bool Initialise(ConfigurationDataBase& cdbData);

	// Execute the module functionalities
	virtual bool Execute(GAM_FunctionNumbers functionNumber);

	virtual bool ProcessHttpMessage(HttpStream &hStream);

	OBJECT_DLL_STUFF(ControllerGAM)
};



#endif


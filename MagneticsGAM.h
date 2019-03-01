#ifndef _MAGNETICSGAM_H
#define	_MAGNETICSGAM_H


//#include <dirent.h>
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"
#include "GAM.h"
#include "HtmlStream.h"
#include "Matrix.h"

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
		// Mirnov coil measurements without the effect from the external fields
		float Magnetics_flux_corrctd_0;
		float Magnetics_flux_corrctd_1;
		float Magnetics_flux_corrctd_2;
		float Magnetics_flux_corrctd_3;
		float Magnetics_flux_corrctd_4;
		float Magnetics_flux_corrctd_5;
		float Magnetics_flux_corrctd_6;
		float Magnetics_flux_corrctd_7;
		float Magnetics_flux_corrctd_8;
		float Magnetics_flux_corrctd_9;
		float Magnetics_flux_corrctd_10;
		float Magnetics_flux_corrctd_11;
		// Extracted signals reconstructed from the PS measurements
		float Magnetics_ext_flux_0;
		float Magnetics_ext_flux_1;
		float Magnetics_ext_flux_2;
		float Magnetics_ext_flux_3;
		float Magnetics_ext_flux_4;
		float Magnetics_ext_flux_5;
		float Magnetics_ext_flux_6;
		float Magnetics_ext_flux_7;
		float Magnetics_ext_flux_8;
		float Magnetics_ext_flux_9;
		float Magnetics_ext_flux_10;
		float Magnetics_ext_flux_11;
		// Radial and vertical position and plasma current with flux corrections
		float Magnetics_R_corrctd;
		float Magnetics_z_corrctd;
		float Magnetics_Ip_corrctd;
		// Variables from the position reconstruction
		float RMSE_mirnv;
		float RMSE_Ifil;
		float Magnetics_SVD_recons_0;
		float Magnetics_SVD_recons_1;
		float Magnetics_SVD_recons_2;
		float Magnetics_SVD_recons_3;
		float Magnetics_SVD_recons_4;
		float Magnetics_SVD_recons_5;
		float Magnetics_SVD_recons_6;
		float Magnetics_SVD_recons_7;
		float Magnetics_SVD_recons_8;
		float Magnetics_SVD_recons_9;
		float Magnetics_SVD_recons_10;
		float Magnetics_SVD_recons_11;

	};

	uint saved_usectime;
	uint lasttime;
	float slope;
	float *lastmirnov;
	float *mirnovaccumulator;
	int accumulatorcounter;
	float buff;
	int k;
	int i;
	int j;
	int m;
	int n;
	int o;
	int p;
	int doonce;
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
	
	// Matrix for miltufilament model
	float *Mpf_SVD;
	float *Mfp;
	float *Ipf_corr;
	float *Ipf;
	float *Bmag_rec;
	float *Bmag_rec_corr;
	float *IfilR;
	float *IfilZ;
	float sum_Ifil;
	float sum_Ifil_corr;
	float Ipf_buff;
	float Ipf_corr_buff;
	
	
	//Cenas pra os modelos em espaço de estados
	float *x_prim;
	float *x_vert;
	float **x_hor;
	float **x_hor_buff;
	float x_hor_cpy;
	float *x_hor_vec;
	float *x_buff_hor;
	float *x_test;
	
	
	
	float *A_prim;
	float *A_vert;
	float ***A_hor;
	float *A_hor_vec;
	
	float *B_prim;
	float *B_vert;
	float **B_hor;
	float *B_hor_vec;
	
	float *C_prim;
	float *C_vert;
	float ** C_hor;
	float *C_hor_vec;
	//MatrixT<float>C_hor;
	//float C_hor[][];
	float *y_prim;
	float *y_vert;
	float *y_hor;
	float buffer_hor;
	float y_buff2;
	// Sinal Reconstruido a partir dos fluxos externos
	float *ADC_ext_flux;
	// Sinal das mirnov sem a contribuição dos campos externos
	float *ADC_final;
	

	// Picked values of magneticflux in databases for substraction
	float *allmirnv_prim;
	float *allmirnv_hor;
	float *allmirnv_vert;
	
	// cenas pràs correntes
	
	float prim_meas;
	float hor_meas;
	float vert_meas;

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
	float radial_position_corr, vertical_position_corr;
//	float rOffset, zOffset;
	float magnetic_field_sum;
	float magnetic_field_sum_corr;
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


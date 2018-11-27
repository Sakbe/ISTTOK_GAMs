
#include "MagneticsGAM.h"
#include "math.h"

OBJECTLOADREGISTER(MagneticsGAM, "$Id: $")


//  ******** Default constructor ***********************************
MagneticsGAM::MagneticsGAM() {
	this->SignalsInputInterface = NULL;
	this->SignalsOutputInterface = NULL;
}


// ********* Destructor ********************************************
MagneticsGAM::~MagneticsGAM()
{

}


//{ ********* Initialise the module ********************************
bool MagneticsGAM::Initialise(ConfigurationDataBase& cdbData) {

	CDBExtended cdb(cdbData);

	int i,j;

	// read config file section: magnetic_probes
	if (!cdb.ReadInt32(usectime_to_wait_for_starting_operation, "usectime_to_wait_for_starting_operation"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s usectime_to_wait_for_starting_operation", this->Name());
		return False;
	}
	else AssertErrorCondition(Information, "MagneticsGAM::Initialise: usectime_to_wait_for_starting_operation = %d", usectime_to_wait_for_starting_operation);
	if (!cdb.ReadInt32(i, "magnetic_radial_bool"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s magnetic_radial_bool", this->Name());
		return False;
	}
	else
	{
		magnetic_radial_bool = (bool)i;
		AssertErrorCondition(Information, "MagneticsGAM::Initialise: magnetic_radial_bool = %d", magnetic_radial_bool);
	}
	if (!cdb.ReadInt32(i, "magnetic_vertical_bool"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s magnetic_vertical_bool", this->Name());
		return False;
	}
	else
	{
		magnetic_vertical_bool = (bool)i;
		AssertErrorCondition(Information, "MagneticsGAM::Initialise: magnetic_vertical_bool = %d", magnetic_vertical_bool);
	}
	if (!cdb.ReadInt32(i, "magnetic_module_correction_bool"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s magnetic_module_correction_bool", this->Name());
		return False;
	}
	else
	{
		magnetic_module_correction_bool = (bool)i;
		AssertErrorCondition(Information, "MagneticsGAM::Initialise:  = %d", magnetic_module_correction_bool);
	}


	if (!cdb->Move("MirnovArrayDescription"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+magnetic_probes.MirnovArrayDescription\"", this->Name());
		return False;
	}
	if (!cdb.ReadInt32(NumberOfProbes, "NumberOfProbes"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s NumberOfProbes", this->Name());
		return False;
	}
	else	AssertErrorCondition(Information, "MagneticsGAM::Initialise: NumberOfProbes = %d", NumberOfProbes);

	if (NumberOfProbes > 0) {
		magnetic_Angles = new float[NumberOfProbes];
		magnetic_Calibration = new float[NumberOfProbes];

		if (!cdb.ReadFloatArray(magnetic_Angles, (int *)(&NumberOfProbes), 1, "Angles"))
		{
			AssertErrorCondition(InitialisationError, "ReadWaveformFiles: Could not read magnetic_Angles");
			return False;
		}
		else for (i = 0; i<NumberOfProbes; i++) AssertErrorCondition(Information, "MagneticsGAM::Initialise: magnetic_Angles[%d] = %f", i, magnetic_Angles[i]);
		if (!cdb.ReadFloatArray(magnetic_Calibration, (int *)(&NumberOfProbes), 1, "Calibration"))
		{
			AssertErrorCondition(InitialisationError, "ReadWaveformFiles: Could not read magnetic_Calibration");
			return False;
		}
		else for (i = 0; i<NumberOfProbes; i++) AssertErrorCondition(Information, "MagneticsGAM::Initialise: magnetic_Calibration[%d] = %f", i, magnetic_Calibration[i]);
	}
	else {
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s NumberOfProbes lower than 1", this->Name());
		return False;
	}

	cdb->MoveToFather();

	if (!cdb->Move("Measurements"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+magnetic_probes.Measurements\"", this->Name());
		return False;
	}
	if (!cdb.ReadInt32(NumberOfMeasurements, "NumberOfMeasurements"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s NumberOfMeasurements", this->Name());
		return False;
	}
	else	AssertErrorCondition(Information, "MagneticsGAM::Initialise: NumberOfMeasurements = %d", NumberOfMeasurements);

	if (NumberOfMeasurements > 0) {
		ProbeNumbers = new int[NumberOfMeasurements];

		if (!cdb.ReadInt32Array(ProbeNumbers, (int *)(&NumberOfMeasurements), 1, "ProbeNumbers"))
		{
			AssertErrorCondition(InitialisationError, "ReadWaveformFiles: Could not read ProbeNumbers");
			return False;
		}
		else for (i = 0; i<NumberOfMeasurements; i++) AssertErrorCondition(Information, "MagneticsGAM::Initialise: ProbeNumbers[%d] = %d", i, ProbeNumbers[i]);
	}
	else {
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s NumberOfMeasurements lower than 1", this->Name());
		return False;
	}

	cdb->MoveToFather();

	//Added for module offset correction
	if (!cdb->Move("ModuleOffsetCorrectionLSBusec"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s Could not move to \"+MARTe.+ISTTOK_RTTh.+magnetic_probes.ModuleOffsetCorrectionLSBusec\"", this->Name());
		return False;
	}
	if (!cdb.ReadInt32(NumberOfModules, "NumberOfModules"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s NumberOfModules", this->Name());
		return False;
	}
	else	AssertErrorCondition(Information, "MagneticsGAM::Initialise: NumberOfModules = %d", NumberOfMeasurements);

	if (NumberOfModules > 0) {
		magnetic_Offset_slope = new float[NumberOfModules];

		if (!cdb.ReadFloatArray(magnetic_Offset_slope, (int *)(&NumberOfModules), 1, "OffsetCalibration"))
		{
			AssertErrorCondition(InitialisationError, "ReadWaveformFiles: Could not OffsetCalibration");
			return False;
		}
		else for (i = 0; i<NumberOfModules; i++) AssertErrorCondition(Information, "MagneticsGAM::Initialise: magnetic_Offset_slope[%d] = %f", i, magnetic_Offset_slope[i]);
	}
	else {
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s NumberOfModules lower than 1", this->Name());
		return False;
	}

	if (NumberOfModules > 0) {
		magnetic_Polarity_calibration = new float[NumberOfModules];

		if (!cdb.ReadFloatArray(magnetic_Polarity_calibration, (int *)(&NumberOfModules), 1, "PolarityCalibration"))
		{
			AssertErrorCondition(InitialisationError, "ReadWaveformFiles: Could not read PolarityCalibration");
			return False;
		}
		else for (i = 0; i<NumberOfModules; i++) AssertErrorCondition(Information, "MagneticsGAM::Initialise: magnetic_Polarity_calibration[%d] = %f", i, magnetic_Polarity_calibration[i]);
	}
	else {
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s NumberOfModules lower than 1", this->Name());
		return False;
	}

	cdb->MoveToFather();

	// Create the signal interfaces
	if (!AddInputInterface(this->SignalsInputInterface, "MagneticsGAMInputInterface"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s failed to add the MagneticsGAMInputInterface", this->Name());
		return False;
	}
	if (!AddOutputInterface(this->SignalsOutputInterface, "MagneticsGAMOutputInterface"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s failed to add the MagneticsGAMOutputInterface", this->Name());
		return False;
	}

	//	INPUT SIGNALS (interface)
	if (!cdb->Move("input_signals"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s Could not move to \"input_signals\"", this->Name());
		return False;
	}
	int number_of_signals_to_read = 16;
	FString *CDB_move_to;
	FString *SignalType;
	CDB_move_to = new FString[number_of_signals_to_read];
	SignalType = new FString[number_of_signals_to_read];
	for (i = 0; i<number_of_signals_to_read - 1; i++) CDB_move_to[i].Printf("Channel_%d", i);
	CDB_move_to[number_of_signals_to_read - 1].Printf("time");
	for (i = 0; i<number_of_signals_to_read; i++) {

		if (!cdb->Move(CDB_move_to[i].Buffer()))
		{
			AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s Could not move to \"%s\"", this->Name(), CDB_move_to[i].Buffer());
			return False;
		}
		if (cdb->Exists("SignalType"))
		{
			FString signalName;
			cdb.ReadFString(SignalType[i], "SignalType");
		}
		if (cdb->Exists("SignalName"))
		{
			FString SignalName;
			cdb.ReadFString(SignalName, "SignalName");
			AssertErrorCondition(Information, "MagneticsGAM::Initialise: Added signal = %s", SignalName.Buffer());

			if (!this->SignalsInputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s failed to add signal", this->Name());
				return False;
			}
		}
		cdb->MoveToFather();
	}

	cdb->MoveToFather();

	//	OUTPUT SIGNALS (interface)
	if (!cdb->Move("output_signals"))
	{
		AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s Could not move to \"output_signals\"", this->Name());
		return False;
	}

	number_of_signals_to_read = 15;
	CDB_move_to = new FString[number_of_signals_to_read];
	SignalType = new FString[number_of_signals_to_read];
	CDB_move_to[0].Printf("magnetic_probes_r");
	CDB_move_to[1].Printf("magnetic_probes_z");
	CDB_move_to[2].Printf("magnetic_probes_plasma_current");
	CDB_move_to[3].Printf("ADC_magnetic_WO_corrctd_0");
	CDB_move_to[4].Printf("ADC_magnetic_WO_corrctd_1");
	CDB_move_to[5].Printf("ADC_magnetic_WO_corrctd_2");
	CDB_move_to[6].Printf("ADC_magnetic_WO_corrctd_3");
	CDB_move_to[7].Printf("ADC_magnetic_WO_corrctd_4");
	CDB_move_to[8].Printf("ADC_magnetic_WO_corrctd_5");
	CDB_move_to[9].Printf("ADC_magnetic_WO_corrctd_6");
	CDB_move_to[10].Printf("ADC_magnetic_WO_corrctd_7");
	CDB_move_to[11].Printf("ADC_magnetic_WO_corrctd_8");
	CDB_move_to[12].Printf("ADC_magnetic_WO_corrctd_9");
	CDB_move_to[13].Printf("ADC_magnetic_WO_corrctd_10");
	CDB_move_to[14].Printf("ADC_magnetic_WO_corrctd_11");

	for (i = 0; i<number_of_signals_to_read; i++) {

		if (!cdb->Move(CDB_move_to[i].Buffer()))
		{
			AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s Could not move to \"%s\"", this->Name(), CDB_move_to[i].Buffer());
			return False;
		}

		if (cdb->Exists("SignalType"))
		{
			FString signalName;
			cdb.ReadFString(SignalType[i], "SignalType");
		}
		if (cdb->Exists("SignalName"))
		{
			FString SignalName;
			cdb.ReadFString(SignalName, "SignalName");
			AssertErrorCondition(Information, "MagneticsGAM::Initialise: Added signal = %s", SignalName.Buffer());

			if (!this->SignalsOutputInterface->AddSignal(SignalName.Buffer(), SignalType[i].Buffer()))
			{
				AssertErrorCondition(InitialisationError, "MagneticsGAM::Initialise: %s failed to add signal", this->Name());
				return False;
			}
		}

		cdb->MoveToFather();
	}

	cdb->MoveToFather();

	// Initialise the accumulators
	this->allmirnv_prim = new float[this->NumberOfProbes];
	this->allmirnv_hor = new float[this->NumberOfProbes];
	this->allmirnv_vert = new float[this->NumberOfProbes];
	this->ADC_values = new float[this->NumberOfProbes];
	this->ADC_WO = new float[this->NumberOfProbes];
	this->ADC_WO_Wb = new float[this->NumberOfProbes];
	
	
	this->corrected_probes = new float[this->NumberOfMeasurements];
	this->magnetic_Offset_zero = new float[this->NumberOfModules];
	for (i = 0; i < this->NumberOfProbes; i++) {
		this->ADC_values[i] = 0.0;
		this->ADC_WO[i] = 0.0;
		this->ADC_WO_Wb[i] = 0.0;
		this->corrected_probes[i] = 0.0;
		this->magnetic_Offset_zero[i] = 0.0;
		this->allmirnv_prim[i]=0.0;
		this->allmirnv_vert[i] = 0.0;
		this->allmirnv_hor[i] = 0.0;
	}
	magnetic_field_sum = 0.0;

	//ACHTUNG ACHTUNG!!! 0.1 if 100us and 0.01 if 1000us
	// Correct Offsets factor - values Bits/ms -> bits/100us
	for (i = 0; i < this->NumberOfMeasurements; i++) {
		this->magnetic_Offset_slope[i] = this->magnetic_Offset_slope[i] * 1;
	}

	// Initialise the auxiliary probe position values [m]
	this->n_samples = 0;
	this->major_radius = 0.46;
	this->probe_radius = 0.0935;
	this->clip_limit = 0.085; // -clip_limit < output r and z position < +clip_limit

	//Some constantants
	this->Area = 4.9e-5; //[m^2]
	this->Nvoltas = 50; //
	this->MAgPerm = 4*3.16159e-7; //[V*s/A*m]
	this->ADCconst = 10/((2^17)*2e6);
	this->Ncoils = 12;

//ADC fatores
this->ADC_fact = (float[12]) {0.8605*1e-11 ,0.8582*1e-11 ,0.8518*1e-11 ,0.8633*1e-11 ,0.8583*1e-11 ,0.8590*1e-11 ,0.8616*1e-11 ,0.8610*1e-11 ,0.8580*1e-11 ,0.8608*1e-11 ,0.8576*1e-11 ,0.8653*1e-11 };

	// slopes

	this->slopes = new float*[this->NumberOfProbes];
	for (i = 0; i<12; i++) {
		this->slopes[i] = new float[29];
	}

	float slopes[12][29]={
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 },
			{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }};
			
			
///////////////// External fluxes subtraction!!!!!!  /////////////////////////// 
	//Lets load initial conditions of the state space models (prim and vert 1 state, hor 4 states) in the state vectors
	
	this ->x_prim=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};
	this ->x_vert=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};
	
	this->x_hor= new float*[12];
	for (i = 0; i<12; i++) {
		this->x_hor[i] = new float[4];
	}
	float x_hor[12][4]={{-0.0003,-0.0013 ,-0.0116,-0.0028  },
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0}};			
			
			
		// Load A matrices
		this ->A_prim=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};
		this ->A_vert=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};		
		this->A_hor= new float**[12]();
	for (i = 0; i<12; i++) {
		this->A_hor[i] = new float*[4]();
		for (j = 0; j < 4; j++)
			this->A_hor[i][j]=new float[4]();
	}
	
	
	float A_hor[12][4][4]={{{0.9721,-0.0326,-0.0061,0.0111}, {-0.0652,0.9102,-0.0437,0.0786},{-0.0021,-0.0113,0.8091,0.5950},{-0.0251,-0.0883,-0.5616, 0.7994}},
			      { {0.9890,-0.0052, 0.0037, 0.0024},{-0.0668,0.7655,0.4834,0.0621},{0.0444,0.1091,0.5870,-1.0826},{0.0347, 0.2852,-0.3127,-0.2216}	},
			    	{{0.9911,-0.0040,-0.0030,-0.0025},{-0.0326,0.8415,-0.4706,-0.0508},{0.0145,0.4454,0.8093, -0.3695},{-0.0338,0.1428,0.3406,0.8775}	},
			        {{0.9759,-0.0401,-0.0430,-0.0229},{-0.0048,0.8243,-0.5626,0.0195},{0.0596,0.5560,0.8088,-0.1765},{-0.0188,0.0708,0.1653,0.9535}	},
			      	{{0.9998,-0.0145,-0.0039,0.0087},{0.0073,0.9215,-0.1037,0.1497},{-0.0006,0.0304,0.8199, 0.5662},{ -0.0114,-0.1889,-0.5493, 0.8112}	},
			   	  { {0.9994,-0.0167,-0.0031,-0.0068},{0.0251,0.8960,-0.0582,-0.0925},{-0.0026,0.0015,0.8130,-0.5863},{0.0063,0.1171,0.5652,0.8039}	},
			   	  { {0.9996,-0.0208,0.0112,0.0027},{0.0126,0.9118,0.0834,0.0073},{0.0131,0.2135,0.0647,-0.7607},{0.0005,-0.0272,-0.2133,-0.0849}	},
			   	  { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}	},
			   	  { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}	},
			   	  { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}	},
			   	  { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}	},
			 	  { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}	} };
	

//Load B matrices
	this ->B_prim=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};
	this ->B_vert=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};
	this->B_hor= new float*[12];
	for (i = 0; i<12; i++) {
		this->B_hor[i] = new float[4];
	}
	float B_hor[12][4]={{0,0,0,0  },
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0}};


//Load C matrices
	this ->C_prim=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};
	this ->C_vert=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};
	this ->C_hor= new float*[12];
	for (i = 0; i<12; i++) {
		this->C_hor[i] = new float[4];
	}
	float C_hor[12][4]={{0,0,0,0 },
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0},
			     {0,0,0,0}};

// Initialization of outputs
	this ->y_prim=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};
	this ->y_vert=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};
	this->y_hor=(float[12]){0,0,0,0,0,0,0,0,0,0,0,0};
//////////////////////////////////////////////////////////////////////////////////
	this->radial_coeficients = new float[this->NumberOfProbes];
	this->vertical_coeficients = new float[this->NumberOfProbes];
	for (i = 0; i < this->NumberOfProbes; i++) {

		this->radial_coeficients[i] = this->probe_radius * cos(this->magnetic_Angles[i] * M_PI / 180);
		this->vertical_coeficients[i] = this->probe_radius * sin(this->magnetic_Angles[i] * M_PI / 180);
	}

	if (NumberOfMeasurements == NumberOfProbes) {

		this->points_x = new float[NumberOfProbes / 4];
		this->points_y = new float[NumberOfProbes / 4];
		this->m_x = new float[NumberOfProbes];
		this->m_y = new float[NumberOfProbes];

		for (i = 0; i < this->NumberOfProbes; i++) {
			m_x[i] = this->radial_coeficients[i] / this->probe_radius;
			m_y[i] = this->vertical_coeficients[i] / this->probe_radius;
		}
	}

	//this->plasma_current_convertion_factor = 4300 * 2.0 * M_PI * this->probe_radius / this->NumberOfMeasurements;
	this->plasma_current_convertion_factor = -1.0*(2.0 * M_PI * this->probe_radius / this->Ncoils)*(1/(this->MAgPerm*this->Nvoltas*this->Area));
	this->saved_usectime = 0;
	this->lastmirnov = new float[this->NumberOfProbes];
	for(i=0;i<this->NumberOfProbes;i++){
		this->lastmirnov[i] = (float) 0;
	}	
	this->mirnovaccumulator = new float[this->NumberOfProbes];
	for(i=0;i<this->NumberOfProbes;i++){
		this->mirnovaccumulator[i] = (float) 0;
	}	
	this->accumulatorcounter = 0;
	this->k = 0;
	this->buff = 0.0;
	return True;
}
//} ******************************************************************


//{ ********* Execute the module functionalities *******************
bool MagneticsGAM::Execute(GAM_FunctionNumbers functionNumber) {

	InputInterfaceStruct *inputstruct = (InputInterfaceStruct *) this->SignalsInputInterface->Buffer();
	this->SignalsInputInterface->Read();
	OutputInterfaceStruct *outputstruct = (OutputInterfaceStruct *) this->SignalsOutputInterface->Buffer();

	int i,j=0;
	
	float prim_meas=0.0;
	float hor_meas=0.0;
	float vert_meas=0.0;

	ADC_values[0] = (float)inputstruct[0].ADC_magnetic_chopper_fp_0;
	ADC_values[1] = (float)inputstruct[0].ADC_magnetic_chopper_fp_1;
	ADC_values[2] = (float)inputstruct[0].ADC_magnetic_chopper_fp_2;
	ADC_values[3] = (float)inputstruct[0].ADC_magnetic_chopper_fp_3;
	ADC_values[4] = (float)inputstruct[0].ADC_magnetic_chopper_fp_4;
	ADC_values[5] = (float)inputstruct[0].ADC_magnetic_chopper_fp_5;
	ADC_values[6] = (float)inputstruct[0].ADC_magnetic_chopper_fp_6;
	ADC_values[7] = (float)inputstruct[0].ADC_magnetic_chopper_fp_7;
	ADC_values[8] = (float)inputstruct[0].ADC_magnetic_chopper_fp_8;
	ADC_values[9] = (float)inputstruct[0].ADC_magnetic_chopper_fp_9;
	ADC_values[10] = (float)inputstruct[0].ADC_magnetic_chopper_fp_10;
	ADC_values[11] = (float)inputstruct[0].ADC_magnetic_chopper_fp_11;

	// Measured horizontal, Vertical & Primary currents
	prim_meas= inputstruct[0].PrimaryCurrent;
	hor_meas= inputstruct[0].HorizontalCurrent;
	vert_meas=inputstruct[0].VerticalCurrent;
	
	

	//Apply coil polarity factor - OK ---vamos usar depois, pra calcular posicao e corrente 
	//for (i = 0; i < this->NumberOfMeasurements; i++) {
		//ADC_values[i] = ADC_values[i] * magnetic_Polarity_calibration[i];
	//}
	


	//if(functionNumber == GAMOffline){
		//if (((uint)inputstruct[0].usectime - this->saved_usectime) > 30000000 || ((uint)inputstruct[0].usectime - this->saved_usectime) < 0 ) {
			//AssertErrorCondition(Information,"MagneticsGAM:: GAMOffline: usectime: %i ", (uint)inputstruct[0].usectime);
			//slope = ((this->ADC_values[0] - this->lastmirnov[0])) / ((float)((uint)inputstruct[0].usectime - this->saved_usectime + 0.1));
			//AssertErrorCondition(Information,"MagneticsGAM:: GAMOffline: usectime: %i, ADC_magnetic_chopper_fp_0 : %i, ADC: %f, slope: %f", (uint)inputstruct[0].usectime,inputstruct[0].ADC_magnetic_chopper_fp_0, ADC_values[0],slope);
			//this->saved_usectime = (uint)inputstruct[0].usectime;
			//lastmirnov[0] = ADC_values[0];
		//}
		//this->lasttime=inputstruct[0].usectime;
		//this->k=0;
	//}




	if (functionNumber == GAMOnline) {
		// Determine the ADC Module offset "b" as "y(n)=a*n+b"
		if (((uint)inputstruct[0].usectime - this->lasttime) > 89000000) { // Start averaging 1s before start taking slope averages
			this->accumulatorcounter++ ;
			for(i=0;i<this->NumberOfProbes;i++){
				this->mirnovaccumulator[i] = (mirnovaccumulator[i] * (accumulatorcounter - 1) + ADC_values[i])/accumulatorcounter;
			}	
			
		}
		
		if (((uint)inputstruct[0].usectime - this->saved_usectime) > 999900 || ((uint)inputstruct[0].usectime - this->saved_usectime) < 0 ) { //compara um segundo
			
			slope = (this->mirnovaccumulator[0] - this->lastmirnov[0]) / ((float)((uint)inputstruct[0].usectime - this->saved_usectime + 0.1));

			//AssertErrorCondition(Information,"MagneticsGAM:: GAMOnline: usectime: %i, ADC_magnetic_chopper_fp_0 : %f, slope: %f", (uint)inputstruct[0].usectime, ADC_values[0],slope);
									
			if (((uint)inputstruct[0].usectime - this->lasttime) > 90000000 && ((uint)inputstruct[0].usectime - this->lasttime) < 120000000) {
				//AssertErrorCondition(Information, "MagneticsGAM:: GAMOnline: sloping it");

				for (i = 0; i < this->NumberOfMeasurements; i++) {
					
					//this->slopes[i][k] = ((this->ADC_values[i] - this->lastmirnov[i])) / ((float)((uint)inputstruct[0].usectime - this->saved_usectime));
					this->slopes[i][k] = ((this->mirnovaccumulator[i] - this->lastmirnov[i])) / ((float)((uint)inputstruct[0].usectime - this->saved_usectime));
				}
				k++;
				
				
				// WO's average

				for (j = 0; j < this->NumberOfMeasurements; j++) {
					for (i = 0; i < 29; i++) {
						this->buff = this->buff + this->slopes[j][i];
					}
					this -> slope_avrg[j] = this->buff / (29) ;
					this->buff = 0.0;
				}
				
		
			}
			else k=0;
			

			//for (i = 0; i < this->NumberOfMeasurements; i++) {
			//	this->lastmirnov[i] = this->ADC_values[i];
			//}
			for (i = 0; i < NumberOfMeasurements; i++) {
				lastmirnov[i] = mirnovaccumulator[i];
				mirnovaccumulator[i] = (float) 0;
			}			
			this->accumulatorcounter = 0;
			
			this->saved_usectime = (uint)inputstruct[0].usectime;
		}

		

		if (inputstruct[0].usectime > 0 && inputstruct[0].usectime < usectime_to_wait_for_starting_operation) {

			//For now we do not use this step (under optimization)

			//Determine "b" by knowing "a" and "y(-100us)"
			//if(inputstruct[0].usectime==900){
			//	for(i = 0 ; i < this->NumberOfMeasurements ; i++){
			//		this->magnetic_Offset_zero[i] = ADC_values[i] + this->magnetic_Offset_slope[i]; // b = y(-100us) - a*(-100us) = y(10) + a*(1)
			//	}				
			//}			
			
			
			
			

			outputstruct[0].MagneticProbesR = 0.;
			outputstruct[0].MagneticProbesZ = 0.;
			outputstruct[0].MagneticProbesPlasmaCurrent = 0.;
			
			i=(int)inputstruct[0].usectime/100;
			if (i==1||i==2||i==3){
				outputstruct[0].ADC_magnetic_WO_corrctd_0 =  slope_avrg[0]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_1 =  slope_avrg[1]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_2 =  slope_avrg[2]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_3 =  slope_avrg[3]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_4 = slope_avrg[4]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_5 = slope_avrg[5]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_6 = slope_avrg[6]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_7 = slope_avrg[7]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_8 = slope_avrg[8]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_9 = slope_avrg[9]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_10 = slope_avrg[10]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_11 = slope_avrg[11]*1e-10;
			}
			else {
			if (i>28) {i=28;}
			// esTamos a multiplicar arbitrariamente por 1e-10 para reduzir a ordem de grandeza do sinal
			outputstruct[0].ADC_magnetic_WO_corrctd_0 = slopes[0][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_1 = slopes[1][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_2 = slopes[2][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_3 = slopes[3][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_4 = slopes[4][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_5 = slopes[5][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_6 = slopes[6][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_7 = slopes[7][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_8 = slopes[8][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_9 = slopes[9][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_10 = slopes[10][i]*1e-10;
			outputstruct[0].ADC_magnetic_WO_corrctd_11 = slopes[11][i]*1e-10;
		}
			
			
		}
		else {

			//Take offset at t=0
			if (inputstruct[0].usectime == usectime_to_wait_for_starting_operation) {
				for (i = 0; i < this->NumberOfMeasurements; i++) {
					this->magnetic_Offset_zero[i] = ADC_values[i];
					magnetic_field_sum = 0.0;
				}
			}

			//Correct using corrected= ADC[n]-(m*x+b), tirei o 1/100
			for (i = 0; i < this->NumberOfMeasurements; i++) {
				corrected_probes[i] = ADC_values[i] - (this->magnetic_Offset_slope[i] * ((inputstruct[0].usectime - usectime_to_wait_for_starting_operation) ) + this->magnetic_Offset_zero[i]);
			}

			//Correct usign the saved averaged slopes

			for (i = 0; i < this->NumberOfMeasurements; i++) {
				ADC_WO[i] = ADC_values[i] - this->slope_avrg[i] * ((inputstruct[0].usectime - usectime_to_wait_for_starting_operation)) - this->magnetic_Offset_zero[i];
			}
			
			// [LSB x sample] -> [V x s] or [Wb]    e com polaridade corregida

			for (i = 0; i < this->NumberOfMeasurements; i++) {
				ADC_WO_Wb[i]=ADC_fact[i]*magnetic_Polarity_calibration[i]*ADC_WO[i];
				}


			//Write the value of the 12 mirnov channel without the WO in Webers and polarity factor corrected
			outputstruct[0].ADC_magnetic_WO_corrctd_0 = ADC_WO_Wb[0];
			outputstruct[0].ADC_magnetic_WO_corrctd_1 = ADC_WO_Wb[1];
			outputstruct[0].ADC_magnetic_WO_corrctd_2 = ADC_WO_Wb[2];
			outputstruct[0].ADC_magnetic_WO_corrctd_3 = ADC_WO_Wb[3];
			outputstruct[0].ADC_magnetic_WO_corrctd_4 = ADC_WO_Wb[4];
			outputstruct[0].ADC_magnetic_WO_corrctd_5 = ADC_WO_Wb[5];
			outputstruct[0].ADC_magnetic_WO_corrctd_6 = ADC_WO_Wb[6];
			outputstruct[0].ADC_magnetic_WO_corrctd_7 = ADC_WO_Wb[7];
			outputstruct[0].ADC_magnetic_WO_corrctd_8 = ADC_WO_Wb[8];
			outputstruct[0].ADC_magnetic_WO_corrctd_9 = ADC_WO_Wb[9];
			outputstruct[0].ADC_magnetic_WO_corrctd_10 = ADC_WO_Wb[10];
			outputstruct[0].ADC_magnetic_WO_corrctd_11 = ADC_WO_Wb[11];
			


			//Compute fluxes to be substracted (Discrete State-Space equations)
			
				
				
			for (i = 0; i < this->NumberOfMeasurements; i++) {
				y_prim[i]=C_prim[i]*x_prim[i];
				x_prim[i]=A_prim[i]*x_prim[i]+B_prim[i]*prim_meas;
				}

			for (i = 0; i < this->NumberOfMeasurements; i++) {
				y_vert[i]=C_vert[i]*x_vert[i];
				x_vert[i]=A_vert[i]*x_vert[i]+B_vert[i]*vert_meas;
				}

			for (i = 0; i < this->NumberOfMeasurements; i++) {
				y_hor[i]=C_hor[i][1]*x_hor[i][1]+C_hor[i][2]*x_hor[i][2]+C_hor[i][3]*x_hor[i][3]+C_hor[i][4]*x_hor[i][4];
				
				}	
			
			for (i = 0; i < this->NumberOfMeasurements; i++) {
				ADC_ext_flux[i]=y_prim[i]+y_vert[i]+y_hor[i];
				}
			


			//Substract from corrected_probes magnetic flu values due to the Vertical, Horizontal and Primary coils
			for (i = 0; i < this->NumberOfMeasurements; i++) {
				corrected_probes[i] = corrected_probes[i]-allmirnv_vert[i]-allmirnv_hor[i]-allmirnv_prim[i];
			}

			// Calculate Ip
			magnetic_field_sum = 0.0;  //this->NumberOfMeasurements
			for (i = 0; i < this->NumberOfMeasurements; i++) {
				magnetic_field_sum = ADC_WO_Wb[i]+ magnetic_field_sum;
			}

			outputstruct[0].MagneticProbesPlasmaCurrent = magnetic_field_sum*this->plasma_current_convertion_factor;//corrected_probes[11]; 



																			
			// Estimate radial_position and vertical_position
			radial_position = 0.0;
			vertical_position = 0.0;
			/* This was done when the integrators were analogic
			if(NumberOfMeasurements == NumberOfProbes){
			// WARNING: this code divides by zero fairly often. Many IGBTs died to bring us this information
			for (i = 0 ; i < this->NumberOfProbes/4  ; i++){
			r_a = this->probe_radius * 2 * corrected_probes[int(i+NumberOfProbes/2)] / (corrected_probes[int(i+NumberOfProbes/2)] + corrected_probes[i]);
			r_b = this->probe_radius * 2 * corrected_probes[int(i+NumberOfProbes/2+NumberOfProbes/4)] / (corrected_probes[int(i+NumberOfProbes/2+NumberOfProbes/4)] + corrected_probes[int(i+NumberOfProbes/4)]);
			x_a = this->radial_coeficients[this->ProbeNumbers[i]] - m_x[i] * r_a;
			x_b = this->radial_coeficients[this->ProbeNumbers[int(i+this->NumberOfProbes/4)]] - m_x[int(i+this->NumberOfProbes/4)] * r_b;
			y_a = this->vertical_coeficients[this->ProbeNumbers[i]] - m_y[i] * r_a;
			y_b = this->vertical_coeficients[this->ProbeNumbers[int(i+this->NumberOfProbes/4)]] - m_y[int(i+this->NumberOfProbes/4)] * r_b;
			//					if (m_x[i] != 0 && m_x[int(i+NumberOfProbes/4)] != 0) {
			m_b = m_y[i]/m_x[i];
			m_a = m_y[int(i+this->NumberOfProbes/4)]/m_x[int(i+this->NumberOfProbes/4)];
			points_x[i] = (m_b*x_b-y_b-m_a*x_a+y_a) / (m_b-m_a);
			points_y[i] = m_a*(points_x[i]-x_a)+y_a;
			//					}
			radial_position += points_x[i];
			vertical_position += points_y[i];
			}

			}
			else {}*/


			for(i = 0 ; i < this->NumberOfMeasurements ; i++){

			radial_position += corrected_probes[i] * this->radial_coeficients[this->ProbeNumbers[i]];
			vertical_position += corrected_probes[i] * this->vertical_coeficients[this->ProbeNumbers[i]];
			}

			if (magnetic_field_sum !=0) {
			radial_position = radial_position / magnetic_field_sum;
			vertical_position = vertical_position / magnetic_field_sum;
			}
			else {
			radial_position = 0;
			vertical_position = 0;
			}
			


			// Hard clip position (limits for the output signal)
			if(radial_position < -this->clip_limit) radial_position = -this->clip_limit;
			if(radial_position > this->clip_limit) radial_position = this->clip_limit;

			if(vertical_position < -this->clip_limit) vertical_position = -this->clip_limit;
			if(vertical_position > this->clip_limit) vertical_position = this->clip_limit;

			

			outputstruct[0].MagneticProbesR = radial_position;
			outputstruct[0].MagneticProbesZ = vertical_position;

		}
	}
	else {		// GAMOffline & others
		//this->n_samples = 0;
		//for(i = 0 ; i < (this->NumberOfMeasurements) ; i++){
		//	this->magnetic_Offset_zero[i] = 0;
		//}
		outputstruct[0].MagneticProbesPlasmaCurrent = 0;
		outputstruct[0].MagneticProbesR = 0;
		outputstruct[0].MagneticProbesZ = 0;
		
		outputstruct[0].ADC_magnetic_WO_corrctd_0 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_1 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_2 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_3 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_4 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_5 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_6 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_7 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_8 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_9 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_10 = 0.;
		outputstruct[0].ADC_magnetic_WO_corrctd_11 = 0.;
		
		if (((uint)inputstruct[0].usectime - this->saved_usectime) > 30000000 || ((uint)inputstruct[0].usectime - this->saved_usectime) < 0 ) {
			AssertErrorCondition(Information,"MagneticsGAM:: GAMOffline: usectime: %i ", (uint)inputstruct[0].usectime);
			slope = ((this->ADC_values[0] - this->lastmirnov[0])) / ((float)((uint)inputstruct[0].usectime - this->saved_usectime + 0.1));
			AssertErrorCondition(Information,"MagneticsGAM:: GAMOffline: usectime: %i, ADC_magnetic_chopper_fp_0 : %i, ADC: %f, slope: %f", (uint)inputstruct[0].usectime,inputstruct[0].ADC_magnetic_chopper_fp_0, ADC_values[0],slope);
			lastmirnov[0] = ADC_values[0];
		}
		this->lasttime=inputstruct[0].usectime;
		this->k=0;
		this->saved_usectime = (uint)inputstruct[0].usectime;
		
		for (i = 0; i < this->NumberOfMeasurements; i++) {
				this->lastmirnov[i] = this->ADC_values[i];
			}
		this->mirnovaccumulator = (float[12]) {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
		this->accumulatorcounter = 0;

	}

	this->SignalsOutputInterface->Write();

	return True;
}
// ******************************************************************

bool MagneticsGAM::ProcessHttpMessage(HttpStream &hStream) {

	HtmlStream hmStream(hStream);
	int i,j;
	hmStream.SSPrintf(HtmlTagStreamMode, "html>\n\
		<head>\n\
		<title>%s</title>\n\
		</head>\n\
		<body>\n\
		<svg width=\"100&#37;\" height=\"100\" style=\"background-color: AliceBlue;\">\n\
		<image x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xlink:href=\"%s\" />\n\
		</svg", (char *) this->Name(), 0, 0, 422, 87, "http://www.ipfn.ist.utl.pt/ipfnPortalLayout/themes/ipfn/_img_/logoIPFN_Topo_officialColours.png");

	hmStream.SSPrintf(HtmlTagStreamMode, "br><br><text style=\"font-family:Arial;font-size:46\">%s</text><br", (char *) this->Name());

	FString submit_view;
	submit_view.SetSize(0);
	if (hStream.Switch("InputCommands.submit_view")) {
		hStream.Seek(0);
		hStream.GetToken(submit_view, "");
		hStream.Switch((uint32)0);
	}
	if (submit_view.Size() > 0) view_input_variables = True;

	FString submit_hide;
	submit_hide.SetSize(0);
	if (hStream.Switch("InputCommands.submit_hide")) {
		hStream.Seek(0);
		hStream.GetToken(submit_hide, "");
		hStream.Switch((uint32)0);
	}
	if (submit_hide.Size() > 0) view_input_variables = False;

	hmStream.SSPrintf(HtmlTagStreamMode, "form enctype=\"multipart/form-data\" method=\"post\"");
	if (!view_input_variables) {
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_view\" value=\"View input variables\"");
	}
	else {
		hmStream.SSPrintf(HtmlTagStreamMode, "input type=\"submit\" name=\"submit_hide\" value=\"Hide input variables\"");
		hmStream.SSPrintf(HtmlTagStreamMode, "br><br>magnetic_radial_bool = %d\n\
		<br>magnetic_vertical_bool = %d\n\
		<br>magnetic_module_correction_bool = %d\n\
		<br>NumberOfProbes = %d\n\
		<br>NumberOfMeasurements = %d\n\
		<br>NumberOfModules = %d\n\
		<br><br", magnetic_radial_bool, magnetic_vertical_bool, magnetic_module_correction_bool, NumberOfProbes, NumberOfMeasurements, NumberOfModules);

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>magnetic_Angles</td");
		for (i = 0; i<NumberOfProbes; i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td", magnetic_Angles[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>magnetic_Calibration</td");
		for (i = 0; i<NumberOfProbes; i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td", magnetic_Calibration[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>ProbeNumbers</td");
		for (i = 0; i<NumberOfMeasurements; i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%d</td", ProbeNumbers[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>magnetic_Offset_slope</td");
		for (i = 0; i<NumberOfModules; i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td", magnetic_Offset_slope[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");

		hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>magnetic_Polarity_calibration</td");
		for (i = 0; i<NumberOfModules; i++)hmStream.SSPrintf(HtmlTagStreamMode, "td>%.2f</td", magnetic_Polarity_calibration[i]);
		hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		
		
		for (j = 0; j < this->NumberOfMeasurements; j++) {
			hmStream.SSPrintf(HtmlTagStreamMode, "table border=\"1\"><tr><td>Calibration mirnov %d </td", j);
				for (i = 0; i < 29; i++) {
					hmStream.SSPrintf(HtmlTagStreamMode, "td>%.4f</td", slopes[j][i]);
				}
				hmStream.SSPrintf(HtmlTagStreamMode, "/tr><tr><td>Average: %.4f</td", slope_avrg[j]);
				hmStream.SSPrintf(HtmlTagStreamMode, "/tr></table><br");
		}
	}
	hmStream.SSPrintf(HtmlTagStreamMode, "/form");


	hmStream.SSPrintf(HtmlTagStreamMode, "/body>\n</html");
	hStream.SSPrintf("OutputHttpOtions.Content-Type", "text/html;charset=utf-8");
	hStream.WriteReplyHeader(True);


	return True;
}

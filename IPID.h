//******************************************************************************
//
//		IPID.cpp - differential PID equation - type C - out(n) = out(n-1) - Kp[PV(n)-PV(n-1)] + Ki*T*e(n) - (Kd/T)*[PV(n) - 2PV(n-1) + PV(n-2)]
//		Ivo Carvalho 29/4/2011
//
//******************************************************************************

#if !defined (IPID_H)
#define IPID_H

/** @file 
     differential PID equation */


#include "Level0.h"
#include "Level1.h"
#include "Level2.h"
//OBJECT_DLL(IPID)

/** differential PID equation */
class IPID {

//OBJECT_DLL_STUFF(IPID)
private:

	float error;
	float old_error;
	float old_old_error;
	float Kp_constant;
	float Ki_constant;
	float Kd_constant;
	float T_constant;
	float P_realtime_constant;
	float I_realtime_constant;
	float D_realtime_constant;
	float upper_limit;
	float lower_limit;
	float old_output;
	float old_PV;
	float old_old_PV;
	int bumpless;
	float proportional;
	float integral;
	float derivative;
	//MIMO controller stuff
	float *A;
	float *C;
	float *B;
	float *D;
	float *K;
	float *Nbar;
    float *x;	
	int antiw;
	

public:
	
	bool LoadOldOutputWithinLimits(float old_output_to_load);
	
	float GetUpperLimit(){
		return (this->upper_limit);
	}	
	
	float GetLowerLimit(){
		return (this->lower_limit);
	}	
	
	float GetOldOutput(){
		return (this->old_output);
	}	
	
	float GetRealtimeConstantP(){
		return (this->P_realtime_constant);
	}	
	
	float GetRealtimeConstantI(){
		return (this->I_realtime_constant);
	}	
	
	float GetRealtimeConstantD(){
		return (this->D_realtime_constant);
	}	
		
	float GetConstantKp(){
		return (this->Kp_constant);
	}	
		
	float GetConstantKi(){
		return (this->Ki_constant);
	}	
		
	float GetConstantKd(){
		return (this->Kd_constant);
	}	
		
	float GetConstantT(){
		return (this->T_constant);
	}	
		
	float GetError(){
		return (this->error);
	}	
	
	float GetOldProcessVariable(){
		return this->old_PV;
	}	
	
	float GetOldOldProcessVariable(){
		return this->old_old_PV;
	}
	
	float GetOldError(){
		return (this->old_error);
	}
	
	float GetOldOldError(){
		return (this->old_old_error);
	}
	
	IPID (float set_Kp_constant, float set_Ki_constant, float set_Kd_constant, float set_T_constant, float set_upper_limit, float set_lower_limit);
	
	IPID (float set_P_realtime_constant, float set_I_realtime_constant, float set_D_realtime_constant, float set_upper_limit, float set_lower_limit);
	
	bool SetPIDConstants(float set_Kp_constant, float set_Ki_constant, float set_Kd_constant, float set_T_constant);
	
	bool SetRealtimePIDConstants(float set_P_realtime_constant, float set_I_realtime_constant, float set_D_realtime_constant);
	
	bool SetLimits(float set_upper_limit, float set_lower_limit);
	
	float CalculatePID(float process_variable, float setpoint);
	
	float CalculatePID_sign(float process_variable, float setpoint, int sign);
	
	float CalculatePI(float process_variable, float setpoint);
	
	float CalculateP(float process_variable, float setpoint);
	
	float CalculatePID_types(float process_variable, float setpoint, float type);
	
	float CalculatePID_vert(float process_variable, float setpoint, int sign, int man2auto);
	
	float CalculatePID_hor(float process_variable, float setpoint, int sign, int man2auto);
	
	float MIMOcontrol(float R, float z, float RSP, float zSP, float sign);
	
	float ReturnErrorInPercentage(float process_variable, float setpoint);
	
	~IPID();

private:

	bool SortWaveform();
	bool RemoveRepeatedValues();

};


#endif












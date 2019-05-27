//******************************************************************************
//
//		IPID.cpp - differential PID equation
//		Ivo Carvalho 29/4/2011
//		Bimbows updated 05/2019	
//******************************************************************************


#include "IPID.h"

//OBJECTLOADREGISTER(IPID,"$Id: IPID.cpp,v 1.0 29/4/2011 14:22:36 ivoc Exp $")
// if cycle time is supplied
IPID::IPID(float set_Kp_constant, float set_Ki_constant, float set_Kd_constant, float set_T_constant, float set_upper_limit, float set_lower_limit){
	
	this->Kp_constant = set_Kp_constant;
	this->Ki_constant = set_Ki_constant;
	this->Kd_constant = set_Kd_constant;
	this->T_constant = set_T_constant;
	this->P_realtime_constant = this->Kp_constant;
	this->I_realtime_constant = this->Ki_constant * this->T_constant;
	this->D_realtime_constant = this->Kd_constant / this->T_constant;
	this->upper_limit = set_upper_limit;
	this->lower_limit = set_lower_limit;
	this->old_output = 0;
	this->old_PV = 0;
	this->old_old_PV = 0;
	this->old_error = 0;
	this->old_old_error = 0;
}
// create without cycle time, just with the constants used in realtime
IPID::IPID(float set_P_realtime_constant, float set_I_realtime_constant, float set_D_realtime_constant, float set_upper_limit, float set_lower_limit){
	
	this->P_realtime_constant = set_P_realtime_constant;
	this->I_realtime_constant = set_I_realtime_constant;
	this->D_realtime_constant = set_D_realtime_constant;
	this->T_constant = 0;	//undefined
	this->Kp_constant = 0;
	this->Ki_constant = 0;
	this->Kd_constant = 0;
	this->upper_limit = set_upper_limit;
	this->lower_limit = set_lower_limit;
	this->old_output = 0;
	this->old_PV = 0;
	this->old_old_PV = 0;
	this->old_error = 0;
	this->old_old_error = 0;
}

// ********* Destructor ********************************************
IPID::~IPID(){
	
}

bool IPID::LoadOldOutputWithinLimits(float old_output_to_load){
	
	if (old_output_to_load > upper_limit) old_output_to_load = upper_limit;
	if (old_output_to_load < lower_limit) old_output_to_load = lower_limit;
	this->old_output = old_output_to_load;
	this->old_PV = 0;
	this->old_old_PV = 0;
	this->old_error = 0;
	this->old_old_error = 0;
	return True;
}

bool IPID::SetPIDConstants(float set_Kp_constant, float set_Ki_constant, float set_Kd_constant, float set_T_constant){
	
	this->Kp_constant = set_Kp_constant;
	this->Ki_constant = set_Ki_constant;
	this->Kd_constant = set_Kd_constant;
	this->T_constant = set_T_constant;
	this->P_realtime_constant = this->Kp_constant;
	this->I_realtime_constant = this->Ki_constant * this->T_constant;
	this->D_realtime_constant = this->Kd_constant / this->T_constant;
	return True;
}

bool IPID::SetRealtimePIDConstants(float set_P_realtime_constant, float set_I_realtime_constant, float set_D_realtime_constant){
	
	this->P_realtime_constant = set_P_realtime_constant;
	this->I_realtime_constant = set_I_realtime_constant;
	this->D_realtime_constant = set_D_realtime_constant;
	if (this->T_constant > 0){
		this->Kp_constant = this->P_realtime_constant;
		this->Ki_constant = this->I_realtime_constant / this->T_constant;
		this->Kd_constant = this->D_realtime_constant * this->T_constant;
	}
	return True;
}

bool IPID::SetLimits(float set_upper_limit, float set_lower_limit){

	this->upper_limit = set_upper_limit;
	this->lower_limit = set_lower_limit;
	return True;
}

float IPID::CalculatePID(float process_variable, float setpoint){

	this->error = setpoint - process_variable;
	//type C
	// this->old_output = this->old_output - this->P_realtime_constant * (process_variable - this->old_PV) + this->I_realtime_constant * this->error - this->D_realtime_constant * (process_variable - 2 * this->old_PV +  this->old_old_PV);
	//type A
	this->old_output = this->old_output + this-> P_realtime_constant* (this->error - this->old_error) +  this->I_realtime_constant *this->error +  this->D_realtime_constant * (this->error - 2 * this->old_error + this->old_old_error);

	this->old_old_PV = this->old_PV;
	this->old_PV = process_variable;

    this->old_old_error = this->old_error;
	this->old_error = this->error;

	if(this->old_output > this->upper_limit) this->old_output = this->upper_limit;
	if(this->old_output < this->lower_limit) this->old_output = this->lower_limit;
	
	return this->old_output;
}

float IPID::CalculatePI(float process_variable, float setpoint){
	this->error = setpoint - process_variable;
	
	this->old_output = this->old_output - this->P_realtime_constant * (process_variable - this->old_PV) + this->I_realtime_constant * this->error;
	
	this->old_old_PV = this->old_PV;
	this->old_PV = process_variable;

	if(this->old_output > this->upper_limit) this->old_output = this->upper_limit;
	if(this->old_output < this->lower_limit) this->old_output = this->lower_limit;
	
	return this->old_output;
}

float IPID::CalculatePID_vert(float process_variable, float setpoint, int sign){
	
			if (sign == 1) {
			this->error = setpoint - process_variable;
			this->old_output = this->old_output + this-> P_realtime_constant* (this->error - this->old_error) +  this->I_realtime_constant *this->error +  this->D_realtime_constant * (this->error - 2 * this->old_error + this->old_old_error);

			}else if (sign == 2){	
				this->error = -setpoint + process_variable;
				this->old_output = this->old_output + 0.55*this-> P_realtime_constant* (this->error - this->old_error) +  0.55*this->I_realtime_constant *this->error +  0.55 * this->D_realtime_constant * (this->error - 2 * this->old_error + this->old_old_error);

			}
			
			//type C
			 //this->old_output = this->old_output - this->P_realtime_constant * (process_variable - this->old_PV) + this->I_realtime_constant * this->error - this->D_realtime_constant * (process_variable - 2 * this->old_PV +  this->old_old_PV);
			//type A
			//this->old_output = this->old_output + this-> P_realtime_constant* (this->error - this->old_error) +  this->I_realtime_constant *this->error +  this->D_realtime_constant * (this->error - 2 * this->old_error + this->old_old_error);
			
			this->old_old_PV = this->old_PV;
			this->old_PV = process_variable;
			this->old_old_error = this->old_error;
			this->old_error = this->error;
			if(this->old_output > this->upper_limit) this->old_output = this->upper_limit;
			if(this->old_output < this->lower_limit) this->old_output = this->lower_limit;
			return this->old_output;
	}
	
	
float IPID::CalculatePID_hor(float process_variable, float setpoint, int sign){
	
			if (sign == 1) {
			this->error = setpoint - process_variable;
			this->old_output = this->old_output + 0.75*this-> P_realtime_constant* (this->error - this->old_error) +  0.75*this->I_realtime_constant *this->error +  0.75*this->D_realtime_constant * (this->error - 2 * this->old_error + this->old_old_error);

			}else if (sign == 2){
			this->error = -setpoint + process_variable;
			this->old_output = this->old_output + this-> P_realtime_constant* (this->error - this->old_error) +  this->I_realtime_constant *this->error +  this->D_realtime_constant * (this->error - 2 * this->old_error + this->old_old_error);

			}
						
			//type C
			// this->old_output = this->old_output - this->P_realtime_constant * (process_variable - this->old_PV) + this->I_realtime_constant * this->error - this->D_realtime_constant * (process_variable - 2 * this->old_PV +  this->old_old_PV);
			//type A
			//this->old_output = this->old_output + this-> P_realtime_constant* (this->error - this->old_error) +  this->I_realtime_constant *this->error +  this->D_realtime_constant * (this->error - 2 * this->old_error + this->old_old_error);
			
			this->old_old_PV = this->old_PV;
			this->old_PV = process_variable;
			this->old_old_error = this->old_error;
			this->old_error = this->error;
			if(this->old_output > this->upper_limit) this->old_output = this->upper_limit;
			if(this->old_output < this->lower_limit) this->old_output = this->lower_limit;
			return this->old_output;
	}
	

float IPID::CalculatePID_sign(float process_variable, float setpoint, int sign){

	//new
	//sing == 1 is +error & sing==2 is -error
	if (sign == 1) {
			this->error = setpoint - process_variable;
			//type C
			 this->old_output = this->old_output - this->P_realtime_constant * (process_variable - this->old_PV) + this->I_realtime_constant * this->error - this->D_realtime_constant * (process_variable - 2 * this->old_PV +  this->old_old_PV);
			//type A
			//this->old_output = this->old_output + this-> P_realtime_constant* (this->error - this->old_error) +  this->I_realtime_constant *this->error +  this->D_realtime_constant * (this->error - 2 * this->old_error + this->old_old_error);
			this->old_old_PV = this->old_PV;
			this->old_PV = process_variable;
			this->old_old_error = this->old_error;
			this->old_error = this->error;
			if(this->old_output > this->upper_limit) this->old_output = this->upper_limit;
			if(this->old_output < this->lower_limit) this->old_output = this->lower_limit;
			return this->old_output;
		}else if (sign == 2){
			//this->error = -setpoint + process_variable;
			this->error = setpoint - process_variable;
			//type C
			 this->old_output = this->old_output + this->P_realtime_constant * (process_variable - this->old_PV) - this->I_realtime_constant * this->error - this->D_realtime_constant * (process_variable - 2 * this->old_PV +  this->old_old_PV);
			//type A
			//this->old_output = this->old_output + this-> P_realtime_constant* (this->error - this->old_error) +  this->I_realtime_constant *this->error +  this->D_realtime_constant * (this->error - 2 * this->old_error + this->old_old_error);
			this->old_old_PV = this->old_PV;
			this->old_PV = process_variable;
			this->old_old_error = this->old_error;
			this->old_error = this->error;
			if(this->old_output > this->upper_limit) this->old_output = this->upper_limit;
			if(this->old_output < this->lower_limit) this->old_output = this->lower_limit;
			return this->old_output;
	}
	} 
	//end new


float IPID::CalculateP(float process_variable, float setpoint){
	this->error = setpoint - process_variable;
	
	this->old_output = this->old_output - this->P_realtime_constant * (process_variable - this->old_PV);
	
	this->old_old_PV = this->old_PV;
	this->old_PV = process_variable;

	if(this->old_output > this->upper_limit) this->old_output = this->upper_limit;
	if(this->old_output < this->lower_limit) this->old_output = this->lower_limit;
	
	return this->old_output;
}

float IPID::CalculatePID_types(float process_variable, float setpoint, float type){
	this->error = setpoint - process_variable;
		
	if(type==1)//Type A
		this->old_output = this->old_output + this->Kp_constant*1000 * (this->error - this->old_error) + this->Ki_constant*1000 * this->error + this->Kd_constant*1000 * (this->error - 2 * this->old_error + this->old_old_error);
	if(type==2)//Type B
		this->old_output = this->old_output + this->Kp_constant*1000 * (this->error - this->old_error) + this->Ki_constant*1000 * this->error - this->Kd_constant*1000 * (process_variable - 2 * this->old_PV + this->old_old_PV);
	if(type==3)//Type C
		this->old_output = this->old_output - this->Kp_constant*1000 * (process_variable - this->old_PV) + this->Ki_constant*1000 * this->error - this->Kd_constant*1000 * (process_variable - 2 * this->old_PV + this->old_old_PV);
	
	this->old_old_PV = this->old_PV;
	this->old_PV = process_variable;
	this->old_old_error = this->old_error;
	this->old_error = this->error;

	//if(this->old_output > this->upper_limit) this->old_output = this->upper_limit;
	//if(this->old_output < this->lower_limit) this->old_output = this->lower_limit;
	
	if(this->old_output > this->upper_limit) return this->upper_limit;
	if(this->old_output < this->lower_limit) return this->lower_limit;
	
	return this->old_output;
}

float IPID::ReturnErrorInPercentage(float process_variable, float setpoint){
	this->error = setpoint - process_variable;
	
	if (this->error < 0) this->error = 0 - this->error;
	
	return ((100 * error)/(this->upper_limit - this->lower_limit)) ;
}

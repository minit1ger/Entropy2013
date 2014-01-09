#define WIDTH_OF_ROBOT 3
#define PI 3.14

#include <math.h>
#include "Drive.h"
#include "../Robotmap.h"
#include "../Commands/Move.h"

Drive::Drive() : Subsystem("Drive") {
	// The following variables are automatically assigned by
	// robotbuilder and will be updated the next time you export to
	// Java from robot builder. Do not put any code or make any change
	// in the following block or it will be lost on an update. To
	// prevent this subsystem from being automatically updated, delete
	// the following line.
	// BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=DECLARATIONS
	quadratureEncoderRight = RobotMap::DRIVE_QUADRATURE_ENCODER_RIGHT;
	quadratureEncoderLeft = RobotMap::DRIVE_QUADRATURE_ENCODER_LEFT;
	robotDrive4 = RobotMap::DRIVE_ROBOT_DRIVE_4;
	cANJaguarRearRight = RobotMap::DRIVE_CAN_JAGUAR_REAR_RIGHT;
	cANJaguarFrontRight = RobotMap::DRIVE_CAN_JAGUAR_FRONT_RIGHT;
	cANJaguarRearLeft = RobotMap::DRIVE_CAN_JAGUAR_REAR_LEFT;
	cANJaguarFrontLeft = RobotMap::DRIVE_CAN_JAGUAR_FRONT_LEFT;
	gyro = RobotMap::DRIVE_GYRO;
    // END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=DECLARATIONS
	LeftInvFlag = false;
	RightInvFlag = false;
}

void Drive::ArcadeDrive(GenericHID *stick) {
	robotDrive4->ArcadeDrive(stick);
} 

void Drive::InitDefaultCommand() {
	// Set the default command for a subsystem here.
	//SetDefaultCommand(new MySpecialCommand());
	// The following variables are automatically assigned by
	// robotbuilder and will be updated the next time you export to
	// Java from robot builder. Do not put any code or make any change
	// in the following block or it will be lost on an update. To
	// prevent this subsystem from being automatically updated, delete
	// the following line.
	// BEGIN AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=DEFAULT_COMMAND
    // END AUTOGENERATED CODE, SOURCE=ROBOTBUILDER ID=DEFAULT_COMMAND
	SetDefaultCommand(new Move());
}
// Put methods for controlling this subsystem
// here. Call these from Commands.

float Drive::auto_get_speed(float distance, float traveled, float speed)
{
	float percent = 0.05;
	
	if(traveled < percent*distance)
	{
		return 0.5*speed;
	}
	else if( traveled > distance - (distance*percent))
	{
		return 0.5*speed;
	}
	else
	{
		return speed;
	}
}

float Drive::GetLeftDistance() {
	if (LeftInvFlag)
		return (RobotMap::DRIVE_QUADRATURE_ENCODER_LEFT->GetDistance() * -1.0);
	else
		return (RobotMap::DRIVE_QUADRATURE_ENCODER_LEFT->GetDistance());
}

float Drive::GetRightDistance() {
	if (RightInvFlag)
		return (RobotMap::DRIVE_QUADRATURE_ENCODER_RIGHT->GetDistance() * -1.0);
	else
		return (RobotMap::DRIVE_QUADRATURE_ENCODER_RIGHT->GetDistance());
}
float Drive::GetDistance() {
	float autoCurrentAverageDistance = 0;
	float autoCurrentLeftDistance = fabs(GetLeftDistance());
	float autoCurrentRightDistance = fabs(GetRightDistance());
	if (autoCurrentLeftDistance > autoCurrentRightDistance)
		autoCurrentAverageDistance = autoCurrentLeftDistance;
	else 
		autoCurrentAverageDistance = autoCurrentRightDistance;
	
	printf("Left %f, Right %f\n",autoCurrentLeftDistance,autoCurrentRightDistance);
	return autoCurrentAverageDistance;
}
void Drive::ResetDistance() {
	RobotMap::DRIVE_QUADRATURE_ENCODER_LEFT->Reset();
	RobotMap::DRIVE_QUADRATURE_ENCODER_RIGHT->Reset();
}

void Drive::auto_drive_straight(float dist, float speed) 
{
	float distance = 0;
	float ideal_speed = speed * -1;
	float ideal_distance = fabs(dist);
	float degrees = 0;
	float degrees_initial = 0;
	float correct_for_reverse = 1.0;
	
	if(speed < 0.0)
	{
		correct_for_reverse = -1.0;
	}
	
    ResetDistance();
    gyro->Reset();
    sleep(1);
    degrees_initial = gyro->GetAngle();
   
	while(distance < ideal_distance) 
	{
		degrees = gyro->GetAngle();
		robotDrive4->ArcadeDrive(ideal_speed, 0);
		distance = fabs(Robot::drive->GetDistance());
				
		if((degrees_initial - degrees) > 1.0) {
			robotDrive4->ArcadeDrive(ideal_speed, 2 * correct_for_reverse);
			printf("Turn Left\n");
		}
		else if((degrees_initial - degrees) < -1.0) { 
			robotDrive4->ArcadeDrive(ideal_speed, -2 * correct_for_reverse);
			printf("Turn Right\n");
		}
		else {
			robotDrive4->ArcadeDrive(ideal_speed, 0);
			printf("Drive Straight\n");
		}
		printf("Angle %f\n", degrees);
		printf("Distance %f, Ideal Distance %f\n", distance, ideal_distance);
	}
	
    ResetDistance();
    gyro->Reset();
    
    robotDrive4->ArcadeDrive(ideal_speed, 0);
}

void Drive::auto_turn_left(float radius, float speed, float degrees) 
{
	ResetDistance();
	gyro->Reset();
	sleep(2);
	
	while(fabs(gyro->GetAngle()) < fabs(degrees))
	{
		DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line1, 1, "%f", gyro->GetAngle());
		DriverStationLCD::GetInstance()->UpdateLCD();
		robotDrive4->ArcadeDrive(speed * -1, radius);
	}
	
	ResetDistance();
	gyro->Reset();
}

void Drive::auto_turn_right(float radius, float speed, float degrees)
{
	ResetDistance();
    gyro->Reset();
    sleep(2);
	
	while(fabs(gyro->GetAngle()) < fabs(degrees))
	{
		DriverStationLCD::GetInstance()->Printf(DriverStationLCD::kUser_Line1, 1, "%f", gyro->GetAngle());
		DriverStationLCD::GetInstance()->UpdateLCD();
		robotDrive4->ArcadeDrive(speed * -1, radius * -1);
	}
	
	ResetDistance();
	gyro->Reset();
}

float Drive::GetAngle() {
	return gyro->GetAngle();
}
	

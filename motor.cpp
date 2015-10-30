#include "motor.h"
#include <main_window.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <QtGui>

//Declare a motor control handle
CPhidgetMotorControlHandle motoControl = 0;

int CCONV AttachHandler(CPhidgetHandle MC, void *userptr)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (MC, &name);
	CPhidget_getSerialNumber(MC, &serialNo);
	printf("%s %10d attached!\n", name, serialNo);

	return 0;
}

int CCONV DetachHandler(CPhidgetHandle MC, void *userptr)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (MC, &name);
	CPhidget_getSerialNumber(MC, &serialNo);
	printf("%s %10d detached!\n", name, serialNo);

	return 0;
}

int CCONV ErrorHandler(CPhidgetHandle MC, void *userptr, int ErrorCode, const char *Description)
{
	printf("Error handled. %d - %s\n", ErrorCode, Description);
	return 0;
}


int CCONV InputChangeHandler(CPhidgetMotorControlHandle MC, void *usrptr, int Index, int State)
{
	printf("Input %d > State: %d\n", Index, State);
	return 0;
}

int CCONV VelocityChangeHandler(CPhidgetMotorControlHandle MC, void *usrptr, int Index, double Value)
{
	printf("Motor %d > Current Speed: %f\n", Index, Value);
	return 0;
}

int CCONV CurrentChangeHandler(CPhidgetMotorControlHandle MC, void *usrptr, int Index, double Value)
{
	printf("Motor: %d > Current Draw: %f\n", Index, Value);
	return 0;
}

int display_properties(CPhidgetMotorControlHandle phid)
{
	int serialNo, version, numInputs, numMotors;
	const char* ptr;

	CPhidget_getDeviceType((CPhidgetHandle)phid, &ptr);
	CPhidget_getSerialNumber((CPhidgetHandle)phid, &serialNo);
	CPhidget_getDeviceVersion((CPhidgetHandle)phid, &version);
	
	CPhidgetMotorControl_getInputCount(phid, &numInputs);
	CPhidgetMotorControl_getMotorCount(phid, &numMotors);

	printf("%s\n", ptr);
	printf("Serial Number: %10d\nVersion: %8d\n", serialNo, version);
	printf("# Inputs: %d\n# Motors: %d\n", numInputs, numMotors);

	return 0;
}

extern "C" Plugin::Object *createRTXIPlugin(void){
	return new motor();
}

static DefaultGUIModel::variable_t vars[] = {
	{"velocity", "velocity", DefaultGUIModel::PARAMETER, }, // output(0)
};

static size_t num_vars = sizeof(vars) / sizeof(DefaultGUIModel::variable_t);

motor::motor(void) : DefaultGUIModel("motor with Custom GUI", ::vars, ::num_vars) {
	setWhatsThis("<p><b>motor:</b><br>QWhatsThis description.</p>");
	DefaultGUIModel::createGUI(vars, num_vars); // this is required to create the GUI
	customizeGUI();
	init_motor();
	update( INIT ); // this is optional, you may place initialization code directly into the constructor
	refresh(); // this is required to update the GUI with parameter and state values
}

motor::~motor(void) { }

void motor::execute(void) {
	return;
}

void motor::update(DefaultGUIModel::update_flags_t flag) {
	switch (flag) {
		case INIT:
			velocity = 0;
			setParameter("velocity", QString::number(velocity)); //initially 0
			CPhidgetMotorControl_setVelocity(motoControl, 0, velocity); // should let you change the target velocity
			break;
	
		case MODIFY:
			velocity = getParameter("velocity").toDouble();
			CPhidgetMotorControl_setVelocity(motoControl, 0, velocity); // should let you change the target velocity
			break;

		case UNPAUSE:
			break;

		case PAUSE:
			break;

		case PERIOD:
			period = RT::System::getInstance()->getPeriod() * 1e-6; // ms
			break;
	
		default:
			break;
	}
}

void motor::customizeGUI(void) {
	QGridLayout *customlayout = DefaultGUIModel::getLayout();
	
	QGroupBox *button_group = new QGroupBox;
	
	customlayout->addWidget(button_group, 0,0);
	setLayout(customlayout);
}

void motor::init_motor()
{
	int result;
	const char *err;

	//Declare a motor control handle
	//CPhidgetMotorControlHandle motoControl = 0;

	//create the motor control object
	CPhidgetMotorControl_create(&motoControl);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)motoControl, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)motoControl, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)motoControl, ErrorHandler, NULL);

	//Registers a callback that will run if an input changes.
	//Requires the handle for the Phidget, the function that will be called, and a arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetMotorControl_set_OnInputChange_Handler (motoControl, InputChangeHandler, NULL);

	//Registers a callback that will run if a motor changes.
	//Requires the handle for the Phidget, the function that will be called, and a arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetMotorControl_set_OnVelocityChange_Handler (motoControl, VelocityChangeHandler, NULL);

	//Registers a callback that will run if the current draw changes.
	//Requires the handle for the Phidget, the function that will be called, and a arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetMotorControl_set_OnCurrentChange_Handler (motoControl, CurrentChangeHandler, NULL);

	//open the motor control for device connections
	CPhidget_open((CPhidgetHandle)motoControl, -1);

	//get the program to wait for a motor control device to be attached
	printf("Waiting for MotorControl to be attached....");
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)motoControl, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		//return 0;
	}

	//Display the properties of the attached motor control device
	display_properties(motoControl);

	//read motor control event data
	printf("Reading.....\n");

	//keep displaying motor control event data until user input is read
	printf("Press any key to start in RTXI\n");
	getchar();

	//Control the motor a bit.
	//Step 1: increase acceleration to 50, set target sped at 100
//	CPhidgetMotorControl_setAcceleration (motoControl, 0, 50.00);
//	CPhidgetMotorControl_setVelocity (motoControl, 0, 100.00);

//	printf("Press any key to continue\n");
//	getchar();

	//Step 2: Set acceleration to 100, decrease target speed to 75
//	CPhidgetMotorControl_setAcceleration (motoControl, 0, 100.00);
//	CPhidgetMotorControl_setVelocity (motoControl, 0, 75.00);

//	printf("Press any key to continue\n");
//	getchar();

	//Step 3: Stop the motor by decreasing speed to 0;
//	CPhidgetMotorControl_setVelocity (motoControl, 0, 0.00);
//	CPhidgetMotorControl_setAcceleration (motoControl, 0, 0.00);

//	printf("Press any key to end\n");
//	getchar();

	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
//	printf("Closing...\n");
//	CPhidget_close((CPhidgetHandle)motoControl);
//	CPhidget_delete((CPhidgetHandle)motoControl);

	//all done, exit
	//return 0;
}

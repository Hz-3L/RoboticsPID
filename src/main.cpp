#include "main.h"
#include "lemlib/api.hpp"
#include "main.h"
#include "lemlib/api.hpp"

// --- 1. MOTORS & IMU(Inertial sensor port) ---
// Negative port number means reversed motor
pros::MotorGroup left_motors({-1, -2, -3}, pros::v5::MotorGears::blue);
pros::MotorGroup right_motors({4, 5, 6}, pros::v5::MotorGears::blue);
pros::Imu imu(10);

// --- 2. DRIVETRAIN & SENSORS ---
lemlib::Drivetrain drivetrain(
    &left_motors, &right_motors, 
    10.5,                       // Track width (inches between left & right wheels)
    lemlib::Omniwheel::NEW_275, // Wheel diameter
    360,                        // Drivetrain RPM
    2                           // Horizontal drift compensation
);

lemlib::OdomSensors sensors(
    nullptr, nullptr, nullptr, nullptr, // Tracking wheels (set to nullptr if using motor encoders)
    &imu                                // Inertial sensor
);

// --- 3. PID CONTROLLERS ---
lemlib::ControllerSettings lateral_controller(
    10, 0, 3, 0, // kP, kI, kD, anti-windup
    1, 100,      // Small error range (inches), timeout (ms)
    3, 500,      // Large error range (inches), timeout (ms)
    20           // Slew rate / max acceleration
);

lemlib::ControllerSettings angular_controller(
    2, 0, 10, 0, // kP, kI, kD, anti-windup
    1, 100,      // Small error range (degrees), timeout (ms)
    3, 500,      // Large error range (degrees), timeout (ms)
    0            // Slew rate / max acceleration
);

// --- 4. CHASSIS OBJECT ---
lemlib::Chassis chassis(drivetrain, lateral_controller, angular_controller, sensors);
     //THIS IS THE PART OF WHATEVR GITHUB WANTS EVERYTHING UNDERWAS ALREADY HERE! :3





/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
	pros::lcd::initialize();
	chassis.calibrate(); //Calibrated the IMU and starts tracking position
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
	// Example PID movement routines (ALSO MORE STUFF NEED TO KNOW)
	chassis.setPose(0, 0, 0); // Set the starting pos to x:0 y:0 heading:0deg
	chassis.moveToPoint(0, 24, 2000); // Drive forward 24 inches using PID
	chassis.turnToHeading(90, 1000); // Turn to 90 degrees using PID
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::MotorGroup left_mg({1, -2, 3});    // Creates a motor group with forwards ports 1 & 3 and reversed port 2
	pros::MotorGroup right_mg({-4, 5, -6});  // Creates a motor group with forwards port 5 and reversed ports 4 & 6


	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

		// Arcade control scheme
		int dir = master.get_analog(ANALOG_LEFT_Y);    // Gets amount forward/backward from left joystick
		int turn = master.get_analog(ANALOG_RIGHT_X);  // Gets the turn left/right from right joystick
		left_mg.move(dir - turn);                      // Sets left motor voltage
		right_mg.move(dir + turn);                     // Sets right motor voltage
		pros::delay(20);                               // Run for 20 ms then update
	}
}
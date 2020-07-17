#include <avr/wdt.h>


void setup() {
  wdt_enable(WDTO_1S);
  
  initializeMotors();
  initializeOutputSignals();
  initializeTelemetry();
  initializeRemotePidConfiguration();
  initializeReceiver();
  initializeIMU();

  wdt_disable();
  wdt_enable(WDTO_60MS);
}

struct Orientation previousOrientation;
struct Orientation actualOrientation;

void loop() {
  wdt_reset();
  
  syncOutputSignals();
  
  struct RawReceiverValues rawReceiverValues = readReceiverValues();
  struct ReceiverCommands receiverCommands = convertToReceiverCommands(rawReceiverValues);
  struct IMU_Values imu_values = readIMUvalues();
  
  if(rawReceiverValues.ReceiverError || receiverCommands.Throttle < THROTTLE_START_POINT || imu_values.IMU_Error)
  {
    runSafetyProtocol();
    return;
  }
  
  if (imu_values.DataAvailable){
    previousOrientation = actualOrientation;
    actualOrientation = imu_values.Orientation;
    calculateMotorPowers(receiverCommands, previousOrientation, actualOrientation);
  }

  spinMotors();
  
  sendTelemetryData();
  readRemotePidConfigurationCommand();
}

void runSafetyProtocol(){
  if (throttle < THROTTLE_START_POINT || receiver_failure == true || imu_failure == true){
    frontLeftMotorPower = MIN_THROTTLE;
    frontRightMotorPower = MIN_THROTTLE;
    rearLeftMotorPower = MIN_THROTTLE;
    rearRightMotorPower = MIN_THROTTLE;
    desired_yaw_angle_change = 0;
    desired_pitch_angle = 0;
    desired_roll_angle = 0;
    roll_pid_i = 0; 
    roll_last_error = 0;
    pitch_pid_i = 0; 
    pitch_last_error = 0;
    yaw_pid_i = 0;
    yaw_last_error = 0;
  }
}

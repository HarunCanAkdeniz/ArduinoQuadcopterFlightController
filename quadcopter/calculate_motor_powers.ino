unsigned long last_time = millis(); 
unsigned long current_time = millis();
int delta_time;
double roll_pid_i, roll_last_error, pitch_pid_i, pitch_last_error, yaw_pid_i, yaw_last_error;

void calculateMotorPowers() {
  unsigned long elapsed_time = millis() - last_time;
  if (elapsed_time < PID_SAMPLING_FREQUENCY)
    return;

  updateCurrentTimeVariables();
  double roll_control_signal = getControlSignal(desired_roll_angle - rollAngle, KP_roll_pitch, KI_roll_pitch, KD_roll_pitch, roll_pid_i, roll_last_error, ROLL_PITCH_INTEGRAL_LIMIT);
  double pitch_control_signal = getControlSignal(desired_pitch_angle - pitchAngle, KP_roll_pitch, KI_roll_pitch, KD_roll_pitch, pitch_pid_i, pitch_last_error, ROLL_PITCH_INTEGRAL_LIMIT);
  double yaw_control_signal = getControlSignal(calculateErrorForYaw(desired_yaw_angle, yawAngle), KP_yaw, KI_yaw, KD_yaw, yaw_pid_i, yaw_last_error, YAW_INTEGRAL_LIMIT);

  roll_control_signal = constrain(roll_control_signal, -MAX_ROLL_PITCH_CONTROL_GAIN, MAX_ROLL_PITCH_CONTROL_GAIN);
  pitch_control_signal = constrain(pitch_control_signal, -MAX_ROLL_PITCH_CONTROL_GAIN, MAX_ROLL_PITCH_CONTROL_GAIN);
  
  frontLeftMotorPower = round(throttle + roll_control_signal + pitch_control_signal - yaw_control_signal);
  frontRightMotorPower = round(throttle - roll_control_signal + pitch_control_signal + yaw_control_signal);
  rearLeftMotorPower = round(throttle + roll_control_signal - pitch_control_signal + yaw_control_signal);
  rearRightMotorPower = round(throttle - roll_control_signal - pitch_control_signal - yaw_control_signal);

  if(frontLeftMotorPower < THROTTLE_START_POINT) frontLeftMotorPower = THROTTLE_START_POINT;
  if(frontRightMotorPower < THROTTLE_START_POINT) frontRightMotorPower = THROTTLE_START_POINT;
  if(rearLeftMotorPower < THROTTLE_START_POINT) rearLeftMotorPower = THROTTLE_START_POINT;
  if(rearRightMotorPower < THROTTLE_START_POINT) rearRightMotorPower = THROTTLE_START_POINT;

  if (throttle == MIN_THROTTLE || receiver_failure == true) {
    frontLeftMotorPower = MIN_THROTTLE;
    frontRightMotorPower = MIN_THROTTLE;
    rearLeftMotorPower = MIN_THROTTLE;
    rearRightMotorPower = MIN_THROTTLE;
  }
  updateLastTimeVariables();
}

double calculateErrorForYaw(double desired, double actual) {
  double error = desired - actual;
  if (error > 180)
    error -= 360;
  else if (error < -180)
    error += 360;
  return error;
}

void updateCurrentTimeVariables() {
  current_time = millis();
  delta_time = (current_time - last_time);
}

void updateLastTimeVariables() {
  last_time = current_time;
}

unsigned long last_time = millis(); 
unsigned long current_time = millis();

void calculateMotorPowers() {
  if (fresh_imu_data_available == false){
    return;
  }
  fresh_imu_data_available = false;
  
  updateCurrentTimeVariables();

  // calculate orientation errors (error: difference between desired orientation and actual orientation)
  double rollError = desired_roll_angle - rollAngle;
  double pitchError = desired_pitch_angle - pitchAngle;
  double yawError = desired_yaw_angle_change - (yawAngle - prev_yawAngle);
    
  // calculate control gains based on errors
  roll_control_signal = getControlSignal(rollError, KP_roll_pitch, KI_roll_pitch, KD_roll_pitch, roll_pid_i, roll_last_error, ROLL_PITCH_INTEGRAL_LIMIT);
  pitch_control_signal = getControlSignal(pitchError, KP_roll_pitch, KI_roll_pitch, KD_roll_pitch, pitch_pid_i, pitch_last_error, ROLL_PITCH_INTEGRAL_LIMIT);
  yaw_control_signal = getControlSignal(yawError, KP_yaw, KI_yaw, KD_yaw, yaw_pid_i, yaw_last_error, YAW_INTEGRAL_LIMIT);

  // limit control gains
  roll_control_signal = constrain(roll_control_signal, -MAX_ROLL_PITCH_CONTROL_GAIN, MAX_ROLL_PITCH_CONTROL_GAIN);
  pitch_control_signal = constrain(pitch_control_signal, -MAX_ROLL_PITCH_CONTROL_GAIN, MAX_ROLL_PITCH_CONTROL_GAIN);
  yaw_control_signal = constrain(yaw_control_signal, -MAX_YAW_CONTROL_GAIN, MAX_YAW_CONTROL_GAIN);

  // calculate power for each motor
  frontLeftMotorPower = round(throttle + roll_control_signal + pitch_control_signal - yaw_control_signal);
  frontRightMotorPower = round(throttle - roll_control_signal + pitch_control_signal + yaw_control_signal);
  rearLeftMotorPower = round(throttle + roll_control_signal - pitch_control_signal + yaw_control_signal);
  rearRightMotorPower = round(throttle - roll_control_signal - pitch_control_signal - yaw_control_signal);

  reduceMotorPowers();

  ensureMotorsAlwaysRun();
 
  updateLastTimeVariables();
}

void reduceMotorPowers(){ // to preserve balance if MAX_THROTTLE limit exceeds)
  int maxMotorPower = max(max(frontLeftMotorPower, frontRightMotorPower), max(rearLeftMotorPower, rearRightMotorPower));
  if (maxMotorPower > MAX_THROTTLE){
    double power_reduction_rate = (double)maxMotorPower / (double)MAX_THROTTLE;
    frontLeftMotorPower = round((double)frontLeftMotorPower / power_reduction_rate);
    frontRightMotorPower = round((double)frontRightMotorPower / power_reduction_rate);
    rearLeftMotorPower = round((double)rearLeftMotorPower / power_reduction_rate);
    rearRightMotorPower = round((double)rearRightMotorPower / power_reduction_rate);
  }
}

void ensureMotorsAlwaysRun(){ // because it takes much more time to spin a stopped motor
  frontLeftMotorPower = max(frontLeftMotorPower, THROTTLE_START_POINT);
  frontRightMotorPower = max(frontRightMotorPower, THROTTLE_START_POINT);
  rearLeftMotorPower = max(rearLeftMotorPower, THROTTLE_START_POINT);
  rearRightMotorPower = max(rearRightMotorPower, THROTTLE_START_POINT);
}

void updateCurrentTimeVariables() {
  current_time = millis();
  delta_time = (current_time - last_time);
}

void updateLastTimeVariables() {
  last_time = current_time;
}

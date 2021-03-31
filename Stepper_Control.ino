
void setup() {
  // put your setup code here, to run once:
  pinMode(9,OUTPUT);
  pinMode(8, INPUT_PULLUP);
  TCCR1A=0x43;
  TCCR1B=0x1B;
  OCR1A=0xFFFF;
}

void loop() {
 float speed=0;
 if(digitalRead(8)){
    speed=.02;
  }
  doMotorSpeed(speed, .01);
  delay(100);
}

void doMotorSpeed(float motorSpeed, float accel){
      // Adjust PWM for desired Motor Speed
      // motorSpeed = Target motor speed in Rev/S
      // accel = Used acceleration rate in Rev/S/S
      // Frequency = Hz = (Rev/S)*200*16
      // Clock preScalar = 8Mhz/64 = 125Khz
      // 125Khz/0xFFFF = 1.9073Hz(minimum frequency)
      static float setPoint;
      static unsigned long prevTime;
      unsigned long nowTime = micros();
      accel = accel/(float)(1000000/(nowTime-prevTime));
      prevTime = nowTime;
      if(motorSpeed!=setPoint){
        float delta = motorSpeed-setPoint;
        if((abs(delta) > abs(accel))&(accel!=0)){
          // Ignore sign on acceleration
          // 0 acceleration means no acceleration
          delta = accel*(delta>0?1:-1);
        }
        setPoint += delta;
      }
      // Convert setPoint to Output Frequency
      float output = 65536;
      if(abs(setPoint)>0){
        output = (39.0625/abs(setPoint));
      }
      if(output>65535){ // Slower than minimum clock rate
        OCR1A = 0xFFFF; // Max Out Clock Rate
        if(motorSpeed==0){
          TCCR1A = 0x03; // Turn off outputs  
        }
      }else{
        OCR1A = (int)output; // Set Clock Rate from 0 to 65535
        TCCR1A = 0x43;
      }
}

// original code from https://www.lightofdawn.org/blog/?viewDetailed=00187#prettyPhoto
// small edits to stop a loop
// now stops when the cap has no more voltage
// set the min threshold on MA very low...
// reversed the pump and discharge pins to resistors dont have to cross one another

#include <avr/sleep.h>

// Measure LED forward voltage
// Measure by measuring residual voltage
// This one is accurate to +/- 0.05V

#define PUMP 4
#define LED_DISCHARGE 3
#define LED_SENSE A0
#define CURRENT_SENSE A2

#define CURRENT_RESISTANCE 130 // in ohm
#define FWD_CURRENT 20 // in mA
#define CAP 470 // in uF
#define CURRENT_THRESHOLD 0.1 // in mA

void setup() {
  pinMode(PUMP, OUTPUT);
  pinMode(LED_DISCHARGE, OUTPUT);
  pinMode(LED_SENSE, INPUT);
  pinMode(CURRENT_SENSE, INPUT);
  
  Serial.begin(9600);
  Serial.println("Wait");  
    
  digitalWrite(PUMP,HIGH); // charge
  digitalWrite(LED_DISCHARGE,HIGH); // block
  delay(1000);
  Serial.println("Start");
}



void loop() {
  int led_sense = analogRead(LED_SENSE);
  int current_sense = analogRead(CURRENT_SENSE);    
  int prev_sense;
  int prev_millis=millis();
    
  digitalWrite(PUMP, LOW); // discharge
  digitalWrite(LED_DISCHARGE,LOW); // unblock
  delay(5);

  Serial.println("VCap VLed VRes IRes ICap");

  while (1) {
    current_sense = analogRead(CURRENT_SENSE);    
    prev_sense = led_sense;
    led_sense = analogRead(LED_SENSE);

    // calc expected current
    long now = millis();
    long dt = (now - prev_millis);
    prev_millis = now;

    // both in mA
    float mea_current = (adc_to_voltage(current_sense)/CURRENT_RESISTANCE)*1000;    
    float calc_current = CAP * adc_to_voltage(prev_sense - led_sense) / dt;
    
    Serial.print("VCap:");
    Serial.print(adc_to_voltage(led_sense)); //vcap
    Serial.print(" ");
    Serial.print("VLed:");
    Serial.print(adc_to_voltage(led_sense - current_sense)); //vled
    Serial.print(" ");
    Serial.print("VRes:");
    Serial.print(adc_to_voltage(current_sense)); //vres
    Serial.print(" ");
    Serial.print("IRes:");
    Serial.print(mea_current); //Ires
    Serial.print(" ");
    Serial.print("ICap:");
    Serial.println(calc_current);
    if (mea_current < CURRENT_THRESHOLD or adc_to_voltage(led_sense) <0.01 ) break;
    delay(5);
  }

  Serial.println("Final VLed");
  Serial.println(adc_to_voltage(led_sense - current_sense));
  
  delay(1000);
  cli();
  sleep_enable();
  sleep_cpu();  
}

float adc_to_voltage(int adc) {
  return adc*5.0/1024;
}

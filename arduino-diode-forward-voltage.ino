// original code from https://www.lightofdawn.org/blog/?viewDetailed=00187#prettyPhoto
// small edits to stop a loop
// now stops when the cap has no more voltage
// changed the input and output pins so it can be used via a shield more easily

#include <avr/sleep.h>

// Measure LED forward voltage
// Measure by measuring residual voltage
// This one is accurate to +/- 0.05V

#define PUMP 7
#define LED_DISCHARGE 5
#define LED_SENSE A0
#define CURRENT_SENSE A1

#define CURRENT_RESISTANCE 200 // in ohm
#define FWD_CURRENT 20 // in mA
#define CAP 470 // in uF
#define CURRENT_THRESHOLD 1 // in mA

int current_state = 0;
float ma_diode_fv;

void setup() {
  pinMode(PUMP, OUTPUT);
  pinMode(LED_DISCHARGE, OUTPUT);
  pinMode(LED_SENSE, INPUT);
  pinMode(CURRENT_SENSE, INPUT);

  Serial.begin(9600);
  Serial.println("Wait");

  digitalWrite(PUMP, HIGH); // charge
  digitalWrite(LED_DISCHARGE, HIGH); // block
  delay(2000);
  Serial.println("Start");
  
}



void loop() {
  int led_sense = analogRead(LED_SENSE);
  int current_sense = analogRead(CURRENT_SENSE);
  int prev_sense;
  int prev_millis = millis();

  digitalWrite(PUMP, LOW); // discharge
  digitalWrite(LED_DISCHARGE, LOW); // unblock
  delay(3);

  //  Serial.println("VCap VLed VRes IRes ICap");

  while (1) {
    current_sense = analogRead(CURRENT_SENSE);
    prev_sense = led_sense;
    led_sense = analogRead(LED_SENSE);

    // calc expected current
    long now = millis();
    long dt = (now - prev_millis);
    prev_millis = now;

    // both in mA
    float mea_current = (adc_to_voltage(current_sense) / CURRENT_RESISTANCE) * 1000;
    float calc_current = CAP * adc_to_voltage(prev_sense - led_sense) / dt;

        Serial.print("VCap:");
        Serial.print(adc_to_voltage(led_sense)); //vcap
        Serial.print(" ");
    Serial.print("Diode FV:");
    float diode_fv = adc_to_voltage(led_sense -current_sense);
    Serial.print(diode_fv); //vled
    Serial.print(" ");
    //    Serial.print("VRes:");
    //    Serial.print(adc_to_voltage(current_sense)); //vres
    //    Serial.print(" ");
    Serial.print("Current:");
    Serial.println(mea_current); //Ires
    if (mea_current <= 3 && current_state == 0) {
      //      int mea_final = mea_current;
      ma_diode_fv = diode_fv;
//      ma_diode_fv = 5;
      current_state = 1;
    }
    //    Serial.print(" ");
    //    Serial.print("ICap:");
    //    Serial.println(calc_current);
    if (mea_current < CURRENT_THRESHOLD or adc_to_voltage(led_sense) < 0.01 or calc_current < 0.01 ) break;
    delay(3);
  }

  Serial.print("2ma Diode FV: ");
  Serial.println(ma_diode_fv);

  delay(1000);
  cli();
  sleep_enable();
  sleep_cpu();
}

float adc_to_voltage(int adc) {
  return adc * 5.0 / 1024;
}

// current_sensing_calibrated.ino
// Step 4 Phase 2: INA219 + voltage divider combined, CSV output
// CSV format: timestamp_ms, v_battery_V, current_mA, power_mW, bus_voltage_V

#include <Wire.h>
#include <Adafruit_INA219.h>

// ── Voltage sensing (from Step 3) ───────────────────────────────
const int   V_PIN       = A0;      // PF0, analogue pin
const int   NUM_SAMPLES = 16;
float SLOPE     = 1.620;            // Update with your calibrated value
float INTERCEPT = 0.000;

// ── Current sensing ─────────────────────────────────────────────
Adafruit_INA219 ina219;             // Address 0x40

// ── Safety thresholds ───────────────────────────────────────────
const float OVERCURRENT_mA  = 2000.0;   // 2A — firmware warning only in Step 4
const float CC_CV_THRESHOLD = 7.30;     // V — used from Step 11 onward

float readBatteryVoltage() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(V_PIN);
    delayMicroseconds(200);
  }
  float v_adc = ((float)sum / NUM_SAMPLES / 1024.0) * 5.0;
  return (v_adc * SLOPE) + INTERCEPT;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  if (!ina219.begin()) {
    Serial.println("FAILED: INA219 not found at 0x40");
    while (1) { delay(10); }
  }
  // Print CSV header — matches sensor_data.csv format
  Serial.println("timestamp_ms,v_battery_V,current_mA,power_mW,bus_voltage_V,status");
}

void loop() {
  float v_bat      = readBatteryVoltage();
  float current_mA = ina219.getCurrent_mA();
  float power_mW   = ina219.getPower_mW();
  float bus_V      = ina219.getBusVoltage_V();

  String status = "OK";
  if (current_mA > OVERCURRENT_mA) status = "OVERCURRENT";
  if (v_bat >= CC_CV_THRESHOLD)    status = "AT_FULL_CHARGE";
  if (v_bat < 5.20)                status = "LOW_BATTERY";

  Serial.print(millis());      Serial.print(",");
  Serial.print(v_bat, 4);      Serial.print(",");
  Serial.print(current_mA, 2); Serial.print(",");
  Serial.print(power_mW, 2);   Serial.print(",");
  Serial.print(bus_V, 3);      Serial.print(",");
  Serial.println(status);

  delay(1000);
}

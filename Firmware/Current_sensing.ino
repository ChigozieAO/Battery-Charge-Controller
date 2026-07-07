// current_sensing_basic.ino
// Step 4 Phase 1: Confirm INA219 communication over I2C
// If 'Failed to find INA219 chip' appears: check SDA=pin20, SCL=pin21, VCC=5V, GND common

#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;   // Default I2C address 0x40

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== INA219 Current Sensor - Step 4 Phase 1 ===");

  if (!ina219.begin()) {
    Serial.println("FAILED: Cannot find INA219 at 0x40");
    Serial.println("Check: SDA=pin20, SCL=pin21, VCC=5V, GND common");
    while (1) { delay(10); }   // Halt — do not proceed with bad sensor
  }
  Serial.println("INA219 found. Calibration: 32V / 2A range.");
  // Default: setCalibration_32V_2A() — max 2A, 0.1mA resolution
}

void loop() {
  float shuntvoltage_mV = ina219.getShuntVoltage_mV();
  float busvoltage_V    = ina219.getBusVoltage_V();
  float current_mA      = ina219.getCurrent_mA();
  float power_mW        = ina219.getPower_mW();
  float loadvoltage_V   = busvoltage_V + (shuntvoltage_mV / 1000.0);

  Serial.println("----------------------------------------");
  Serial.print("Shunt Voltage : "); Serial.print(shuntvoltage_mV, 2); Serial.println(" mV");
  Serial.print("Bus Voltage   : "); Serial.print(busvoltage_V, 3);    Serial.println(" V");
  Serial.print("Load Voltage  : "); Serial.print(loadvoltage_V, 3);   Serial.println(" V");
  Serial.print("Current       : "); Serial.print(current_mA, 2);      Serial.println(" mA");
  Serial.print("Power         : "); Serial.print(power_mW, 2);        Serial.println(" mW");

  delay(1000);
}
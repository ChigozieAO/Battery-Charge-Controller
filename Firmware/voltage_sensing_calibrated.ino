const int V_PIN = A0;
const int NUM_SAMPLES = 16;      // Average 16 readings to reduce noise
                                 // Each analogRead ~110us -> 16 samples ~1.76ms total

float SLOPE     = 1.620;         // Scaling factor (close to (R1+R2)/R2 = 1.62)
float INTERCEPT = 0.000;         // Offset correction in Volts
float raw_avg = 0.0;

float readBatteryVoltage() {
  long sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogRead(V_PIN);
    delayMicroseconds(200);      // Allow ADC mux to settle between reads
  }
  raw_avg = sum / (float)NUM_SAMPLES;

  // Convert averaged ADC count to actual battery voltage:
  // Step 1: ADC count -> voltage at A0 pin

  float v_adc = (raw_avg / 1024.0) * 5.0;

  // Step 2: Undo the voltage divider scaling, apply calibration
  float v_bat = (v_adc * SLOPE) + INTERCEPT;

  return v_bat;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("timestamp_ms,raw_avg,v_adc_V,v_battery_V");  // CSV header
}

void loop() {
  float voltage = readBatteryVoltage();

  float v_adc = (raw_avg / 1024.0) * 5.0;
  // Print as CSV — matches sensor_data.csv format from your first project
  Serial.print(millis());
  Serial.print(",");
  Serial.print(raw_avg, 2);           // Raw average
  Serial.print(",");
  Serial.print(v_adc, 4);             // Voltage at Arduino pin
  Serial.print(",");
  Serial.print(voltage, 4);           // Final estimated battery voltage
  Serial.println();

  delay(1000);   // 1 reading per second
}


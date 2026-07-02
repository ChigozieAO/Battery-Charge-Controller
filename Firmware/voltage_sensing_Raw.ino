const int V_PIN = A0;        // Analogue pin connected to voltage divider junction
const float VREF = 5.0;      // Nominal reference — will be corrected in Phase 2
const float R1 = 6200.0;     // Upper resistor in ohms
const float R2 = 10000.0;    // Lower resistor in ohms
const float SCALE = (R1 + R2) / R2;   // = 1.62

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("--- Voltage Sensing Step 3: Raw Reading ---");
}

void loop() {
  int raw = analogRead(V_PIN);
  float v_adc = (raw / 1024.0) * VREF;
  float v_bat = v_adc * SCALE;

  Serial.print("RAW: ");
  Serial.print(raw);
  Serial.print("  V_adc: ");
  Serial.print(v_adc, 4);
  Serial.print(" V  V_bat: ");
  Serial.print(v_bat, 4);
  Serial.println(" V");

  delay(1000);
}
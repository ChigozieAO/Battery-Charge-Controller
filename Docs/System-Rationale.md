# System Design Rationale
**Battery-Charge-Controller**

---

## 1. Battery Chemistry: Why LFP

This project uses **Lithium Iron Phosphate (LFP / LiFePO₄)** cells in the 18650 cylindrical format.

LFP is selected over lead-acid and NMC for three concrete reasons:

**Safety.** LFP has a thermal runaway onset temperature of approximately 270°C, compared to 150–200°C for NMC and ~120°C for lead-acid. For a bench prototype without active cooling or a dedicated protection PCB, this margin is the decisive factor.

**Cycle life.** LFP delivers 2,500–7,000+ charge cycles at 80% Depth of Discharge (DoD). Lead-acid delivers 300–1,000 cycles at 50% DoD. This project is a long-term research instrument — the battery should outlive the experimentation.

**CC-CV compatibility.** LFP charges cleanly with a two-stage CC-CV profile and requires no float charge. Its relatively flat discharge curve between 20–80% SoC is a known characteristic that makes Coulomb counting the correct primary SoC estimation method (the flat OCV curve makes voltage-only estimation unreliable in that range, motivating current integration).

**Cell format:** 18650 (18 mm diameter, 65 mm length). Widely available, standardised, and compatible with standard cell holders. A 1S configuration (3.2 V nominal, 3.65 V max) is used for initial testing. A 2S configuration (6.4 V nominal, 7.3 V max) may be used in later test phases.

| Parameter | Lead-Acid | **LFP (selected)** | NMC |
|---|---|---|---|
| Nominal voltage | 2.0 V/cell | **3.2 V/cell** | 3.6–3.7 V/cell |
| Cycle life (80% DoD) | 300–1,000 | **2,500–7,000+** | 500–2,000 |
| Round-trip efficiency | 75–85% | **92–96%** | 90–95% |
| Thermal runaway temp. | ~120°C | **~270°C** | ~150–200°C |
| Charging algorithm | Multi-stage + float | **CC-CV (no float)** | CC-CV (no float) |

---

## 2. Charging Profile: CC-CV

The **Constant Current / Constant Voltage (CC-CV)** algorithm is implemented in firmware.

**Stage 1 — Constant Current (CC):**
The charger delivers a fixed current (0.5C target, approximately 1 A for a 2 Ah cell) while the battery terminal voltage rises toward the cutoff threshold of **3.65 V/cell**. This phase delivers approximately 70–80% of total cell capacity.

**Stage 2 — Constant Voltage (CV):**
Once the terminal voltage reaches 3.65 V, the firmware holds voltage constant and allows current to taper exponentially. Charging terminates when current drops below the termination threshold (0.05C, approximately 100 mA).

**Safety cutoffs (interrupt charging unconditionally):**
- Overvoltage: terminal voltage exceeds 3.70 V (control loop failure)
- Overcurrent: charging current exceeds 2.5 A (hardware fault)
- Overtemperature: battery surface temperature exceeds 45°C

The CC-CV profile is implemented as a closed-loop PWM control system: the firmware reads the INA219 current sensor each control cycle, computes the error relative to the CC setpoint, adjusts PWM duty cycle, and checks the voltage sensor for the CC→CV transition condition. Control loop update rate: 10 Hz.

---

## 3. Block Diagram

See `docs/block-diagram.png` (generated separately).

The system consists of four logical layers:
- **Power path:** DC source → MOSFET switch (IRLZ44N) → LFP battery
- **Sensing layer:** Voltage divider (V), INA219 (I), DS18B20 (T) — all feeding into the microcontroller
- **Control layer:** Arduino Mega 2560 — reads sensors, runs CC-CV algorithm, drives PWM gate signal
- **Data layer:** Serial UART → Python logger → CSV → dashboard

---

## 4. References

| Source | Used for |
|---|---|
| Battery University, BU-409 (batteryuniversity.com) | LFP charging profile, CC-CV termination conditions |
| BloombergNEF 2025 Battery Price Survey | LFP cost data |
| Texas Instruments, SLUA887 | CC-CV algorithm implementation |
| Movassagh et al., *Energies* 14(14), 2021 | Coulomb counting limitations and OCV calibration |
| Maroșan et al., *Proc. Manufacturing Systems* 18(2), 2023 | INA219 vs ACS712 accuracy comparison |
| Infineon IRLZ44N Datasheet | MOSFET gate drive and switching specifications |

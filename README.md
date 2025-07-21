# Heater-Control-System
The goal is to simulate or build a basic embedded system that turns a "heater" on or off based on temperature thresholds.
*simulator code and non simulator code to understand the changing of state and temperature is observed continuously.*

In this page youll find various versions of the heater. Created 3 versions of the code.
before that, in Wokwi, diagram.json is the single file that defines your entire virtual circuit—every component, its position, and every wire connection. When you place parts or route wires in the editor, Wokwi updates that JSON so it can recreate and simulate your exact hardware setup anytime or share it with others.

Wokwi also includes a built‑in set of popular Arduino libraries (e.g. Wire, LiquidCrystal_I2C, DHT, FreeRTOS, BLE). To add anything else, open the Libraries panel, search the Arduino Library Manager for the one you need, click install, and then #include it in your sketch—just like on real hardware.
First copy the diagram code into diagram.json file in wokwi

Basic Heater (Sensor‐Driven):
An ESP32 reads the DHT22 every 0.5 s and, based on five thresholds, automatically switches a relay, LEDs, buzzer, and a 10‑segment bargraph. Below 20 °C it heats; 20–28 °C it “stabilizes”; exactly 28 °C it shows “target reached”; above 45 °C it alarms “overheat”; otherwise it idles. The LCD and serial log the temperature and state.

Non‑Simulated Heater:
Using FreeRTOS, the ESP32 reads actual DHT22 temperatures every second and applies the same state logic (Heating, Stabilizing, Target OK, Overheat). Outputs (relay, LEDs, buzzer, bargraph) update only on state changes, and a BLE task broadcasts the current state every 3 s. To test different starting points, simply edit the "temperature" field in your Wokwi JSON.

Simulator Sketch:
Without a physical sensor, two RTOS tasks simulate and control heating: one adds 2 °C to an internal currentTemp every 2 s; the other evaluates that value every 200 ms to switch states, drive indicators, and update the LCD and bargraph. Over 45 °C it alarms; 40–45 °C shows “target” and idle LED; 30–40 °C is “stabilizing”; 0–30 °C is “heating”; and exactly 0 °C is “idle.” The main loop() remains idle, letting FreeRTOS handle all timing.

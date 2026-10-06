# One Robot, Endless Possibilities | Speak, Control, Avoid

A multi-mode Arduino robot that supports:

- **Speak** ΓÇö voice commands sent over Bluetooth/Serial (`F`, `B`, `L`, `R`, `S`)
- **Control** ΓÇö manual remote driving via the same serial command set
- **Avoid** ΓÇö autonomous ultrasonic obstacle avoidance with servo scanning

## Hardware

- Arduino + Adafruit Motor Shield (`AFMotor`)
- 4├ù DC motors
- HC-SR04 ultrasonic sensor (`TRIG` ΓåÆ A1, `ECHO` ΓåÆ A0)
- Servo on pin D9 for left/right scanning

## Firmware

Open `one-robot-speak-control-avoid.ino` in the Arduino IDE.

### Libraries

- `Servo` (built-in)
- [Adafruit Motor Shield library](https://github.com/adafruit/Adafruit-Motor-Shield-library) (`AFMotor`)

### Serial commands

| Command | Action |
|---------|--------|
| `F` or `^` | Forward |
| `B` or `-` | Backward |
| `L` or `<` | Turn left |
| `R` or `>` | Turn right |
| `S` or `*` | Stop |

When no serial command is available, the robot runs autonomous obstacle avoidance (stop / reverse / scan / turn toward open space when distance Γëñ 12 cm).

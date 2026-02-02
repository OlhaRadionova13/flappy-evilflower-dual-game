# 🐤 Flappy Bird on NeoPixel Matrix (Arduino)

A small Arduino game inspired by **Flappy Bird**, running on a **32×8 NeoPixel LED matrix**, with smooth physics, score counter, and a parallel LCD menu.

✅ **Wokwi simulation:** https://wokwi.com/projects/453918368819949569

---

## 🎮 Features

- 32×8 NeoPixel LED matrix (WS2812B / NeoPixel)
- Smooth bird physics (velocity + gravity)
- Single-column pipes
- Score counter (top-left corner)
- Game Over screen: **GO**
- Start screen (game starts only on button press)
- LCD menu works in parallel with the game
- Fully non-blocking code (`millis()` based)

---

## 🧩 Hardware Requirements

- Arduino Uno / Nano
- 32×8 NeoPixel LED matrix (WS2812B)
- 16×2 LCD (HD44780 compatible)
- 5× push buttons
- Breadboard & jumper wires
- External 5V power supply (recommended for NeoPixels)

---

## 🔌 Pin Mapping

### NeoPixel Matrix
| Signal | Arduino Pin |
|------|-------------|
| DIN  | D4 |
| 5V   | 5V (external recommended) |
| GND  | GND |

### Game Button
| Purpose | Arduino Pin |
|-------|-------------|
| Jump / Start | D13 |

### LCD (4-bit mode)
| LCD Pin | Arduino Pin |
|-------|-------------|
| RS | D8 |
| E  | D9 |
| D4 | D5 |
| D5 | D6 |
| D6 | D7 |
| D7 | D3 |

### Menu Buttons
| Button | Arduino Pin |
|-------|-------------|
| BTN1 | A0 |
| BTN2 | A1 |
| BTN3 | A2 |
| BTN4 | A3 |

---

## 🛠 Build & Setup

Step-by-step guide:  
📄 `docs/setup.md`  
Wiring details:  
📄 `docs/wiring.md`  
Game logic explanation:  
📄 `docs/gameplay.md`

---

## 🤝 Contributors

- **Olha Radionova**, **David Kirakosian** — project author, hardware & game design  


---

## 📜 License

MIT License.

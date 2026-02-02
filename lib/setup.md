# ✅ Setup (Step-by-step)

This guide explains how to:
1) prepare the project locally
2) upload to Arduino
3) publish on GitHub

---

## 1) Create folder structure

Create a new folder:

`flappy-neopixel-arduino`

Inside create:

- `src/`
- `docs/`
- `assets/` (optional)

---

## 2) Add the Arduino code

Create file:

`src/flappy_neopixel_lcd.ino`

Paste the full project code into this file.

---

## 3) Install Arduino libraries

Open Arduino IDE → Tools → Manage Libraries

Install:
- `Adafruit NeoPixel`
- `LiquidCrystal` (usually built-in)

---

## 4) Wire the hardware

Follow:
`docs/wiring.md`

Important:
- If using external 5V for NeoPixels: **connect GND of PSU to Arduino GND**
- DIN must go to **D4**
- Jump button must go to **D13**, other side to **GND** (INPUT_PULLUP)

---

## 5) Upload to Arduino

Arduino IDE:
- Tools → Board → (Uno / Nano)
- Tools → Port → select correct COM port
- Click Upload

---

## 6) Create GitHub repository

On GitHub:
- New repository
- Name: `flappy-neopixel-arduino`
- Public
- Do NOT initialize with README (we already have one)

---

## 7) Push code using Git

In your project folder:

```bash
git init
git add .
git commit -m "Initial release: Flappy Bird on NeoPixel Matrix"
git branch -M main
git remote add origin https://github.com/<YOUR_USERNAME>/flappy-neopixel-arduino.git
git push -u origin main

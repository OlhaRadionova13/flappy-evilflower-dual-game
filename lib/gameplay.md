# 🧠 Gameplay & Code Explanation

## Parallel tasks
The sketch runs two independent tasks without blocking delays:
- `taskLCD()` — LCD menu, scrolling text, 4 menu buttons
- `taskFlappy()` — NeoPixel game

Both are driven by `millis()` timers.

---

## Game states
- START: initial picture shown, waits for D13 press
- PLAYING: pipes move, gravity applies, jump impulse on D13 press
- GO: shows "GO", then returns to START

---

## Bird physics
- `birdVy` — vertical velocity
- `GRAVITY` accelerates bird downward
- `JUMP_IMPULSE` pushes bird up on press
- `MAX_FALL` clamps falling speed for better control

---

## Score
The score increases when a pipe passes the bird’s X position.
Displayed on the top-left corner using a 3×5 custom digit font.

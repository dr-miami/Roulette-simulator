# Slot Machine (C++)

A terminal-based slot machine game built in C++17[cite: 6]. Features animated spinning reels[cite: 4, 5], dynamic audio effects via `miniaudio`[cite: 4, 5], and weighted RNG symbol payouts[cite: 4, 6].

---

## Features

* **Animated Reel Spins:** Visual reel animation rendered directly in your terminal[cite: 4, 5].
* **Cross-Platform Audio:** Integrated background music and sound effects using `miniaudio` (PulseAudio / ALSA)[cite: 4, 5].
* **Weighted Probabilities:** Uses `std::discrete_distribution` for realistic symbol landing odds[cite: 4, 6].
* **Wild Symbol Mechanics:** Wilds substitute for high-matching symbol combinations and trigger mega jackpots[cite: 4, 5].

---

## Audio Assets

Place the following WAV audio files in the same directory as your compiled executable[cite: 5]:

* `sound.wav` — Background music (loops during gameplay)[cite: 5]
* `spin.wav` — Reel spin animation effect[cite: 5]
* `win.wav` — Winning spin / Jackpot sound effect[cite: 5]
* `loss.wav` — Losing spin sound effect[cite: 5]

---

## Building & Running

### Linux System Dependencies
Install system sound headers required for `miniaudio` compilation:

```bash
# Ubuntu / Debian / Pop!_OS
sudo apt update && sudo apt install libasound2-dev libpulse-dev

# Arch Linux
sudo pacman -S alsa-lib libpulse

# Fedora
sudo dnf install alsa-lib-devel pulseaudio-libs-devel

```

### Compilation

**Linux (GCC / Clang):**

```bash
g++ -std=c++17 main.cpp -lpthread -ldl -lm -lasound -lpulse -o Slots
./Slots

```

**Windows (MSVC / MinGW):**

```bash
g++ -std=c++17 main.cpp -o Slots.exe
Slots.exe

```

---

## How to Play

1. Start with an initial balance of **$1000**.


2. Enter your **bet amount** for the round (must be between $1 and your available balance).


3. The 3 reels spin and land on weighted random symbols.


4. Payouts are awarded based on left-to-right matching symbols or **Wild** substitutions.


5. Enter **`y`** to play another round or **`n`** to cash out and exit.



---

## Symbols & Payouts

Payouts are awarded as bet multipliers based on matching 2 or 3 symbols:

| Symbol | 2 Matches | 3 Matches | Weight | Approx. Chance / Reel |
| --- | --- | --- | --- | --- |
| **Wild** | x40

 | x200

 | 2

 | ~2%

 |
| **Man** | x30

 | x100

 | 3

 | ~3%

 |
| **Woman** | x20

 | x75

 | 5

 | ~5%

 |
| **Dog** | x20

 | x75

 | 5

 | ~5%

 |
| **Bag of Money** | x15

 | x50

 | 8

 | ~8%

 |
| **Bell** | x15

 | x50

 | 8

 | ~8%

 |
| **A** | x10

 | x30

 | 12

 | ~12%

 |
| **K** | x8

 | x20

 | 14

 | ~14%

 |
| **Q** | x8

 | x20

 | 14

 | ~14%

 |
| **J** | x5

 | x15

 | 14

 | ~14%

 |
| **10** | x5

 | x15

 | 15

 | ~15%

 |

---

## Project Structure

```
.
├── main.cpp        # Game execution loop, bet input, audio engine[cite: 5, 6]
├── functions.hpp   # Reel animation, symbol definitions, UI helpers[cite: 4, 6]
└── miniaudio.h     # Single-header audio library[cite: 4, 5]

```

```

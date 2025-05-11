# Racing Simulator

---

## Download Instructions

1. Go to the **[Releases](../../releases)** tab (on the right, under the "About" section).
2. Download `Racing-Game-3.build.zip`.
3. Extract the ZIP to any folder of your choice.
4. Inside the extracted folder, open the `Executable` folder and run `RacingGame.exe`.
5. You may see a **Microsoft Defender SmartScreen** warning. Click **More info**, then **Run anyway**.  
   > SmartScreen uses reputation-based filtering and does **not** scan for viruses. If you're concerned, feel free to scan the file on [VirusTotal](https://www.virustotal.com) or using any antivirus software.

---

## Input Information

### Keyboard Controls

> **Note:** Driving with a keyboard is supported but less precise.

| Action        | Key       |
|---------------|-----------|
| Throttle      | `W`       |
| Brake         | `S`       |
| Steer Left    | `A`       |
| Steer Right   | `D`       |
| Shift Down    | `O`       |
| Shift Up      | `P`       |

### Controller Controls

> Any generic controller should work (via SDL2).  
> Example below uses PlayStation-style mappings.

| Action        | Button              |
|---------------|---------------------|
| Throttle      | Left Trigger (L2)   |
| Brake         | Right Trigger (R2)  |
| Steering      | Left Stick (X-axis) |
| Shift Down    | `X` (Cross)         |
| Shift Up      | `☐` (Square)        |

- If a controller is connected, it will **vibrate** during loss of traction - even when playing on keyboard.

### Additional Controls

| Action            | Key       |
|-------------------|-----------|
| Open Settings Menu| `Esc`     |
| Reset Car         | `Space`   |
| Cycle Cameras     | `Tab`     |
| Slow Motion       | `B`       |
| Normal Time       | `N`       |
| Pause Time        | `M`       |

---

## Gameplay Information

- **Collisions** are disabled (e.g., you can drive through walls or fall through the floor) for stability. This is common in some professional simulators.
- **Suspension behavior** may cause the car to fall through the floor. Press `Space` to reset the car.
- Some **curbs** (especially at the final chicane) can launch the car - avoid them.
- The **start/finish line** has misplaced model geometry causing car bouncing - try to steer around the odd-colored start marks.
- **Grip** is uniform across all surfaces. Differentiated grip for off-track areas is out of scope for this version.
- **Tire screeching** can be heard when the tires spin under throttle or lock under braking.

---

## Tips for New Players

- Power is delivered **only to the rear tires**, so ease onto the throttle on corner exit to avoid spinning.
- Braking while turning can **lock up front tires**, reducing grip. Try **trail braking** (ease off the brake as you turn in).
- **Upshift** when the rev bar reaches the end - otherwise the car won't accelerate further.
- Choose **brake markers** (e.g., trackside boards) and adjust them as your skills improve.
- **Engine sound** can help you shift - a higher pitch means it's time to upshift.
- In tricky corners, try using a **higher gear** to reduce wheelspin and improve traction.

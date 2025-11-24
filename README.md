# Snake Game for DE10-Lite FPGA

A classic Snake game implementation for the DE10-Lite FPGA board with VGA output.

## Features

- **320x240 VGA Display** with custom graphics driver
- **7-Segment Display** showing real-time score (4 digits)
- **LED Feedback** showing snake length (LEDs 0-9)
- **Pause/Resume** functionality
- **Two Wall Modes**: wrap-around or deadly walls
- **Variable Speed**: normal and fast modes
- **Color-coded Snake**: cyan head, green body, red food

## Controls

### Switches (SW0-SW9)
- **SW0** - Pause/Resume toggle
- **SW7** - Wall mode (OFF=wrap around, ON=deadly walls)
- **SW8** - Speed control (OFF=normal, ON=fast)
- **SW9** - Reset game

### Push Buttons (KEY0-KEY3, Active Low)
- **KEY0** - Move Up
- **KEY1** - Move Down
- **KEY2** - Move Left
- **KEY3** - Move Right

## Gameplay

1. **Starting**: The snake starts at the center moving right
2. **Movement**: Use the push buttons to change direction
3. **Food**: Collect red blocks to grow and score points
4. **Score**: Displayed on screen and 7-segment displays
5. **Game Over**: Occurs when:
   - You hit yourself
   - You hit a wall (if SW7 is ON)

## Display Layout

```
┌─────────────────────────────────────┐
│ SCORE: XXXX            SNAKE        │ ← Header (20px)
├─────────────────────────────────────┤
│                                     │
│         [Snake Game Area]           │
│            20x13 Grid               │
│         16x16 pixel blocks          │
│                                     │
└─────────────────────────────────────┘
```

## Building

```bash
cd src
make clean
make
```

## Hardware Requirements

- DE10-Lite FPGA Board
- VGA Monitor (320x240 @ 60Hz)
- VGA cable

## Memory Map

- **VGA Buffer**: 0x08000000 (320x240 pixels, 8-bit color)
- **LEDs**: 0x04000000
- **Switches**: 0x04000010
- **Push Buttons**: 0x040000D0
- **7-Segment**: 0x04000050

## Tips

1. **Score Display**: Score updates continuously on both VGA and 7-segment
2. **Pause Feature**: Press SW0 to pause, "PAUSED" appears on screen
3. **Speed Adjustment**: Switch SW8 anytime to change game speed
4. **Wall Mode**: Try both modes - wrap-around is easier!
5. **Reset**: Hold SW9 to restart with a fresh game

## Color Palette (RGB 3-3-2)

- Background: Black (0x00)
- Snake Head: Cyan (0x1F)
- Snake Body: Green (0x1C)
- Food: Red (0xE0)
- Text: White (0xFF) / Yellow (0xFC)

## Technical Details

- **Grid Size**: 20 columns × 13 rows
- **Block Size**: 16×16 pixels
- **Max Snake Length**: 260 blocks
- **Frame Delay**: 400ms (normal) / 150ms (fast)
- **Font**: Custom 5×7 pixel bitmap font

## Known Issues

- Maximum score display is 9999 (4 digits)
- Food occasionally spawns at edges (cosmetic issue)

## Future Improvements

- [ ] Add timer interrupt for smoother gameplay
- [ ] Implement difficulty levels
- [ ] Add sound effects
- [ ] High score persistence
- [ ] Multiple food types with different values

## License

See COPYING file for license information.

## Authors

Based on DTEK-V framework by Artur Podobas, Wiktor Szczerek, and Pedro Antunes.
Snake game implementation: [Your Name]
---
title: Debugging
description: Fix scrambled, upside-down or glitching panels, and other common problems.
order: 2
group: Setup
---

Here are some common issues

- **Upside down panels or wrong order.** [Check the panels](#check-the-panels)
- **Glitching panels or incorrect rendering.** [Fix glitching panels](#fix-glitching-panels)
- **The code won't compile after downloading new files.** Check `Config.h` and `Protogen_Faces.h` came from the same version of Toaster Oven, and that your copy of Toaster OS is up to date.
- **"Low memory available" when compiling.** The Nano Every has 6 KB of memory. If you've added lots of expressions or animation frames, remove some you don't use.





## Check the panels

If the face looks scrambled, some panels are probably in the wrong order or upside down. Toaster OS has a debug mode to help:

1. In Toaster Oven, go to **Features → General** and set **Debug mode** to **Render numbers on all panels**. Download `Config.h` and upload again.
2. Each panel now shows its position in its chain (0, 1, 2 and so on), with a dot in its top outer corner.
3. Check every number reads the right way up, with the dot at the top. If a panel's dot is at the bottom, change **Upside down** for it on the Panels page. If the numbers are in the wrong places, fix the panel order.
4. Set **Debug mode** back to **Off** when you're done.

Numbering goes from 0 to 9, then A, B, C, D, E, F.

### Working example

In this example, I have two connections: one for the left side, one for the right. If you have a single connection, then you will get different numbers (e.g. 0-6 on the right, then 7-13 on the left).

![Left side](images/debug-panels-left.jpg)

![Right side](images/debug-panels-right.jpg)

## Fix glitching panels

If panels are flickering, not showing anything, fully lit up, showing random patterns or otherwise not working properly, follow these steps.

This software comes with a feature to "auto-recover" panels with loose connections. So you may see the panel fix itself every few seconds, before glitching again.

### Overview

Loose connections on MAX7219 panels can be very difficult to debug. A problem in a chain of panels can cause **any of the panels** to glitch out, not just the one with the loose connections.

You first have to test a small number of panels, in different orders, wiggling the cables, to ensure their inputs and outputs work correctly. Then gradually introduce other panels.

### Example

![Debug steps](images/debug-loose-connections.jpg)

1. In this example, the purple pair of panels was glitching
   1. This could be a problem with the purple panels
   2. Or the output of the orange panels
   3. Or a problem somewhere else in the chain
2. Swapping the purple and green panels resulted in the green panels glitching
   1. This makes it less likely that the problem is with the purple panels
   2. It seems most likely to be a problem with the orange panels
   3. It could still be a problem somewhere else in the chain
3. Moving both purple and green panels to the front resulted in no glitches
   1. The orange panel output seems like the most likely cause now

To test further:

- We could swap the blue and orange panels and see if the blue panel glitches
- Or have blue -> green -> purple and see if anything glitches

To fix:

- Check and resolder the pins for the problem panels

**REMEMBER**: You may have multiple problems with your panels! It's worth having spare panels.\


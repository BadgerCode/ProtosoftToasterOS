---
title: Managing expressions
description: Draw, animate and organise your protogen's expressions in Toaster Oven.
order: 3
group: Setup
---

Your protogen's expressions are made on the **Expressions** page of [Toaster Oven](/configure/expressions), and saved in `Protogen_Faces.h`.

## The expressions list

The list on the left shows every expression. The **default** expression, the one the head shows most of the time, is always at the top, and the rest are in alphabetical order.

- **+ New** adds a blank expression.
- **Duplicate** copies the selected one.
- **Remove** deletes it. You can't remove the default expression: set another as the default first.
- **Set as default** makes the selected expression the default.

To rename an expression, click the pencil next to its name. Names can use letters, numbers and spaces.

An expression with a lock is used by the firmware itself (the heart, for rubbing the nose). You can change how it looks, but not rename or remove it.

## Drawing

Draw the **right side** of the face, as seen from the front. The left side is mirrored automatically.

- Choose **Draw** or **Erase**, or press **D** or **E**.
- Right-click, or hold **Alt**, to erase while drawing.
- **Undo** and **Redo** (**Ctrl+Z** and **Ctrl+Y**, or **⌘** on a Mac) undo your drawing, and changes to blinks and animations.

The **Whole face** preview below shows both sides together.

## Blinking

**Add blink** gives the expression a second set of eyes, shown briefly every 10–15 seconds. Switch between **Eyes open** and **Blink** to draw each one, or **Remove blink** to stop it blinking.

## Animations

An animated expression loops through a set of frames, like the spinning spiral or the beating heart.

1. Click **Add animation**. This adds a second frame, and selects it.
2. Draw on each frame as a whole face. **Frame 1** is the expression itself. On other frames, anything you don't change follows frame 1, so editing frame 1 updates them too.
3. Set how long each frame shows, in milliseconds, under its picture.
4. Use **+ Add frame** to copy the selected frame, **Move earlier** and **Move later** to reorder them, and **Remove frame** to delete one.

The preview plays the animation. Animated expressions don't blink.

## Expression settings

Below the preview, each expression has two settings:

- **Show in place of the default expression.** Every 10–14 seconds, and after a boop, the head swaps the default expression for a random one of these for 5 seconds. Turn it off for every expression to always show the default.
- **Movement speed.** How often the face moves up and down while this expression shows: every 200 ms (normal), 300 ms (slow) or 600 ms (very slow). You can turn this movement off on **Features → General**.

Expressions are also used by the [boop sensor](/guides/toaster-os/boop-sensor) and [remote control](/guides/toaster-os/remote-control), which you set up on the **Features** page.

## Saving your expressions

Toaster Oven doesn't save anything in your browser yet, so download `Protogen_Faces.h` before you leave the page. Put it in the `ToasterOS` folder and upload to the Arduino.

To carry on later, use **Import** to load the file you downloaded.

> [!TIP]
> Every expression and frame uses some of the Arduino's 6 KB of memory. If the Arduino IDE warns about low memory, remove expressions you don't use.

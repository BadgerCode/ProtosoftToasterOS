<script setup lang="ts">
import { FACE_NEUTRAL } from '@/faces/faces'

// The neutral right eye (front + back panels), as a 16x8 LED logo
const [front, back] = FACE_NEUTRAL.eye
const leds: { x: number; y: number }[] = []
for (let row = 0; row < 8; row++) {
  const bits = (front[row]! << 8) | back[row]!
  for (let col = 0; col < 16; col++) {
    if (bits & (0x8000 >> col)) leds.push({ x: col, y: row })
  }
}
</script>

<template>
  <svg viewBox="0 0 16 8" class="h-4 w-8 overflow-visible" aria-hidden="true">
    <circle
      v-for="led in leds"
      :key="`${led.x}-${led.y}`"
      :cx="led.x + 0.5"
      :cy="led.y + 0.5"
      r="0.42"
      class="fill-led"
      style="filter: drop-shadow(0 0 0.6px var(--color-led))"
    />
  </svg>
</template>

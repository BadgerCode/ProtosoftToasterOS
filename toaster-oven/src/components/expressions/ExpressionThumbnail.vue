<script setup lang="ts">
import { computed } from 'vue'
import type { FaceExpressionData } from '@/faces/faces'
import { EDITABLE_PANELS, LAYOUT_HEIGHT, LAYOUT_WIDTH, isLit } from './expression-layout'

// A small picture of the right side of an expression, for lists

const props = defineProps<{ face: FaceExpressionData }>()

/** One square per lit LED, as a single path to keep lists light */
const path = computed(() => {
  let d = ''
  for (const panel of EDITABLE_PANELS) {
    const bitmap = panel.bitmap(props.face, false)
    for (let row = 0; row < 8; row++) {
      for (let col = 0; col < 8; col++) {
        if (isLit(bitmap, row, col)) d += `M${panel.x + col} ${panel.y + row}h0.9v0.9h-0.9z`
      }
    }
  }
  return d
})
</script>

<template>
  <svg :viewBox="`0 0 ${LAYOUT_WIDTH} ${LAYOUT_HEIGHT}`" class="block" aria-hidden="true">
    <path :d="path" class="fill-led" />
  </svg>
</template>

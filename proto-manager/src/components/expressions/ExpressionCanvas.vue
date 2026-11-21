<script setup lang="ts">
import { computed, ref } from 'vue'
import type { FaceExpressionData } from '@/faces/faces'
import {
  EDITABLE_PANELS,
  LAYOUT_HEIGHT,
  LAYOUT_WIDTH,
  isLit,
  ledAt,
  setLed,
} from './expression-layout'

// Paint LEDs on the right side of the face with the mouse, pen or finger.

const props = defineProps<{
  face: FaceExpressionData
  /** Edit the blink frame of the eyes instead of the open eyes */
  blink: boolean
  tool: 'draw' | 'erase'
}>()

const emit = defineEmits<{
  /** A new brush stroke is about to change the face (for undo) */
  strokeStart: []
}>()

const svg = ref<SVGSVGElement>()
const hover = ref<{ x: number; y: number } | null>(null)

let stroke: { erase: boolean; last: { x: number; y: number } | null; started: boolean } | null =
  null

const panels = computed(() =>
  EDITABLE_PANELS.map((panel) => {
    const bitmap = panel.bitmap(props.face, props.blink)
    const leds = []
    for (let row = 0; row < 8; row++) {
      for (let col = 0; col < 8; col++) leds.push({ row, col, on: isLit(bitmap, row, col) })
    }
    return { ...panel, leds }
  }),
)

function toLayout(event: PointerEvent) {
  const matrix = svg.value?.getScreenCTM()
  if (!matrix) return null
  const point = new DOMPoint(event.clientX, event.clientY).matrixTransform(matrix.inverse())
  return { x: point.x, y: point.y }
}

function paintAt(x: number, y: number) {
  const hit = ledAt(x, y)
  if (!hit || !stroke) return
  const bitmap = hit.panel.bitmap(props.face, props.blink)
  const on = !stroke.erase
  if (isLit(bitmap, hit.row, hit.col) === on) return

  if (!stroke.started) {
    emit('strokeStart')
    stroke.started = true
  }
  setLed(bitmap, hit.row, hit.col, on)
}

/** Paint every LED between the last point and this one, so fast strokes don't leave gaps */
function paintTo(point: { x: number; y: number }) {
  if (!stroke) return
  const from = stroke.last ?? point
  const steps = Math.max(1, Math.ceil(Math.hypot(point.x - from.x, point.y - from.y) / 0.4))
  for (let i = 1; i <= steps; i++) {
    paintAt(from.x + ((point.x - from.x) * i) / steps, from.y + ((point.y - from.y) * i) / steps)
  }
  stroke.last = point
}

function onPointerDown(event: PointerEvent) {
  if (event.button !== 0 && event.button !== 2) return
  const point = toLayout(event)
  if (!point) return
  svg.value?.setPointerCapture(event.pointerId)
  stroke = {
    erase: props.tool === 'erase' || event.altKey || event.button === 2,
    last: null,
    started: false,
  }
  paintAt(point.x, point.y)
  stroke.last = point
}

function onPointerMove(event: PointerEvent) {
  const point = toLayout(event)
  if (point) {
    const hit = ledAt(point.x, point.y)
    hover.value = hit ? { x: hit.panel.x + hit.col, y: hit.panel.y + hit.row } : null
  }
  if (stroke && point) paintTo(point)
}

function endStroke() {
  stroke = null
}
</script>

<template>
  <svg
    ref="svg"
    :viewBox="`-1 -2.5 ${LAYOUT_WIDTH + 2} ${LAYOUT_HEIGHT + 3.5}`"
    class="block h-auto w-full touch-none select-none"
    :class="tool === 'erase' ? 'cursor-cell' : 'cursor-crosshair'"
    role="img"
    aria-label="Expression editor. Paint LEDs to draw the right side of the face."
    @pointerdown="onPointerDown"
    @pointermove="onPointerMove"
    @pointerup="endStroke"
    @pointercancel="endStroke"
    @pointerleave="hover = null"
    @contextmenu.prevent
  >
    <defs>
      <filter id="editor-led-glow" x="-10%" y="-10%" width="120%" height="120%">
        <feGaussianBlur stdDeviation="0.3" result="blur" />
        <feMerge>
          <feMergeNode in="blur" />
          <feMergeNode in="SourceGraphic" />
        </feMerge>
      </filter>
    </defs>

    <g v-for="panel in panels" :key="panel.key" :transform="`translate(${panel.x} ${panel.y})`">
      <rect class="panel-frame" x="0" y="0" width="8" height="8" rx="0.2" />
      <text class="panel-label" x="0" y="-0.6">{{ panel.label }}</text>
      <g filter="url(#editor-led-glow)">
        <circle
          v-for="led in panel.leds"
          :key="`${led.row}-${led.col}`"
          class="led"
          :class="{ on: led.on }"
          :cx="led.col + 0.5"
          :cy="led.row + 0.5"
          r="0.38"
        />
      </g>
    </g>

    <!-- LED under the pointer -->
    <rect
      v-if="hover"
      class="hover-ring"
      :x="hover.x + 0.05"
      :y="hover.y + 0.05"
      width="0.9"
      height="0.9"
      rx="0.45"
    />
  </svg>
</template>

<style scoped>
.panel-frame {
  fill: var(--color-visor-glass);
  stroke: var(--color-slate-500);
  stroke-width: 0.08;
}

.panel-label {
  font-family: var(--font-sans);
  font-size: 1px;
  font-weight: 600;
  fill: var(--color-slate-400);
}

.led {
  fill: var(--color-led-off);
}

.led.on {
  fill: var(--color-led);
}

.hover-ring {
  fill: none;
  stroke: var(--color-white);
  stroke-width: 0.08;
  pointer-events: none;
}
</style>

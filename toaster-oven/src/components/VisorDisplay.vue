<script setup lang="ts">
import { computed, onMounted, ref } from 'vue'
import { PanelType } from '@/state/config-types'
import type { PanelBitmap } from '@/faces/faces'

// Draws the face panels on a visor, laid out like Docs/Images/panels_diagram_twocon.png
// (as seen from the front). Units are LEDs.

export interface PanelMarker {
  state: 'used' | 'unused' | 'attention'
  label?: string
  title?: string
}

const props = withDefaults(
  defineProps<{
    /** What each panel shows. Panels without a bitmap are drawn unlit. */
    bitmaps: Record<number, PanelBitmap>
    /** Which panels to draw */
    panelIds: number[]
    /** Outline, label and colour each panel by status (panel layout map) */
    markers?: Record<number, PanelMarker>
    /** Light the LEDs row by row when first shown, like the MAX7219s do */
    boot?: boolean
  }>(),
  { markers: undefined, boot: false },
)

const hasSingleNose = computed(() => props.panelIds.includes(PanelType.SINGLE_NOSE))

/** Top-left corner of each panel */
const positions = computed<Record<number, { x: number; y: number }>>(() => {
  // Side noses move outwards to make room for the single nose
  const noseX = hasSingleNose.value ? 7 : 1
  const right: Record<number, { x: number; y: number }> = {
    [PanelType.RIGHT_EYE_FRONT]: { x: 24, y: 2 },
    [PanelType.RIGHT_EYE_BACK]: { x: 32, y: 2 },
    [PanelType.RIGHT_NOSE]: { x: noseX, y: 9 },
    [PanelType.RIGHT_MOUTH_FRONT]: { x: 6, y: 20 },
    [PanelType.RIGHT_MOUTH_MID_FRONT]: { x: 14, y: 20 },
    [PanelType.RIGHT_MOUTH_MID_BACK]: { x: 22, y: 20 },
    [PanelType.RIGHT_MOUTH_BACK]: { x: 30, y: 20 },
  }
  const leftOf: Record<number, number> = {
    [PanelType.LEFT_EYE_FRONT]: PanelType.RIGHT_EYE_FRONT,
    [PanelType.LEFT_EYE_BACK]: PanelType.RIGHT_EYE_BACK,
    [PanelType.LEFT_NOSE]: PanelType.RIGHT_NOSE,
    [PanelType.LEFT_MOUTH_FRONT]: PanelType.RIGHT_MOUTH_FRONT,
    [PanelType.LEFT_MOUTH_MID_FRONT]: PanelType.RIGHT_MOUTH_MID_FRONT,
    [PanelType.LEFT_MOUTH_MID_BACK]: PanelType.RIGHT_MOUTH_MID_BACK,
    [PanelType.LEFT_MOUTH_BACK]: PanelType.RIGHT_MOUTH_BACK,
  }
  const all: Record<number, { x: number; y: number }> = {
    ...right,
    [PanelType.SINGLE_NOSE]: { x: -4, y: 9 },
  }
  for (const [left, r] of Object.entries(leftOf)) {
    all[Number(left)] = { x: -(right[r]!.x + 8), y: right[r]!.y }
  }
  return all
})

const panels = computed(() =>
  props.panelIds.map((id) => {
    const bitmap = props.bitmaps[id]
    const marker = props.markers?.[id]
    const leds = []
    for (let row = 0; row < 8; row++) {
      for (let col = 0; col < 8; col++) {
        const on = !!bitmap && !!(bitmap[row]! & (0x80 >> col))
        leds.push({ row, col, on })
      }
    }
    return { id, ...positions.value[id]!, leds, marker }
  }),
)

const booting = ref(props.boot)
onMounted(() => {
  if (booting.value) setTimeout(() => (booting.value = false), 1400)
})
</script>

<template>
  <svg
    viewBox="-50 -7 100 45"
    class="visor block h-auto w-full"
    :class="{ booting }"
    role="img"
    aria-label="Protogen face panels"
  >
    <defs>
      <filter id="led-glow" x="-10%" y="-10%" width="120%" height="120%">
        <feGaussianBlur stdDeviation="0.55" result="blur" />
        <feMerge>
          <feMergeNode in="blur" />
          <feMergeNode in="SourceGraphic" />
        </feMerge>
      </filter>
      <linearGradient id="visor-sheen" x1="0" y1="0" x2="0" y2="1">
        <stop offset="0" stop-color="white" stop-opacity="0.07" />
        <stop offset="0.35" stop-color="white" stop-opacity="0" />
      </linearGradient>
    </defs>

    <!-- Smoked visor -->
    <path
      class="visor-glass"
      d="M -43 -5 H 43 Q 48 -5 48 0 V 27 Q 48 35.5 39.5 35.5 H -39.5 Q -48 35.5 -48 27 V 0 Q -48 -5 -43 -5 Z"
    />
    <path
      fill="url(#visor-sheen)"
      d="M -43 -5 H 43 Q 48 -5 48 0 V 27 Q 48 35.5 39.5 35.5 H -39.5 Q -48 35.5 -48 27 V 0 Q -48 -5 -43 -5 Z"
    />

    <g v-for="panel in panels" :key="panel.id" :transform="`translate(${panel.x} ${panel.y})`">
      <title v-if="panel.marker?.title">{{ panel.marker.title }}</title>

      <rect
        v-if="panel.marker"
        class="panel-frame"
        :class="`panel-${panel.marker.state}`"
        x="0"
        y="0"
        width="8"
        height="8"
        rx="0.3"
      />

      <g
        filter="url(#led-glow)"
        :class="{ 'panel-attention-leds': panel.marker?.state === 'attention' }"
      >
        <circle
          v-for="led in panel.leds"
          :key="`${led.row}-${led.col}`"
          class="led"
          :class="{ on: led.on }"
          :style="{ '--row': led.row }"
          :cx="led.col + 0.5"
          :cy="led.row + 0.5"
          r="0.36"
        />
      </g>

      <text
        v-if="panel.marker?.label"
        class="panel-label"
        :class="`panel-${panel.marker.state}`"
        x="0"
        y="-1.1"
      >
        {{ panel.marker.label }}
      </text>
    </g>
  </svg>
</template>

<style scoped>
.visor-glass {
  fill: var(--color-visor-glass);
  stroke: var(--color-border);
  stroke-width: 0.4;
}

.led {
  fill: var(--color-led-off);
}

.led.on {
  fill: var(--color-led);
}

.panel-attention-leds .led.on {
  fill: var(--color-gold-400);
}

.panel-frame {
  fill: none;
  /* Sits on the panel edge, in the gap between neighbouring panels' LEDs */
  stroke-width: 0.18;
}

.panel-frame.panel-used {
  /* Light grey so the frame stands apart from the blue LEDs */
  stroke: var(--color-slate-500);
}

.panel-frame.panel-unused {
  stroke: var(--color-slate-600);
  stroke-dasharray: 0.8 0.6;
}

.panel-frame.panel-attention {
  stroke: var(--color-gold-400);
  stroke-width: 0.22;
}

.panel-label {
  font-family: var(--font-sans);
  font-size: 1.7px;
  font-weight: 600;
  fill: var(--color-slate-300);
}

.panel-label.panel-attention {
  fill: var(--color-gold-300);
}

/* Power-on: rows light top to bottom across every panel at once, like the MAX7219 chain */
@media (prefers-reduced-motion: no-preference) {
  .booting .led.on {
    animation: led-row-on 90ms steps(1, end) both;
    animation-delay: calc(var(--row) * 110ms + 200ms);
  }
}

@keyframes led-row-on {
  from {
    fill: var(--color-led-off);
  }
  to {
    fill: var(--color-led);
  }
}
</style>

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

/** A chain of panels wired together, drawn as wires from the Arduino through each panel in turn */
export interface PanelChain {
  /** Shown where the wires come in from the Arduino, e.g. "C1" */
  label: string
  /** Tooltip for the incoming wires, e.g. the pins */
  title?: string
  /** The panels in wiring order */
  panelIds: number[]
}

const props = withDefaults(
  defineProps<{
    /** What each panel shows. Panels without a bitmap are drawn unlit. */
    bitmaps: Record<number, PanelBitmap>
    /** Which panels to draw */
    panelIds: number[]
    /** Outline, label and colour each panel by status (panel layout map) */
    markers?: Record<number, PanelMarker>
    /** Draw the wiring of each chain (panel layout map) */
    chains?: PanelChain[]
    /** Light the LEDs row by row when first shown, like the MAX7219s do */
    boot?: boolean
  }>(),
  { markers: undefined, chains: undefined, boot: false },
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
    // Labels go above the panel, unless a wire meets its top edge there
    const labelBelow = wiring.value.labelBelow.has(id)
    return { id, ...positions.value[id]!, leds, marker, labelY: labelBelow ? 10.3 : -1.1 }
  }),
)

// Wiring: a wire from each panel to the next, with an arrow where it goes in. Panels are 8x8,
// so a wire leaves a panel's edge facing the next panel and enters the next panel's facing edge
type Point = { x: number; y: number }

interface Wire {
  key: string
  /** The wires in from the Arduino */
  input: boolean
  line: { from: Point; to: Point }
  arrow: string
  label?: Point & { text: string; anchor: 'start' | 'end' }
  title?: string
  /** The panels the wire meets, and where on their edges */
  ends: { panel: number; point: Point }[]
}

/** How far the wires in from the Arduino start outside the visor, leaving room for their label */
const CABLE_START_X = 62.5

/** Where a line from a panel's centre towards a point crosses the panel's edge */
function edgeToward(panel: Point, target: Point): Point {
  const centre = { x: panel.x + 4, y: panel.y + 4 }
  const dx = target.x - centre.x
  const dy = target.y - centre.y
  const t = Math.min(dx ? 4 / Math.abs(dx) : Infinity, dy ? 4 / Math.abs(dy) : Infinity)
  return { x: centre.x + dx * t, y: centre.y + dy * t }
}

/** An arrowhead whose tip is just past `at`, pointing from `from` towards `at` */
function arrowhead(from: Point, at: Point) {
  const length = Math.hypot(at.x - from.x, at.y - from.y) || 1
  const ux = (at.x - from.x) / length
  const uy = (at.y - from.y) / length
  const tip = { x: at.x + ux * 0.6, y: at.y + uy * 0.6 }
  const back = { x: at.x - ux * 0.9, y: at.y - uy * 0.9 }
  return [
    tip,
    { x: back.x - uy * 0.85, y: back.y + ux * 0.85 },
    { x: back.x + uy * 0.85, y: back.y - ux * 0.85 },
  ]
    .map((p) => `${p.x},${p.y}`)
    .join(' ')
}

const wiring = computed(() => {
  const wires = (props.chains ?? []).flatMap((chain) => {
    const shown = chain.panelIds.filter((id) => props.panelIds.includes(id) && positions.value[id])
    if (!shown.length) return []
    const at = (id: number) => positions.value[id]!
    const centre = (id: number) => ({ x: at(id).x + 4, y: at(id).y + 4 })

    // Wires in from the Arduino, from outside the visor on the first panel's side,
    // labelled just outside the visor
    const first = shown[0]!
    const side = centre(first).x >= 0 ? 1 : -1
    const start = { x: side * CABLE_START_X, y: centre(first).y }
    const entry = edgeToward(at(first), start)
    const chainWires: Wire[] = [
      {
        key: `${chain.label}-in`,
        input: true,
        line: { from: start, to: entry },
        arrow: arrowhead(start, entry),
        label: {
          text: chain.label,
          x: side * 49.5,
          y: start.y - 0.9,
          anchor: side > 0 ? 'start' : 'end',
        },
        title: chain.title,
        ends: [{ panel: first, point: entry }],
      },
    ]

    // Wires from each panel to the next
    for (let i = 1; i < shown.length; i++) {
      const from = shown[i - 1]!
      const to = shown[i]!
      if (from === to) continue
      const exit = edgeToward(at(from), centre(to))
      const enter = edgeToward(at(to), centre(from))
      chainWires.push({
        key: `${chain.label}-${i}`,
        input: false,
        line: { from: exit, to: enter },
        // Touching panels have no gap to draw a wire in: point from one centre to the other
        arrow: arrowhead(
          Math.hypot(enter.x - exit.x, enter.y - exit.y) < 0.5 ? centre(from) : exit,
          enter,
        ),
        ends: [
          { panel: from, point: exit },
          { panel: to, point: enter },
        ],
      })
    }
    return chainWires
  })

  // Panels whose top edge a wire meets (where their label goes) but not their bottom edge
  const onEdge = (id: number, y: number) =>
    wires.some((w) => w.ends.some((e) => e.panel === id && Math.abs(e.point.y - y) < 0.01))
  const labelBelow = new Set(
    props.panelIds.filter((id) => {
      const top = positions.value[id]?.y
      return top !== undefined && onEdge(id, top) && !onEdge(id, top + 8)
    }),
  )

  return { wires, labelBelow }
})

/** Wider when wiring is shown, to fit the wires coming in from the Arduino and their labels */
const viewBox = computed(() => (props.chains?.length ? '-64 -7 128 45' : '-50 -7 100 45'))

const booting = ref(props.boot)
onMounted(() => {
  if (booting.value) setTimeout(() => (booting.value = false), 1400)
})
</script>

<template>
  <svg
    :viewBox="viewBox"
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
        :y="panel.labelY"
      >
        {{ panel.marker.label }}
      </text>
    </g>

    <!-- Wiring, on top of the panels so the arrows show where wires go in -->
    <g
      v-for="wire in wiring.wires"
      :key="wire.key"
      class="wire"
      :class="{ 'wire-input': wire.input }"
    >
      <title v-if="wire.title">{{ wire.title }}</title>
      <line
        :x1="wire.line.from.x"
        :y1="wire.line.from.y"
        :x2="wire.line.to.x"
        :y2="wire.line.to.y"
      />
      <polygon class="wire-arrow" :points="wire.arrow" />
      <text
        v-if="wire.label"
        class="wire-label"
        :x="wire.label.x"
        :y="wire.label.y"
        :text-anchor="wire.label.anchor"
      >
        {{ wire.label.text }}
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

/* Wires and arrows are green, so they stand out from the blue LEDs */
.wire line {
  stroke: #2fcd03;
  stroke-width: 0.3;
  stroke-linecap: round;
}

/* The wires in from the Arduino: a thicker cable */
.wire-input line {
  stroke-width: 0.6;
}

.wire-arrow {
  fill: #2fcd03;
  stroke: var(--color-visor-glass);
  stroke-width: 0.15;
  stroke-linejoin: round;
}

.wire-label {
  font-family: var(--font-sans);
  font-size: 1.7px;
  font-weight: 700;
  fill: var(--color-white);
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

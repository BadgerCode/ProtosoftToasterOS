<script setup lang="ts">
import { computed, onBeforeUnmount, ref, useId, watch } from 'vue'

// A colour picker that looks the same in every browser (Firefox's native one is the old
// Windows dialog). The value is a "#rrggbb" string. Hue, saturation and brightness are kept
// separately so dragging through grey or black doesn't lose the hue.

const colour = defineModel<string>({ required: true })

defineProps<{ label: string }>()

const open = ref(false)
const root = ref<HTMLElement>()
const panelId = useId()

// HSV, each 0-1
const hue = ref(0)
const saturation = ref(0)
const brightness = ref(0)

function hexToHsv(hex: string) {
  const n = parseInt(hex.slice(1), 16)
  const r = (n >> 16) / 255
  const g = ((n >> 8) & 255) / 255
  const b = (n & 255) / 255
  const max = Math.max(r, g, b)
  const delta = max - Math.min(r, g, b)
  let h = 0
  if (delta) {
    if (max === r) h = ((g - b) / delta + 6) % 6
    else if (max === g) h = (b - r) / delta + 2
    else h = (r - g) / delta + 4
  }
  return { h: h / 6, s: max ? delta / max : 0, v: max }
}

function hsvToHex(h: number, s: number, v: number) {
  const channel = (n: number) => {
    const k = (n + h * 6) % 6
    const value = v - v * s * Math.max(0, Math.min(k, 4 - k, 1))
    return Math.round(value * 255)
      .toString(16)
      .padStart(2, '0')
  }
  return `#${channel(5)}${channel(3)}${channel(1)}`
}

// Follow outside changes (e.g. an import), but keep the hue when the colour is the same
watch(
  colour,
  (value) => {
    if (value === hsvToHex(hue.value, saturation.value, brightness.value)) return
    const hsv = hexToHsv(value)
    if (hsv.s && hsv.v) hue.value = hsv.h
    saturation.value = hsv.s
    brightness.value = hsv.v
  },
  { immediate: true },
)

function update() {
  colour.value = hsvToHex(hue.value, saturation.value, brightness.value)
}

const clamp = (n: number) => Math.min(1, Math.max(0, n))
const pureHue = computed(() => hsvToHex(hue.value, 1, 1))

// Dragging on the saturation/brightness area and the hue slider
function drag(event: PointerEvent, onMove: (x: number, y: number) => void) {
  const target = event.currentTarget as HTMLElement
  const move = (e: PointerEvent) => {
    const rect = target.getBoundingClientRect()
    onMove(clamp((e.clientX - rect.left) / rect.width), clamp((e.clientY - rect.top) / rect.height))
  }
  target.setPointerCapture(event.pointerId)
  move(event)
  target.onpointermove = move
  target.onpointerup = () => (target.onpointermove = null)
}

function dragArea(event: PointerEvent) {
  drag(event, (x, y) => {
    saturation.value = x
    brightness.value = 1 - y
    update()
  })
}

function dragHue(event: PointerEvent) {
  drag(event, (x) => {
    hue.value = x
    update()
  })
}

// Keyboard: arrows move by 1%, or 10% with Shift
function step(event: KeyboardEvent) {
  return event.shiftKey ? 0.1 : 0.01
}

function areaKey(event: KeyboardEvent) {
  const s = step(event)
  if (event.key === 'ArrowLeft') saturation.value = clamp(saturation.value - s)
  else if (event.key === 'ArrowRight') saturation.value = clamp(saturation.value + s)
  else if (event.key === 'ArrowUp') brightness.value = clamp(brightness.value + s)
  else if (event.key === 'ArrowDown') brightness.value = clamp(brightness.value - s)
  else return
  event.preventDefault()
  update()
}

function hueKey(event: KeyboardEvent) {
  const s = step(event)
  if (event.key === 'ArrowLeft' || event.key === 'ArrowDown') hue.value = clamp(hue.value - s)
  else if (event.key === 'ArrowRight' || event.key === 'ArrowUp') hue.value = clamp(hue.value + s)
  else return
  event.preventDefault()
  update()
}

// Hex box: applied once it's a full colour
const hexDraft = ref('')
watch([colour, open], () => (hexDraft.value = colour.value.slice(1).toUpperCase()), {
  immediate: true,
})

function onHexInput() {
  const hex = hexDraft.value.trim().replace(/^#/, '')
  if (/^[0-9a-f]{6}$/i.test(hex)) colour.value = `#${hex.toLowerCase()}`
}

// Closing: click outside or Escape
function onDocumentPointerDown(event: PointerEvent) {
  if (!root.value?.contains(event.target as Node)) open.value = false
}

watch(open, (isOpen) => {
  if (isOpen) document.addEventListener('pointerdown', onDocumentPointerDown)
  else document.removeEventListener('pointerdown', onDocumentPointerDown)
})
onBeforeUnmount(() => document.removeEventListener('pointerdown', onDocumentPointerDown))
</script>

<template>
  <div ref="root" class="relative" @keydown.esc="open = false">
    <button
      type="button"
      class="flex items-center gap-3 rounded-md border border-border bg-page py-1.5 pr-3 pl-1.5 text-left transition-colors hover:border-accent disabled:cursor-not-allowed disabled:opacity-50 disabled:hover:border-border"
      :aria-expanded="open"
      :aria-controls="panelId"
      @click="open = !open"
    >
      <span
        class="size-6 rounded border border-white/15"
        :style="{ backgroundColor: colour }"
        aria-hidden="true"
      />
      <span class="text-sm text-slate-200">{{ label }}</span>
      <code class="text-xs text-slate-500">{{ colour.toUpperCase() }}</code>
    </button>

    <div
      v-if="open"
      :id="panelId"
      class="absolute top-full left-0 z-20 mt-2 w-60 space-y-3 rounded-lg border border-border bg-surface p-3 shadow-lg shadow-black/40"
      role="dialog"
      :aria-label="`${label} picker`"
    >
      <!-- Saturation (left to right) and brightness (bottom to top) -->
      <div
        class="relative h-36 cursor-crosshair touch-none rounded-md"
        :style="{
          backgroundColor: pureHue,
          backgroundImage:
            'linear-gradient(to top, #000, transparent), linear-gradient(to right, #fff, transparent)',
        }"
        role="slider"
        tabindex="0"
        :aria-label="`${label} saturation and brightness`"
        :aria-valuetext="`Saturation ${Math.round(saturation * 100)}%, brightness ${Math.round(brightness * 100)}%`"
        @pointerdown="dragArea"
        @keydown="areaKey"
      >
        <span
          class="pointer-events-none absolute size-4 -translate-x-1/2 -translate-y-1/2 rounded-full border-2 border-white shadow ring-1 ring-black/40"
          :style="{
            left: `${saturation * 100}%`,
            top: `${(1 - brightness) * 100}%`,
            backgroundColor: colour,
          }"
        />
      </div>

      <!-- Hue -->
      <div
        class="relative h-3 cursor-pointer touch-none rounded-full"
        style="background: linear-gradient(to right, #f00, #ff0, #0f0, #0ff, #00f, #f0f, #f00)"
        role="slider"
        tabindex="0"
        :aria-label="`${label} hue`"
        aria-valuemin="0"
        aria-valuemax="360"
        :aria-valuenow="Math.round(hue * 360)"
        @pointerdown="dragHue"
        @keydown="hueKey"
      >
        <span
          class="pointer-events-none absolute top-1/2 size-4 -translate-x-1/2 -translate-y-1/2 rounded-full border-2 border-white shadow ring-1 ring-black/40"
          :style="{ left: `${hue * 100}%`, backgroundColor: pureHue }"
        />
      </div>

      <label class="flex items-center gap-2">
        <span class="text-sm text-slate-400">#</span>
        <input
          v-model="hexDraft"
          class="form-input font-mono uppercase"
          maxlength="7"
          spellcheck="false"
          autocomplete="off"
          :aria-label="`${label} hex code`"
          @input="onHexInput"
          @keydown.enter="open = false"
        />
      </label>
    </div>
  </div>
</template>

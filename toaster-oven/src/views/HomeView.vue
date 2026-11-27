<script setup lang="ts">
import { computed, onBeforeUnmount, onMounted, ref } from 'vue'
import { RouterLink } from 'vue-router'
import VisorDisplay from '@/components/VisorDisplay.vue'
import { expressionToPanels } from '@/faces/faces'
import { appStore, getDefaultExpression } from '@/state/app-state'
import { boopExpressionFor } from '@/state/boop-expressions'
import type { Expression } from '@/state/expressions'
import { PANEL_TYPES, PanelType } from '@/state/config-types'

const repoUrl = 'https://github.com/BadgerCode/ProtosoftToasterOS'

const features = [
  {
    title: 'Face rendering',
    description: 'Renders eyes, nose and mouth on each side of the face.',
  },
  {
    title: 'Expressions',
    description: 'Multiple expressions, with simple animations and blinking support.',
  },
  {
    title: 'Improved MAX7219 support',
    description:
      '93% more efficient panel updates, auto-recovery for loose connections, up to 16 panels per chain.',
  },
  {
    title: 'Remote control',
    description: 'Switch expressions and behaviour using a remote control.',
  },
  {
    title: 'Side panel lights',
    description: 'Supports RGB light strips in each side panel/headphone.',
  },
  {
    title: 'Boop detection',
    description:
      'Detects nose boops with an IR sensor, showing different expressions for consecutive boops.',
  },
]

// The hero visor: the default two-nose layout, blinking and boopable like the real thing
const visorPanels = PANEL_TYPES.map((p) => p.id).filter((id) => id !== PanelType.SINGLE_NOSE)

const blinking = ref(false)
/** The boop expression being shown, if any. Uses your edited expressions and boop settings. */
const boopFace = ref<Expression | null>(null)
let boopCount = 0

const bitmaps = computed(() =>
  expressionToPanels(boopFace.value ?? getDefaultExpression(), blinking.value && !boopFace.value),
)

let blinkTimer: ReturnType<typeof setTimeout> | undefined
let boopTimer: ReturnType<typeof setTimeout> | undefined
let boopResetTimer: ReturnType<typeof setTimeout> | undefined

function scheduleBlink() {
  blinkTimer = setTimeout(
    () => {
      blinking.value = true
      blinkTimer = setTimeout(() => {
        blinking.value = false
        scheduleBlink()
      }, 200)
    },
    3000 + Math.random() * 3000,
  )
}

// Same escalation as ExpressionManager, using the boop expressions from the Features tab
function boop() {
  boopCount++
  boopFace.value = boopExpressionFor(boopCount, appStore.boopExpressions, appStore.expressions)

  clearTimeout(boopTimer)
  boopTimer = setTimeout(() => (boopFace.value = null), 1700)

  clearTimeout(boopResetTimer)
  boopResetTimer = setTimeout(() => (boopCount = 0), 5000)
}

onMounted(scheduleBlink)
onBeforeUnmount(() => {
  clearTimeout(blinkTimer)
  clearTimeout(boopTimer)
  clearTimeout(boopResetTimer)
})
</script>

<template>
  <section>
    <!-- Hero -->
    <div class="grid items-center gap-8 lg:grid-cols-[minmax(0,5fr)_minmax(0,7fr)] lg:gap-12">
      <div class="order-2 lg:order-1">
        <h1 class="display-title text-5xl sm:text-6xl">Protosoft Toaster OS</h1>

        <div class="mt-6 max-w-prose space-y-3 text-lg leading-relaxed text-slate-300">
          <p>Beeeeeep! :3</p>
          <p>Toaster Oven helps you to create your protogen!</p>
        </div>

        <div class="mt-8 flex flex-wrap gap-3">
          <RouterLink to="/configure" class="btn btn-primary px-5 py-2.5 text-base">
            Configure your protogen
          </RouterLink>
          <RouterLink to="/guides" class="btn btn-secondary px-5 py-2.5 text-base">
            Help guides
          </RouterLink>
          <a
            :href="repoUrl"
            target="_blank"
            rel="noopener"
            class="btn btn-secondary px-5 py-2.5 text-base"
          >
            <svg class="size-5" viewBox="0 0 24 24" fill="currentColor" aria-hidden="true">
              <path
                d="M12 .5C5.65.5.5 5.65.5 12a11.5 11.5 0 0 0 7.86 10.92c.58.1.79-.25.79-.56v-2c-3.2.7-3.87-1.37-3.87-1.37-.53-1.33-1.28-1.69-1.28-1.69-1.05-.72.08-.7.08-.7 1.16.08 1.77 1.19 1.77 1.19 1.03 1.77 2.7 1.26 3.36.96.1-.75.4-1.26.73-1.55-2.55-.29-5.24-1.28-5.24-5.68 0-1.26.45-2.28 1.19-3.08-.12-.29-.52-1.46.11-3.05 0 0 .97-.31 3.17 1.18a11 11 0 0 1 5.77 0c2.2-1.49 3.17-1.18 3.17-1.18.63 1.59.23 2.76.11 3.05.74.8 1.19 1.82 1.19 3.08 0 4.41-2.69 5.39-5.26 5.67.41.36.78 1.06.78 2.14v3.17c0 .31.21.67.8.56A11.5 11.5 0 0 0 23.5 12C23.5 5.65 18.35.5 12 .5Z"
              />
            </svg>
            View on GitHub
          </a>
        </div>
      </div>

      <figure class="order-1 lg:order-2">
        <button
          type="button"
          class="block w-full cursor-pointer rounded-3xl transition-transform active:scale-[0.99]"
          aria-label="Boop the protogen"
          @click="boop"
        >
          <VisorDisplay :bitmaps="bitmaps" :panel-ids="visorPanels" boot />
        </button>
        <figcaption class="mt-2 text-center text-sm text-slate-500">
          Click the visor to boop it.
        </figcaption>
      </figure>
    </div>

    <!-- About -->
    <div class="mt-20 max-w-prose space-y-3 text-slate-300">
      <h2 class="text-2xl font-bold text-white">What is Toaster OS?</h2>
      <p>
        Toaster OS is software for protogens using MAX7219 8x8 LED panels, on an Arduino Nano Every
        or ESP32 microcontroller. It draws the face, switches between expressions, and runs extras
        like side lights, a boop sensor and a remote control.
      </p>
      <p>
        Toaster Oven, this website, lets you lay out your panels, draw your expressions and choose
        your features. Then you download the files and upload them to your microcontroller. The
        <RouterLink to="/guides" class="text-accent hover:underline">guides</RouterLink> take you
        through it, from wiring to your first upload.
      </p>
    </div>

    <!-- Features -->
    <div class="mt-12">
      <h2 class="text-2xl font-bold text-white">Features</h2>
      <p class="mt-1 text-slate-400">
        Features are configurable and can be disabled if you lack the hardware.
      </p>

      <dl class="mt-6 grid gap-x-12 sm:grid-cols-2">
        <div v-for="feature in features" :key="feature.title" class="border-t border-border py-5">
          <dt class="font-semibold text-white">{{ feature.title }}</dt>
          <dd class="mt-1 max-w-prose text-slate-400">{{ feature.description }}</dd>
        </div>
      </dl>
    </div>
  </section>
</template>

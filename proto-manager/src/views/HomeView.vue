<script setup lang="ts">
import PageHeading from '@/components/PageHeading.vue'

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
    description: '93% more efficient panel updates, auto-recovery for loose connections, up to 16 panels per chain.',
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
    description: 'Detects nose boops with an IR sensor, showing different expressions for consecutive boops.',
  },
]
</script>

<template>
  <section class="space-y-8">
    <PageHeading
      title="Home"
      subtitle="Build configuration and data files for the Protogen Arduino project."
    />

    <!-- About -->
    <div class="rounded-lg border border-border bg-surface p-6">
      <h2 class="text-xl font-semibold text-white">Protosoft Toaster OS</h2>
      <div class="mt-3 max-w-3xl space-y-3 leading-relaxed text-slate-300">
        <p>
          Beeeeeep! This project is for protogen fursuit heads which use MAX7219 8x8 LED panels.
        </p>

        <p>
          Use this page to configure the software &amp; manage your expressions :3
        </p>
      </div>

      <a
        :href="repoUrl"
        target="_blank"
        rel="noopener"
        class="mt-5 inline-flex items-center gap-2 rounded-md bg-accent px-4 py-2 text-sm font-medium text-white transition-colors hover:bg-accent-hover"
      >
        <svg class="size-5" viewBox="0 0 24 24" fill="currentColor" aria-hidden="true">
          <path
            d="M12 .5C5.65.5.5 5.65.5 12a11.5 11.5 0 0 0 7.86 10.92c.58.1.79-.25.79-.56v-2c-3.2.7-3.87-1.37-3.87-1.37-.53-1.33-1.28-1.69-1.28-1.69-1.05-.72.08-.7.08-.7 1.16.08 1.77 1.19 1.77 1.19 1.03 1.77 2.7 1.26 3.36.96.1-.75.4-1.26.73-1.55-2.55-.29-5.24-1.28-5.24-5.68 0-1.26.45-2.28 1.19-3.08-.12-.29-.52-1.46.11-3.05 0 0 .97-.31 3.17 1.18a11 11 0 0 1 5.77 0c2.2-1.49 3.17-1.18 3.17-1.18.63 1.59.23 2.76.11 3.05.74.8 1.19 1.82 1.19 3.08 0 4.41-2.69 5.39-5.26 5.67.41.36.78 1.06.78 2.14v3.17c0 .31.21.67.8.56A11.5 11.5 0 0 0 23.5 12C23.5 5.65 18.35.5 12 .5Z"
          />
        </svg>
        View on GitHub
      </a>
    </div>

    <!-- Features -->
    <div>
      <h2 class="text-xl font-semibold text-white">Features</h2>
      <p class="mt-1 text-slate-400">Features are configurable and can be disabled if you lack the hardware.</p>

      <ul class="mt-4 grid gap-4 sm:grid-cols-2 lg:grid-cols-3">
        <li
          v-for="feature in features"
          :key="feature.title"
          class="rounded-lg border border-border bg-surface p-5 transition-colors hover:border-accent"
        >
          <h3 class="flex items-center gap-2 font-semibold text-white">
            <span class="size-2 rounded-full bg-accent" />
            {{ feature.title }}
          </h3>
          <p class="mt-2 text-sm text-slate-400">{{ feature.description }}</p>
        </li>
      </ul>
    </div>
  </section>
</template>

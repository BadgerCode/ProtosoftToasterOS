<script setup lang="ts">
import { ref, useId } from 'vue'
import { RouterLink } from 'vue-router'

const props = defineProps<{
  title: string
  description?: string
  /** Show a button in the header that hides and shows the body */
  collapsible?: boolean
  /** Start with the body hidden. Only applies when collapsible */
  startCollapsed?: boolean
  /** Address of the guide for this card, e.g. /guides/toaster-os/side-lights, shown as a link in the header */
  guide?: string
}>()

const open = ref(!(props.collapsible && props.startCollapsed))
const bodyId = useId()
</script>

<template>
  <div class="rounded-xl border border-border bg-surface">
    <div
      class="flex items-start justify-between gap-4 px-5 py-4"
      :class="{ 'border-b border-border': open }"
    >
      <div class="flex min-w-0 flex-1 flex-col items-start">
        <button
          v-if="props.collapsible"
          type="button"
          class="flex w-full cursor-pointer items-start gap-3 text-left"
          :aria-expanded="open"
          :aria-controls="bodyId"
          @click="open = !open"
        >
          <svg
            class="mt-1.5 size-4 shrink-0 text-slate-400"
            :class="{ '-rotate-90': !open }"
            viewBox="0 0 20 20"
            fill="currentColor"
            aria-hidden="true"
          >
            <path
              fill-rule="evenodd"
              d="M5.23 7.21a.75.75 0 0 1 1.06.02L10 11.17l3.71-3.94a.75.75 0 1 1 1.08 1.04l-4.25 4.5a.75.75 0 0 1-1.08 0l-4.25-4.5a.75.75 0 0 1 .02-1.06Z"
            />
          </svg>
          <span>
            <span class="block text-lg font-semibold text-white">{{ title }}</span>
            <span v-if="description" class="mt-0.5 block text-sm text-slate-400">
              {{ description }}
            </span>
          </span>
        </button>
        <template v-else>
          <h2 class="text-lg font-semibold text-white">{{ title }}</h2>
          <p v-if="description" class="mt-0.5 text-sm text-slate-400">{{ description }}</p>
        </template>
        <!-- Outside the button, as a link can't go inside one. ml-7 lines it up with the title -->
        <RouterLink
          v-if="props.guide"
          :to="props.guide"
          class="mt-1 text-sm text-accent hover:underline"
          :class="{ 'ml-7': props.collapsible }"
          :aria-label="`${title} guide`"
        >
          Guide
        </RouterLink>
      </div>
      <slot name="actions" />
    </div>
    <div v-show="open" :id="bodyId" class="p-5">
      <slot />
    </div>
  </div>
</template>

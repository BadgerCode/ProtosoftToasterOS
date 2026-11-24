<script setup lang="ts">
import { computed, onBeforeUnmount, ref } from 'vue'
import { DOWNLOADS, downloadFile, hasChanged, type DownloadKind } from '@/state/config-download'

const props = withDefaults(
  defineProps<{
    kind: DownloadKind
    size?: 'md' | 'lg'
    /** Primary (blue) or secondary styling */
    primary?: boolean
  }>(),
  { size: 'lg', primary: true },
)

const download = computed(() => DOWNLOADS[props.kind])
const changed = computed(() => hasChanged(props.kind))

const justDownloaded = ref(false)
let resetTimer: ReturnType<typeof setTimeout> | undefined

function onClick() {
  downloadFile(props.kind)
  justDownloaded.value = true
  clearTimeout(resetTimer)
  resetTimer = setTimeout(() => (justDownloaded.value = false), 2500)
}

onBeforeUnmount(() => clearTimeout(resetTimer))
</script>

<template>
  <button
    type="button"
    class="btn relative"
    :class="[
      primary ? 'btn-primary' : 'btn-secondary',
      size === 'lg' ? 'px-5 py-2.5 text-base' : 'px-4 py-2',
      changed && primary && 'ring-2 ring-accent/50 ring-offset-2 ring-offset-page',
    ]"
    :title="`Download ${download.fileName}`"
    @click="onClick"
  >
    <!-- Check icon after downloading -->
    <svg
      v-if="justDownloaded && !changed"
      class="size-5"
      viewBox="0 0 20 20"
      fill="currentColor"
      aria-hidden="true"
    >
      <path
        fill-rule="evenodd"
        d="M16.7 4.15a.75.75 0 0 1 .15 1.05l-8 10.5a.75.75 0 0 1-1.13.08l-4.5-4.5a.75.75 0 0 1 1.06-1.06l3.9 3.9 7.47-9.82a.75.75 0 0 1 1.05-.15Z"
      />
    </svg>
    <!-- Download icon -->
    <svg v-else class="size-5" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
      <path
        d="M10.75 2.75a.75.75 0 0 0-1.5 0v8.614L6.295 8.235a.75.75 0 1 0-1.09 1.03l4.25 4.5a.75.75 0 0 0 1.09 0l4.25-4.5a.75.75 0 0 0-1.09-1.03l-2.955 3.129V2.75Z"
      />
      <path
        d="M3.5 12.75a.75.75 0 0 0-1.5 0v2.5A2.75 2.75 0 0 0 4.75 18h10.5A2.75 2.75 0 0 0 18 15.25v-2.5a.75.75 0 0 0-1.5 0v2.5c0 .69-.56 1.25-1.25 1.25H4.75c-.69 0-1.25-.56-1.25-1.25v-2.5Z"
      />
    </svg>

    <span class="flex flex-col items-start leading-tight">
      <span>
        <template v-if="justDownloaded && !changed">Downloaded</template>
        <template v-else>Download {{ download.label }}</template>
      </span>
      <span class="text-xs font-normal opacity-75">{{ download.fileName }}</span>
    </span>

    <!-- Pulsing "changed" badge -->
    <span v-if="changed" class="absolute -top-1 -right-1 flex size-3">
      <span
        class="absolute inline-flex size-full rounded-full bg-gold-400 opacity-75 motion-safe:animate-ping"
      />
      <span class="relative inline-flex size-3 rounded-full bg-gold-400" />
    </span>
  </button>
</template>

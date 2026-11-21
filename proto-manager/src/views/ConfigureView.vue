<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'
import PageHeading from '@/components/PageHeading.vue'
import { appStore } from '@/state/app-state'
import { downloadTextFile, generateConfigHeader } from '@/state/generate-config'

const tabs = [
  { to: '/configure/panels', label: 'Panel layout' },
  { to: '/configure/expressions', label: 'Expressions' },
  { to: '/configure/features', label: 'Features' },
]

function downloadConfig() {
  const header = generateConfigHeader(appStore.faceConfig, appStore.featureConfig)
  downloadTextFile('Config.h', header)
}
</script>

<template>
  <section>
    <PageHeading title="Configure" subtitle="Set up the hardware and behaviour of your protogen.">
      <template #actions>
        <button
          type="button"
          class="btn btn-primary px-5 py-2.5 text-base shadow-lg shadow-accent/20"
          @click="downloadConfig"
        >
          <svg class="size-5" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
            <path
              d="M10.75 2.75a.75.75 0 0 0-1.5 0v8.614L6.295 8.235a.75.75 0 1 0-1.09 1.03l4.25 4.5a.75.75 0 0 0 1.09 0l4.25-4.5a.75.75 0 0 0-1.09-1.03l-2.955 3.129V2.75Z"
            />
            <path
              d="M3.5 12.75a.75.75 0 0 0-1.5 0v2.5A2.75 2.75 0 0 0 4.75 18h10.5A2.75 2.75 0 0 0 18 15.25v-2.5a.75.75 0 0 0-1.5 0v2.5c0 .69-.56 1.25-1.25 1.25H4.75c-.69 0-1.25-.56-1.25-1.25v-2.5Z"
            />
          </svg>
          Download Config.h
        </button>
      </template>
    </PageHeading>

    <nav class="-mt-4 mb-8 flex gap-1 overflow-x-auto shadow-[inset_0_-1px_0] shadow-border">
      <RouterLink
        v-for="tab in tabs"
        :key="tab.to"
        :to="tab.to"
        class="border-b-2 border-transparent px-4 py-2 text-sm font-medium whitespace-nowrap text-slate-400 transition-colors hover:text-white"
        active-class="!border-accent !text-white"
      >
        {{ tab.label }}
      </RouterLink>
    </nav>

    <RouterView />
  </section>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { RouterLink, RouterView, useRoute } from 'vue-router'
import PageHeading from '@/components/PageHeading.vue'
import DownloadConfigButton from '@/components/DownloadConfigButton.vue'
import { DOWNLOADS, changedDownloads, hasUndownloadedChanges } from '@/state/config-download'

const tabs = [
  { to: '/configure/panels', label: 'Panel layout' },
  { to: '/configure/expressions', label: 'Expressions' },
  { to: '/configure/features', label: 'Features' },
]

// The download for the tab you're on gets the blue button
const route = useRoute()
const onExpressions = computed(() => route.name === 'configure-expressions')

const changedSummary = computed(() =>
  changedDownloads.value.map((kind) => `${DOWNLOADS[kind].label}`).join(' and '),
)
</script>

<template>
  <section>
    <PageHeading title="Configure" subtitle="Set up the hardware and behaviour of your protogen.">
      <template #actions>
        <div class="flex flex-wrap gap-3">
          <DownloadConfigButton kind="config" :primary="!onExpressions" />
          <DownloadConfigButton kind="expressions" :primary="onExpressions" />
        </div>
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

    <!-- Sticky reminder while there are changes that haven't been downloaded -->
    <Transition
      enter-from-class="translate-y-4 opacity-0"
      leave-to-class="translate-y-4 opacity-0"
      enter-active-class="transition duration-200"
      leave-active-class="transition duration-200"
    >
      <div v-if="hasUndownloadedChanges" class="sticky bottom-4 z-10 mt-8">
        <div
          class="flex flex-col gap-3 rounded-lg border border-gold-500/40 bg-slate-900/95 px-4 py-3 shadow-xl shadow-black/40 backdrop-blur sm:flex-row sm:items-center sm:justify-between"
        >
          <p class="flex items-center gap-3 text-sm text-gold-200">
            <span class="size-2.5 shrink-0 rounded-full bg-gold-400" />
            <span>
              Your {{ changedSummary }} {{ changedDownloads.length > 1 ? 'have' : 'has' }} changed.
              Download again and copy the {{ changedDownloads.length > 1 ? 'files' : 'file' }} into
              your ToasterOS folder.
            </span>
          </p>
          <div class="flex shrink-0 flex-wrap gap-2">
            <DownloadConfigButton
              v-for="kind in changedDownloads"
              :key="kind"
              :kind="kind"
              size="md"
            />
          </div>
        </div>
      </div>
    </Transition>
  </section>
</template>

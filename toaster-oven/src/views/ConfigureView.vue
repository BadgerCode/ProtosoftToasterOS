<script setup lang="ts">
import { computed, ref, watch } from 'vue'
import { RouterLink, RouterView, useRoute } from 'vue-router'
import PageHeading from '@/components/PageHeading.vue'
import DownloadConfigButton from '@/components/DownloadConfigButton.vue'
import ImportFilesButton from '@/components/ImportFilesButton.vue'
import {
  DOWNLOADS,
  changedDownloads,
  downloadFile,
  hasUndownloadedChanges,
} from '@/state/config-download'
import type { ImportSummary } from '@/state/import/import-files'

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

// Hiding the reminder lasts until everything has been downloaded
const reminderHidden = ref(false)
watch(hasUndownloadedChanges, (changed) => {
  if (!changed) reminderHidden.value = false
})

// What the last import did, until dismissed
const importSummary = ref<(ImportSummary & { at: Date }) | null>(null)

function onImported(summary: ImportSummary) {
  importSummary.value = { ...summary, at: new Date() }
}

/** e.g. "24 Nov, 14:03:12", so an old summary is easy to tell apart from a new one */
const importedAt = computed(() =>
  importSummary.value?.at.toLocaleString(undefined, {
    day: 'numeric',
    month: 'short',
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit',
  }),
)
const importNeedsAttention = computed(
  () =>
    !!importSummary.value &&
    (importSummary.value.notes.length > 0 || importSummary.value.errors.length > 0),
)
</script>

<template>
  <section>
    <PageHeading title="Configure" subtitle="Set up the hardware and behaviour of your protogen.">
      <template #actions>
        <div class="flex flex-wrap gap-3">
          <ImportFilesButton @imported="onImported" />
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

    <!-- What the last import did -->
    <div
      v-if="importSummary"
      class="-mt-4 mb-8 flex items-start gap-3 rounded-lg border bg-surface py-3 pr-2 pl-4 text-sm"
      :class="
        importNeedsAttention ? 'border-gold-500/40 text-gold-200' : 'border-border text-slate-300'
      "
      role="status"
    >
      <div class="min-w-0 flex-1 space-y-1.5">
        <p class="font-semibold">
          <template v-if="importSummary.imported.length">
            Imported {{ importSummary.imported.join(' and ') }}
          </template>
          <template v-else>Nothing was imported</template>
          <span class="ml-2 font-normal text-slate-400">{{ importedAt }}</span>
        </p>
        <ul
          v-if="importSummary.notes.length || importSummary.errors.length"
          class="list-disc space-y-1 pl-5"
        >
          <li v-for="note in importSummary.notes" :key="note">{{ note }}</li>
          <li v-for="error in importSummary.errors" :key="error">{{ error }}</li>
        </ul>
      </div>
      <button
        type="button"
        class="btn btn-icon -my-1.5 shrink-0"
        title="Dismiss"
        aria-label="Dismiss import summary"
        @click="importSummary = null"
      >
        <svg class="size-4" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
          <path
            d="M6.28 5.22a.75.75 0 0 0-1.06 1.06L8.94 10l-3.72 3.72a.75.75 0 1 0 1.06 1.06L10 11.06l3.72 3.72a.75.75 0 1 0 1.06-1.06L11.06 10l3.72-3.72a.75.75 0 0 0-1.06-1.06L10 8.94 6.28 5.22Z"
          />
        </svg>
      </button>
    </div>

    <RouterView />

    <!-- Small sticky reminder while there are changes that haven't been downloaded -->
    <div
      v-if="hasUndownloadedChanges && !reminderHidden"
      class="pointer-events-none sticky bottom-4 z-10 mt-8 flex justify-end"
    >
      <div
        class="pointer-events-auto flex flex-wrap items-center justify-end gap-2 rounded-lg border border-border bg-slate-900/95 py-1.5 pr-1.5 pl-3 shadow-lg shadow-black/30 backdrop-blur"
      >
        <p class="flex items-center gap-2 text-sm text-slate-300">
          <span class="size-2 shrink-0 rounded-full bg-gold-400" />
          Your {{ changedSummary }} {{ changedDownloads.length > 1 ? 'have' : 'has' }} changed
        </p>
        <div class="flex items-center gap-1">
          <button
            v-for="kind in changedDownloads"
            :key="kind"
            type="button"
            class="btn btn-secondary whitespace-nowrap"
            :title="`Download ${DOWNLOADS[kind].fileName}`"
            @click="downloadFile(kind)"
          >
            Download {{ DOWNLOADS[kind].label }}
          </button>
          <button
            type="button"
            class="btn btn-icon"
            title="Hide until you download"
            aria-label="Hide reminder"
            @click="reminderHidden = true"
          >
            <svg class="size-4" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
              <path
                d="M6.28 5.22a.75.75 0 0 0-1.06 1.06L8.94 10l-3.72 3.72a.75.75 0 1 0 1.06 1.06L10 11.06l3.72 3.72a.75.75 0 1 0 1.06-1.06L11.06 10l3.72-3.72a.75.75 0 0 0-1.06-1.06L10 8.94 6.28 5.22Z"
              />
            </svg>
          </button>
        </div>
      </div>
    </div>
  </section>
</template>

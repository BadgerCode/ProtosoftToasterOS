<script setup lang="ts">
import { ref } from 'vue'
import { importFiles, type ImportSummary } from '@/state/import/import-files'

const emit = defineEmits<{ imported: [summary: ImportSummary] }>()

const input = ref<HTMLInputElement>()

async function onChange() {
  const picked = [...(input.value?.files ?? [])]
  // Clear it so picking the same file again still triggers a change
  if (input.value) input.value.value = ''
  if (!picked.length) return

  const files = await Promise.all(picked.map(async (f) => ({ name: f.name, text: await f.text() })))
  const summary = importFiles(files, () =>
    confirm("Importing will replace changes you haven't downloaded. Continue?"),
  )
  if (summary.imported.length || summary.errors.length) emit('imported', summary)
}
</script>

<template>
  <button
    type="button"
    class="btn btn-secondary px-5 py-2.5 text-base"
    title="Import Config.h or Protogen_Faces.h you downloaded before"
    @click="input?.click()"
  >
    <!-- Upload icon -->
    <svg class="size-5" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
      <path
        d="M9.25 13.25a.75.75 0 0 0 1.5 0V4.636l2.955 3.129a.75.75 0 0 0 1.09-1.03l-4.25-4.5a.75.75 0 0 0-1.09 0l-4.25 4.5a.75.75 0 1 0 1.09 1.03L9.25 4.636v8.614Z"
      />
      <path
        d="M3.5 12.75a.75.75 0 0 0-1.5 0v2.5A2.75 2.75 0 0 0 4.75 18h10.5A2.75 2.75 0 0 0 18 15.25v-2.5a.75.75 0 0 0-1.5 0v2.5c0 .69-.56 1.25-1.25 1.25H4.75c-.69 0-1.25-.56-1.25-1.25v-2.5Z"
      />
    </svg>
    <span class="flex flex-col items-start leading-tight">
      <span>Import</span>
      <span class="text-xs font-normal opacity-75">.h files</span>
    </span>
  </button>
  <input ref="input" type="file" accept=".h" multiple class="hidden" @change="onChange" />
</template>

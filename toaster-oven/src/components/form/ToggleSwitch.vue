<script setup lang="ts">
const enabled = defineModel<boolean>({ required: true })

defineProps<{
  label: string
  description?: string
  disabled?: boolean
}>()
</script>

<template>
  <label
    class="flex cursor-pointer items-start justify-between gap-4 has-disabled:cursor-not-allowed has-disabled:opacity-50"
  >
    <span>
      <span class="block text-sm font-medium text-slate-200">{{ label }}</span>
      <span v-if="description" class="mt-0.5 block text-xs text-slate-500">{{ description }}</span>
    </span>

    <span class="relative mt-0.5 inline-flex shrink-0">
      <input
        v-model="enabled"
        type="checkbox"
        role="switch"
        class="peer sr-only"
        :disabled="disabled"
      />
      <span
        class="h-6 w-11 rounded-full bg-slate-700 transition-colors peer-checked:bg-accent peer-focus-visible:ring-2 peer-focus-visible:ring-accent peer-focus-visible:ring-offset-2 peer-focus-visible:ring-offset-surface"
      />
      <span
        class="absolute top-0.5 left-0.5 size-5 rounded-full bg-white shadow transition-transform peer-checked:translate-x-5"
      />
    </span>
  </label>
</template>

<script setup lang="ts">
defineProps<{
  title: string
  description?: string
}>()
</script>

<template>
  <div class="rounded-lg border border-border bg-surface">
    <div class="flex items-start justify-between gap-4 border-b border-border px-5 py-4">
      <div>
        <h2 class="text-lg font-semibold text-white">{{ title }}</h2>
        <p v-if="description" class="mt-0.5 text-sm text-slate-400">{{ description }}</p>
      </div>
      <slot name="actions" />
    </div>
    <div class="p-5">
      <slot />
    </div>
  </div>
</template>

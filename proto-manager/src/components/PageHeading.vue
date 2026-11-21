<script setup lang="ts">
defineProps<{
  title: string
  subtitle?: string
}>()
</script>

<template>
  <div
    class="mb-8 flex flex-col gap-4 border-b border-border pb-5 sm:flex-row sm:items-end sm:justify-between"
  >
    <div>
      <h1 class="display-title text-4xl sm:text-5xl">{{ title }}</h1>
      <p v-if="subtitle" class="mt-3 text-slate-400">{{ subtitle }}</p>
    </div>

    <div v-if="$slots.actions" class="shrink-0">
      <slot name="actions" />
    </div>
  </div>
</template>

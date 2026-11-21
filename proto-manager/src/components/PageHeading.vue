<script setup lang="ts">
defineProps<{
  title: string
  subtitle?: string
}>()
</script>

<template>
  <div
    class="mb-8 flex flex-col gap-4 border-b border-border pb-4 sm:flex-row sm:items-end sm:justify-between"
  >
    <div>
      <h1 class="text-3xl font-bold tracking-tight text-white">{{ title }}</h1>
      <p v-if="subtitle" class="mt-1 text-slate-400">{{ subtitle }}</p>
      <div class="mt-3 h-1 w-16 rounded-full bg-accent" />
    </div>

    <div v-if="$slots.actions" class="shrink-0">
      <slot name="actions" />
    </div>
  </div>
</template>

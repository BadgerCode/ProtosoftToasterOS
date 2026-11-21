<script setup lang="ts">
defineProps<{
  title: string
  subtitle?: string
}>()
</script>

<template>
  <div class="mb-8 border-b border-border pb-4">
    <h1 class="text-3xl font-bold tracking-tight text-white">{{ title }}</h1>
    <p v-if="subtitle" class="mt-1 text-slate-400">{{ subtitle }}</p>
    <div class="mt-3 h-1 w-16 rounded-full bg-accent" />
  </div>
</template>

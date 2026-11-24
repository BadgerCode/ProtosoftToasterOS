<script setup lang="ts">
defineProps<{
  label: string
  hint?: string
}>()
</script>

<template>
  <label class="block">
    <span class="block text-sm font-medium text-slate-200">{{ label }}</span>
    <span class="mt-1.5 block">
      <slot />
    </span>
    <span v-if="hint" class="mt-1 block text-xs text-slate-500">{{ hint }}</span>
  </label>
</template>

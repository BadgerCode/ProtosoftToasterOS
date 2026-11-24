<script setup lang="ts">
import { computed } from 'vue'
import { appStore } from '@/state/app-state'
import { pinConflicts } from '@/state/pin-usage'

const conflicts = computed(() => pinConflicts(appStore.faceConfig, appStore.featureConfig))
</script>

<template>
  <div
    v-if="conflicts.length"
    class="rounded-lg border border-gold-500/40 bg-gold-950/40 px-4 py-3 text-sm text-gold-200"
  >
    <p class="font-semibold">Some pins are used more than once</p>
    <ul class="mt-1 list-inside list-disc text-gold-200/80">
      <li v-for="conflict in conflicts" :key="conflict">{{ conflict }}</li>
    </ul>
  </div>
</template>

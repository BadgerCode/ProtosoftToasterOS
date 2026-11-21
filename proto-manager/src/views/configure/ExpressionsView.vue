<script setup lang="ts">
import VisorDisplay from '@/components/VisorDisplay.vue'
import { PANEL_TYPES, PanelType } from '@/state/config-types'

const panelIds = PANEL_TYPES.map((p) => p.id).filter((id) => id !== PanelType.SINGLE_NOSE)
</script>

<template>
  <div class="rounded-xl border border-dashed border-border px-6 py-12 text-center">
    <div class="mx-auto max-w-md opacity-70">
      <VisorDisplay :bitmaps="{}" :panel-ids="panelIds" />
    </div>
    <p class="mt-6 text-lg font-semibold text-white">Expressions</p>
    <p class="mt-1 text-sm text-slate-400">Coming soon.</p>
  </div>
</template>

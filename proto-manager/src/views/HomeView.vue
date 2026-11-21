<script setup lang="ts">
import PageHeading from '@/components/PageHeading.vue'
import { appStore } from '@/state/app-state'
</script>

<template>
  <section>
    <PageHeading
      title="Home"
      subtitle="Build configuration and data files for the Protogen Arduino project."
    />

    <div class="rounded-lg border border-border bg-surface p-6">
      <p class="text-slate-300">
        State example: <span class="font-medium text-accent">{{ appStore.example }}</span>
      </p>
    </div>
  </section>
</template>

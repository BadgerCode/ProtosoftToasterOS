<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'
import PageHeading from '@/components/PageHeading.vue'

const tabs = [
  { to: '/configure/panels', label: 'Panel layout' },
  { to: '/configure/expressions', label: 'Expressions' },
  { to: '/configure/features', label: 'Features' },
]
</script>

<template>
  <section>
    <PageHeading title="Configure" subtitle="Set up the hardware and behaviour of your protogen." />

    <nav class="-mt-4 mb-8 flex gap-1 overflow-x-auto shadow-[inset_0_-1px_0] shadow-border">
      <RouterLink
        v-for="tab in tabs"
        :key="tab.to"
        :to="tab.to"
        class="border-b-2 border-transparent px-4 py-2 text-sm font-medium whitespace-nowrap text-slate-400 transition-colors hover:text-white"
        active-class="!border-accent !text-white"
      >
        {{ tab.label }}
      </RouterLink>
    </nav>

    <RouterView />
  </section>
</template>

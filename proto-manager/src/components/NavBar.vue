<script setup lang="ts">
import { RouterLink } from 'vue-router'

const links = [
  { to: '/', label: 'Home', exact: true },
  { to: '/configure', label: 'Configure', exact: false },
]
</script>

<template>
  <header class="sticky top-0 z-10 border-b border-border bg-surface/90 backdrop-blur">
    <nav class="mx-auto flex h-14 max-w-6xl items-center gap-8 px-4 sm:px-6">
      <RouterLink to="/" class="text-lg font-semibold tracking-tight text-white">
        Proto<span class="text-accent">Manager</span>
      </RouterLink>

      <ul class="flex items-center gap-1">
        <li v-for="link in links" :key="link.to">
          <RouterLink
            :to="link.to"
            class="rounded-md px-3 py-1.5 text-sm font-medium text-slate-300 transition-colors hover:bg-accent-muted hover:text-white"
            :exact-active-class="link.exact ? '!bg-accent !text-white' : ''"
            :active-class="link.exact ? '' : '!bg-accent !text-white'"
          >
            {{ link.label }}
          </RouterLink>
        </li>
      </ul>
    </nav>
  </header>
</template>

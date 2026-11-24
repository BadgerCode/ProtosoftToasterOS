<script setup lang="ts">
import { RouterLink } from 'vue-router'
import LedEyeLogo from '@/components/LedEyeLogo.vue'
import { hasUndownloadedChanges } from '@/state/config-download'

const links = [
  { to: '/', label: 'Home', exact: true, showChanges: false },
  { to: '/configure', label: 'Configure', exact: false, showChanges: true },
]
</script>

<template>
  <header class="sticky top-0 z-20 border-b border-border bg-page/85 backdrop-blur">
    <nav class="mx-auto flex h-14 max-w-6xl items-center gap-4 px-4 sm:gap-8 sm:px-6">
      <RouterLink to="/" class="flex items-center gap-2.5 rounded-md">
        <LedEyeLogo />
        <span class="display-title text-xl whitespace-nowrap [text-shadow:none]"
          >Toaster Oven</span
        >
      </RouterLink>

      <ul class="flex items-center gap-1">
        <li v-for="link in links" :key="link.to">
          <RouterLink
            :to="link.to"
            class="relative rounded-md px-3 py-1.5 text-sm font-semibold text-slate-300 transition-colors hover:bg-accent-muted hover:text-white"
            :exact-active-class="link.exact ? '!bg-accent-muted !text-white' : ''"
            :active-class="link.exact ? '' : '!bg-accent-muted !text-white'"
          >
            {{ link.label }}
            <span
              v-if="link.showChanges && hasUndownloadedChanges"
              class="absolute -top-0.5 -right-0.5 size-2.5 rounded-full bg-gold-400 ring-2 ring-page"
              title="Config has changed since last download"
            />
          </RouterLink>
        </li>
      </ul>
    </nav>
  </header>
</template>

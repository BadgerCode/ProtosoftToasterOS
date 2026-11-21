<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'
import PageHeading from '@/components/PageHeading.vue'
import DownloadConfigButton from '@/components/DownloadConfigButton.vue'
import { hasUndownloadedChanges } from '@/state/config-download'

const tabs = [
  { to: '/configure/panels', label: 'Panel layout' },
  { to: '/configure/expressions', label: 'Expressions' },
  { to: '/configure/features', label: 'Features' },
]
</script>

<template>
  <section>
    <PageHeading title="Configure" subtitle="Set up the hardware and behaviour of your protogen.">
      <template #actions>
        <DownloadConfigButton />
      </template>
    </PageHeading>

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

    <!-- Sticky reminder while there are changes that haven't been downloaded -->
    <Transition
      enter-from-class="translate-y-4 opacity-0"
      leave-to-class="translate-y-4 opacity-0"
      enter-active-class="transition duration-200"
      leave-active-class="transition duration-200"
    >
      <div v-if="hasUndownloadedChanges" class="sticky bottom-4 z-10 mt-8">
        <div
          class="flex flex-col gap-3 rounded-lg border border-amber-500/40 bg-slate-900/95 px-4 py-3 shadow-xl shadow-black/40 backdrop-blur sm:flex-row sm:items-center sm:justify-between"
        >
          <p class="flex items-center gap-3 text-sm text-amber-200">
            <span class="size-2.5 shrink-0 rounded-full bg-amber-400" />
            Your config has changed. Download Config.h again to update your Arduino project.
          </p>
          <DownloadConfigButton size="md" class="shrink-0" />
        </div>
      </div>
    </Transition>
  </section>
</template>

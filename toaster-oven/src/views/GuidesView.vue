<script setup lang="ts">
import { RouterLink } from 'vue-router'
import PageHeading from '@/components/PageHeading.vue'
import { sectionContents } from '@/guides/guides'
import { GUIDE_SECTIONS } from '@/guides/sections'

const sections = GUIDE_SECTIONS.map((section) => ({
  ...section,
  contents: sectionContents(section.folder),
}))
</script>

<template>
  <section>
    <PageHeading title="Guides" subtitle="Set up, build and extend your protogen." />

    <div class="space-y-12">
      <section
        v-for="section in sections"
        :id="section.folder"
        :key="section.folder"
        class="scroll-mt-20"
      >
        <h2 class="text-2xl font-bold text-white">{{ section.title }}</h2>
        <p class="mt-1 text-slate-400">{{ section.description }}</p>

        <div
          v-for="part in section.contents"
          :key="part.group ?? part.guides[0]!.slug"
          class="mt-5"
        >
          <h3 v-if="part.group" class="mb-3 text-sm font-semibold text-slate-300">
            {{ part.group }}
          </h3>
          <ol class="grid max-w-3xl gap-4">
            <li v-for="(guide, i) in part.guides" :key="guide.slug">
              <RouterLink
                :to="`/guides/${guide.slug}`"
                class="flex h-full gap-4 rounded-xl border border-border bg-surface p-5 transition-colors hover:border-accent"
              >
                <!-- Each list is numbered from 1, unless its group is unnumbered -->
                <span
                  v-if="!part.group || !section.unnumberedGroups.includes(part.group)"
                  class="display-title text-2xl text-accent [text-shadow:none]"
                >
                  {{ i + 1 }}
                </span>
                <span>
                  <span class="block font-semibold text-white">{{ guide.title }}</span>
                  <span class="mt-1 block text-sm text-slate-400">{{ guide.description }}</span>
                </span>
              </RouterLink>
            </li>
          </ol>
        </div>

        <div
          v-if="section.comingSoon.length"
          class="mt-5 rounded-xl border border-dashed border-border p-5"
        >
          <p class="text-sm font-semibold text-slate-300">Coming soon</p>
          <ul class="mt-3 flex flex-wrap gap-2">
            <li
              v-for="topic in section.comingSoon"
              :key="topic"
              class="rounded-md border border-border px-2.5 py-1 text-sm text-slate-400"
            >
              {{ topic }}
            </li>
          </ul>
        </div>
      </section>
    </div>
  </section>
</template>

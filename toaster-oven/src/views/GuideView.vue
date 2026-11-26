<script setup lang="ts">
import { computed, getCurrentInstance, onBeforeUnmount, onMounted, ref, render, watch } from 'vue'
import { RouterLink, useRoute, useRouter } from 'vue-router'
import { findGuide, renderGuide, sectionGuides } from '@/guides/guides'
import { guideEmbed } from '@/guides/embeds'
import { GUIDE_SECTIONS } from '@/guides/sections'

const props = defineProps<{ slug: string }>()
const router = useRouter()
const route = useRoute()

const guide = computed(() => findGuide(props.slug))
const rendered = computed(() => (guide.value ? renderGuide(guide.value) : null))
// Level 2 and 3 headings, with level 3 indented under them
const contents = computed(
  () => rendered.value?.headings.filter((h) => h.level === 2 || h.level === 3) ?? [],
)

// Previous and next are within the guide's section
const section = computed(() => GUIDE_SECTIONS.find((s) => s.folder === guide.value?.section))
const siblings = computed(() => (guide.value ? sectionGuides(guide.value.section) : []))
const index = computed(() => siblings.value.findIndex((g) => g.slug === props.slug))
const previous = computed(() => siblings.value[index.value - 1])
const next = computed(() => siblings.value[index.value + 1])

// Parts of the site placed in the guide with `::name args` lines (see embeds.ts), mounted into
// the placeholders the Markdown renderer leaves
const article = ref<HTMLElement>()
const appContext = getCurrentInstance()!.appContext
let mountedEmbeds: HTMLElement[] = []

function unmountEmbeds() {
  for (const el of mountedEmbeds) render(null, el)
  mountedEmbeds = []
}

function mountEmbeds() {
  unmountEmbeds()
  for (const el of article.value?.querySelectorAll<HTMLElement>('.guide-embed') ?? []) {
    const vnode = guideEmbed(el.dataset.embed ?? '', el.dataset.args ?? '')
    vnode.appContext = appContext
    render(vnode, el)
    mountedEmbeds.push(el)
  }
}

// Once the guide is on the page, and again after moving to another guide
onMounted(mountEmbeds)
watch(rendered, mountEmbeds, { flush: 'post' })
onBeforeUnmount(unmountEmbeds)

// The link button after each heading copies a link to that section
const copyMessage = ref<string | null>(null)
let copyMessageTimer: ReturnType<typeof setTimeout> | undefined
onBeforeUnmount(() => clearTimeout(copyMessageTimer))

async function copyHeadingLink(hash: string) {
  // Also put it in the address bar, so it can be copied from there if copying isn't allowed
  router.replace({ hash })
  const url = `${window.location.origin}${route.path}${hash}`
  try {
    await navigator.clipboard.writeText(url)
    copyMessage.value = 'Link copied'
  } catch {
    copyMessage.value = "Couldn't copy the link. It's in the address bar instead"
  }
  clearTimeout(copyMessageTimer)
  copyMessageTimer = setTimeout(() => (copyMessage.value = null), 2500)
}

function onContentClick(event: MouseEvent) {
  const headingLink = (event.target as HTMLElement).closest('a.heading-link')
  if (headingLink) {
    event.preventDefault()
    copyHeadingLink(headingLink.getAttribute('href')!)
    return
  }

  // Links to other pages of the site open without reloading the page
  const link = (event.target as HTMLElement).closest('a')
  const href = link?.getAttribute('href')
  if (!link || !href?.startsWith('/') || link.target || event.ctrlKey || event.metaKey) return
  event.preventDefault()
  router.push(href)
}
</script>

<template>
  <section v-if="guide && rendered">
    <RouterLink
      :to="section ? `/guides#${section.folder}` : '/guides'"
      class="text-sm text-slate-400 hover:text-white"
    >
      ← {{ section ? `${section.title} guides` : 'All guides' }}
    </RouterLink>
    <p v-if="guide.group" class="mt-4 text-sm font-semibold text-accent">{{ guide.group }}</p>

    <div class="mt-4 mb-8 border-b border-border pb-5">
      <h1 class="display-title text-4xl sm:text-5xl">{{ guide.title }}</h1>
      <p v-if="guide.description" class="mt-3 text-slate-400">{{ guide.description }}</p>
    </div>

    <div class="grid gap-10 lg:grid-cols-[minmax(0,1fr)_14rem]">
      <!-- The guides are written by the site's owner, and the renderer escapes any HTML in them -->
      <article
        ref="article"
        class="guide-content min-w-0"
        @click="onContentClick"
        v-html="rendered.html"
      />

      <nav v-if="contents.length > 1" class="hidden lg:block" aria-label="On this page">
        <!-- Scrolls on its own if a long guide's contents don't fit the window -->
        <div class="sticky top-20 max-h-[calc(100dvh-6rem)] overflow-y-auto">
          <p class="text-sm font-semibold text-slate-200">On this page</p>
          <ul class="mt-2 space-y-1.5 border-l border-border text-sm">
            <li v-for="heading in contents" :key="heading.id">
              <a
                :href="`#${heading.id}`"
                class="-ml-px block border-l border-transparent text-slate-400 hover:border-accent hover:text-white"
                :class="heading.level === 3 ? 'pl-6 text-[0.8rem]' : 'pl-3'"
              >
                {{ heading.text }}
              </a>
            </li>
          </ul>
        </div>
      </nav>
    </div>

    <nav
      class="mt-12 grid gap-4 border-t border-border pt-6 sm:grid-cols-2"
      aria-label="Other guides"
    >
      <RouterLink
        v-if="previous"
        :to="`/guides/${previous.slug}`"
        class="rounded-xl border border-border bg-surface p-4 transition-colors hover:border-accent"
      >
        <span class="block text-xs text-slate-500">Previous</span>
        <span class="mt-0.5 block font-semibold text-white">{{ previous.title }}</span>
      </RouterLink>
      <RouterLink
        v-if="next"
        :to="`/guides/${next.slug}`"
        class="rounded-xl border border-border bg-surface p-4 text-right transition-colors hover:border-accent sm:col-start-2"
      >
        <span class="block text-xs text-slate-500">Next</span>
        <span class="mt-0.5 block font-semibold text-white">{{ next.title }}</span>
      </RouterLink>
    </nav>
  </section>

  <section v-else>
    <h1 class="display-title text-4xl">Guide not found</h1>
    <p class="mt-4 text-slate-400">
      There's no guide here.
      <RouterLink to="/guides" class="text-accent hover:underline">See all guides</RouterLink>.
    </p>
  </section>
  <!-- "Link copied", after using a heading's link button -->
  <div
    class="pointer-events-none fixed inset-x-0 bottom-6 z-30 flex justify-center px-4"
    aria-live="polite"
  >
    <p
      v-if="copyMessage"
      class="rounded-lg border border-border bg-slate-900/95 px-4 py-2 text-sm text-white shadow-lg shadow-black/30"
    >
      {{ copyMessage }}
    </p>
  </div>
</template>

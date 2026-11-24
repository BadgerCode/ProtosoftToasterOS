<script setup lang="ts">
import { onBeforeUnmount, onMounted } from 'vue'
import { RouterView } from 'vue-router'
import NavBar from '@/components/NavBar.vue'
import { hasUndownloadedChanges } from '@/state/config-download'

// Config isn't saved anywhere yet, so warn before a refresh/close loses changes
function warnAboutUndownloadedChanges(event: BeforeUnloadEvent) {
  if (hasUndownloadedChanges.value) event.preventDefault()
}

onMounted(() => window.addEventListener('beforeunload', warnAboutUndownloadedChanges))
onBeforeUnmount(() => window.removeEventListener('beforeunload', warnAboutUndownloadedChanges))
</script>

<template>
  <div class="flex min-h-screen flex-col">
    <NavBar />
    <main class="mx-auto w-full max-w-6xl flex-1 px-4 py-8 sm:px-6">
      <RouterView />
    </main>
  </div>
</template>

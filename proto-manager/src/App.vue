<script setup lang="ts">
import { RouterView } from 'vue-router'
import NavBar from '@/components/NavBar.vue'
</script>

<template>
  <div class="flex min-h-screen flex-col">
    <NavBar />
    <main class="mx-auto w-full max-w-6xl flex-1 px-4 py-8 sm:px-6">
      <RouterView />
    </main>
  </div>
</template>

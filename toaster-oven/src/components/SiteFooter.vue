<script setup lang="ts">
const contactUrl = 'https://badgercode.furrcard.com'
const repoUrl = 'https://github.com/BadgerCode/ProtosoftToasterOS'
</script>

<template>
  <footer class="border-t border-border">
    <div
      class="mx-auto flex max-w-6xl flex-wrap items-center justify-between gap-x-6 gap-y-2 px-4 py-6 text-sm text-slate-500 sm:px-6"
    >
      <p>
        Made by
        <a
          :href="contactUrl"
          target="_blank"
          rel="noopener"
          class="text-slate-300 hover:text-white hover:underline"
          >BadgerCode</a
        >
        🦡
      </p>
      <a
        :href="repoUrl"
        target="_blank"
        rel="noopener"
        class="text-slate-300 hover:text-white hover:underline"
      >
        Toaster OS on GitHub
      </a>
    </div>
  </footer>
</template>

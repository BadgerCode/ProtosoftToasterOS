<script setup lang="ts">
import { appStore } from '@/state/app-state';

</script>

<template>
  <h1>You did it!</h1>
  <p>
    Visit <a href="https://vuejs.org/" target="_blank" rel="noopener">vuejs.org</a> to read the
    documentation
  </p>

  <p>{{appStore.example }}</p>
</template>

<style scoped></style>

<template>
  <svg viewBox="0 0 34 26" class="h-6.5 w-8.5 overflow-visible" aria-hidden="true">
    <!-- Toast slices, poking out above the body -->
    <path
      v-for="x in [8, 18]"
      :key="x"
      :d="`M${x} 13 V7 Q${x} 3 ${x + 4} 3 Q${x + 8} 3 ${x + 8} 7 V13 Z`"
      class="fill-slate-200 stroke-slate-400"
      stroke-width="1.2"
      stroke-linejoin="round"
    />

    <!-- Body -->
    <rect
      x="3"
      y="10"
      width="28"
      height="15"
      rx="5"
      class="fill-surface stroke-led"
      stroke-width="1.6"
      style="filter: drop-shadow(0 0 1.5px var(--color-led))"
    />

    <!-- Lever -->
    <rect x="31" y="13" width="2.5" height="4" rx="1" class="fill-led" />

    <!-- Power LED -->
    <circle
      cx="8.5"
      cy="19"
      r="1.5"
      class="fill-led"
      style="filter: drop-shadow(0 0 1px var(--color-led))"
    />
  </svg>
</template>

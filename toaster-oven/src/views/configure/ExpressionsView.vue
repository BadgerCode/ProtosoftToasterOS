<script setup lang="ts">
import { computed, nextTick, onBeforeUnmount, onMounted, ref, watch } from 'vue'
import { appStore } from '@/state/app-state'
import {
  createExpression,
  displayName,
  nameFromDisplay,
  uniqueName,
  validateName,
} from '@/state/expressions'
import {
  recordChange,
  redo,
  redoStack,
  selectedExpressionId,
  undo,
  undoStack,
} from '@/state/expression-editor'
import { requiredBy } from '@/faces/required-expressions'
import { expressionToPanels } from '@/faces/faces'
import { PANEL_TYPES, PanelType } from '@/state/config-types'
import VisorDisplay from '@/components/VisorDisplay.vue'
import ExpressionCanvas from '@/components/expressions/ExpressionCanvas.vue'
import ExpressionThumbnail from '@/components/expressions/ExpressionThumbnail.vue'

const expressions = appStore.expressions

const selected = computed(
  () => expressions.find((e) => e.id === selectedExpressionId.value) ?? expressions[0]!,
)
const selectedRequiredBy = computed(() => requiredBy(selected.value.name))

// Default expression (DEFAULT_FACE): can be renamed, but not removed while it's the default
const isDefault = computed(() => selected.value.id === appStore.defaultExpressionId)

function setAsDefault() {
  appStore.defaultExpressionId = selected.value.id
}

const tool = ref<'draw' | 'erase'>('draw')

/** Shown in the undo/redo shortcut hints */
const modKey = /Mac|iPhone|iPad/.test(navigator.platform) ? '⌘' : 'Ctrl'
const showBlink = ref(false)

// Only one eye frame to show when the expression doesn't blink
watch(selectedExpressionId, () => (showBlink.value = false))
const editingBlink = computed(() => showBlink.value && selected.value.eyeBlink !== null)

// Whole-face preview
const previewPanels = PANEL_TYPES.map((p) => p.id).filter((id) => id !== PanelType.SINGLE_NOSE)
const previewBitmaps = computed(() => expressionToPanels(selected.value, editingBlink.value))

// Renaming
const nameInput = ref<HTMLInputElement>()
const nameDraft = ref('')
const nameError = ref<string | null>(null)
watch(
  () => selected.value.name,
  (name) => {
    nameDraft.value = displayName(name)
    nameError.value = null
  },
  { immediate: true },
)

function onNameInput() {
  nameError.value = validateName(nameFromDisplay(nameDraft.value), expressions, selected.value.id)
}

function commitName() {
  const name = nameFromDisplay(nameDraft.value)
  if (validateName(name, expressions, selected.value.id)) {
    nameDraft.value = displayName(selected.value.name)
    nameError.value = null
    return
  }
  selected.value.name = name
}

// Adding and removing
async function selectAndRename(id: number) {
  selectedExpressionId.value = id
  await nextTick()
  nameInput.value?.focus()
  nameInput.value?.select()
}

function addExpression() {
  const expression = createExpression(uniqueName('New_expression', expressions))
  expressions.push(expression)
  selectAndRename(expression.id)
}

function duplicateExpression() {
  const source = selected.value
  const copy = createExpression(uniqueName(`${source.name}_copy`, expressions), source)
  expressions.splice(expressions.indexOf(source) + 1, 0, copy)
  selectAndRename(copy.id)
}

function removeExpression() {
  const expression = selected.value
  if (isDefault.value) return
  if (!confirm(`Remove "${displayName(expression.name)}"? This can't be undone.`)) return
  const index = expressions.indexOf(expression)
  expressions.splice(index, 1)
  selectedExpressionId.value = expressions[Math.min(index, expressions.length - 1)]!.id
}

// Blink
function addBlink() {
  recordChange(selected.value, 'Add blink')
  selected.value.eyeBlink = JSON.parse(JSON.stringify(selected.value.eye))
  showBlink.value = true
}

function removeBlink() {
  recordChange(selected.value, 'Remove blink')
  selected.value.eyeBlink = null
  showBlink.value = false
}

// Keyboard shortcuts: undo/redo, and D/E for the tools
function onKeyDown(event: KeyboardEvent) {
  const target = event.target as HTMLElement
  if (target.closest('input, textarea, select, [contenteditable]')) return

  const mod = event.ctrlKey || event.metaKey
  const key = event.key.toLowerCase()
  if (mod && key === 'z' && !event.shiftKey) undo()
  else if (mod && (key === 'y' || (key === 'z' && event.shiftKey))) redo()
  else if (!mod && key === 'd') tool.value = 'draw'
  else if (!mod && key === 'e') tool.value = 'erase'
  else return
  event.preventDefault()
}

onMounted(() => window.addEventListener('keydown', onKeyDown))
onBeforeUnmount(() => window.removeEventListener('keydown', onKeyDown))
</script>

<template>
  <div class="grid gap-6 lg:grid-cols-[15rem_minmax(0,1fr)]">
    <!-- Expression list -->
    <aside class="rounded-xl border border-border bg-surface lg:self-start">
      <div class="flex items-center justify-between gap-2 border-b border-border px-4 py-3">
        <h2 class="font-semibold text-white">
          Expressions <span class="font-normal text-slate-500">{{ expressions.length }}</span>
        </h2>
        <button type="button" class="btn btn-secondary" @click="addExpression">+ New</button>
      </div>

      <ul class="max-h-72 overflow-y-auto p-2 lg:max-h-[calc(100vh-14rem)]">
        <li v-for="expression in expressions" :key="expression.id">
          <button
            type="button"
            class="flex w-full items-center gap-3 rounded-md px-2 py-1.5 text-left text-sm transition-colors"
            :class="
              expression.id === selected.id
                ? 'bg-accent-muted text-white'
                : 'text-slate-300 hover:bg-page hover:text-white'
            "
            :aria-current="expression.id === selected.id ? 'true' : undefined"
            @click="selectedExpressionId = expression.id"
          >
            <ExpressionThumbnail
              :face="expression"
              class="h-6 w-9 shrink-0 rounded-sm bg-visor-glass p-0.5"
            />
            <span class="min-w-0 flex-1 truncate">{{ displayName(expression.name) }}</span>
            <span
              v-if="expression.id === appStore.defaultExpressionId"
              class="shrink-0 rounded border border-border px-1.5 text-xs text-slate-400"
            >
              Default
            </span>
            <svg
              v-if="requiredBy(expression.name)"
              class="size-3.5 shrink-0 text-slate-500"
              viewBox="0 0 20 20"
              fill="currentColor"
              aria-label="Used by the firmware"
            >
              <title>Used by the firmware</title>
              <path
                fill-rule="evenodd"
                d="M10 1a4.5 4.5 0 0 0-4.5 4.5V9H5a2 2 0 0 0-2 2v6a2 2 0 0 0 2 2h10a2 2 0 0 0 2-2v-6a2 2 0 0 0-2-2h-.5V5.5A4.5 4.5 0 0 0 10 1Zm3 8V5.5a3 3 0 1 0-6 0V9h6Z"
              />
            </svg>
          </button>
        </li>
      </ul>
    </aside>

    <!-- Editor -->
    <section class="min-w-0 rounded-xl border border-border bg-surface">
      <!-- Name and actions -->
      <div
        class="flex flex-col gap-4 border-b border-border px-5 py-4 sm:flex-row sm:items-start sm:justify-between"
      >
        <div class="min-w-0 flex-1">
          <template v-if="selectedRequiredBy">
            <h2 class="text-lg font-semibold text-white">{{ displayName(selected.name) }}</h2>
            <p class="mt-0.5 text-sm text-slate-400">
              Used by: {{ selectedRequiredBy.join(', ') }}. You can edit its LEDs, but it can't be
              renamed or removed yet.
            </p>
          </template>
          <template v-else>
            <label for="expression-name" class="block text-sm font-medium text-slate-200">
              Name
            </label>
            <input
              id="expression-name"
              ref="nameInput"
              v-model="nameDraft"
              class="form-input mt-1.5 sm:max-w-80"
              :class="{ '!border-gold-400': nameError }"
              autocomplete="off"
              spellcheck="false"
              @input="onNameInput"
              @change="commitName"
              @keydown.enter="($event.target as HTMLInputElement).blur()"
            />
            <p v-if="nameError" class="mt-1 text-xs text-gold-300">{{ nameError }}</p>
            <p v-else class="mt-1 text-xs text-slate-500">
              Saved as <code class="text-slate-400">Face_{{ selected.name }}</code>
            </p>
          </template>
          <p v-if="isDefault" class="mt-2 text-sm text-slate-400">
            The default expression, shown when idle between random happy expressions.
            <template v-if="!selectedRequiredBy">
              To remove it, set another expression as the default first.
            </template>
          </p>
        </div>

        <div class="flex shrink-0 flex-wrap gap-2">
          <button v-if="!isDefault" type="button" class="btn btn-secondary" @click="setAsDefault">
            Set as default
          </button>
          <button type="button" class="btn btn-secondary" @click="duplicateExpression">
            Duplicate
          </button>
          <button
            v-if="!selectedRequiredBy && !isDefault"
            type="button"
            class="btn btn-secondary hover:!border-red-400 hover:!text-red-300"
            @click="removeExpression"
          >
            Remove
          </button>
        </div>
      </div>

      <!-- Toolbar -->
      <div class="flex flex-wrap items-center gap-x-6 gap-y-3 border-b border-border px-5 py-3">
        <div class="flex items-center gap-2">
          <div class="inline-flex rounded-md border border-border bg-page p-0.5" role="group">
            <button
              v-for="option in [
                { value: 'draw', label: 'Draw', key: 'D' },
                { value: 'erase', label: 'Erase', key: 'E' },
              ] as const"
              :key="option.value"
              type="button"
              class="rounded px-3 py-1 text-sm font-semibold transition-colors"
              :class="
                tool === option.value ? 'bg-accent text-white' : 'text-slate-400 hover:text-white'
              "
              :aria-pressed="tool === option.value"
              :title="`${option.label} (press ${option.key})`"
              @click="tool = option.value"
            >
              {{ option.label }}
              <kbd class="kbd ml-1.5">{{ option.key }}</kbd>
            </button>
          </div>
        </div>

        <div class="flex items-center gap-2">
          <div
            v-if="selected.eyeBlink"
            class="inline-flex rounded-md border border-border bg-page p-0.5"
            role="group"
            aria-label="Eye frame to edit"
          >
            <button
              v-for="option in [
                { value: false, label: 'Eyes open' },
                { value: true, label: 'Blink' },
              ]"
              :key="option.label"
              type="button"
              class="rounded px-3 py-1 text-sm font-semibold transition-colors"
              :class="
                showBlink === option.value
                  ? 'bg-accent-muted text-white'
                  : 'text-slate-400 hover:text-white'
              "
              :aria-pressed="showBlink === option.value"
              @click="showBlink = option.value"
            >
              {{ option.label }}
            </button>
          </div>
          <button
            v-if="selected.eyeBlink"
            type="button"
            class="btn btn-secondary"
            @click="removeBlink"
          >
            Remove blink
          </button>
          <button v-else type="button" class="btn btn-secondary" @click="addBlink">
            Add blink
          </button>
        </div>

        <div class="flex items-center gap-1 sm:ml-auto">
          <button
            type="button"
            class="btn btn-secondary"
            :disabled="!undoStack.length"
            :title="`Undo (${modKey}+Z)`"
            @click="undo"
          >
            Undo
            <kbd class="kbd">{{ modKey }}+Z</kbd>
          </button>
          <button
            type="button"
            class="btn btn-secondary"
            :disabled="!redoStack.length"
            :title="`Redo (${modKey}+Y)`"
            @click="redo"
          >
            Redo
            <kbd class="kbd">{{ modKey }}+Y</kbd>
          </button>
        </div>
      </div>

      <!-- Canvas -->
      <div class="p-5">
        <p class="mb-3 text-sm text-slate-400">
          <template v-if="editingBlink">
            Editing the blink frame. The eyes show this briefly every 10–15 seconds.
          </template>
          <template v-else>
            Draw the right side of the face (as seen from the front). The left side is mirrored
            automatically.
          </template>
          Right-click or hold <kbd class="kbd">Alt</kbd> to erase while drawing.
        </p>

        <div class="rounded-lg bg-visor-glass p-3 sm:p-5">
          <ExpressionCanvas
            :face="selected"
            :blink="editingBlink"
            :tool="tool"
            @stroke-start="recordChange(selected, tool === 'erase' ? 'Erase' : 'Draw')"
          />
        </div>

        <div class="mt-6">
          <h3 class="text-sm font-medium text-slate-200">Whole face</h3>
          <div class="mt-2 max-w-md">
            <VisorDisplay :bitmaps="previewBitmaps" :panel-ids="previewPanels" />
          </div>
        </div>
      </div>
    </section>
  </div>
</template>

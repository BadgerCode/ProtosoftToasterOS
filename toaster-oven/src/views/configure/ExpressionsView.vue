<script setup lang="ts">
import { computed, nextTick, onBeforeUnmount, onMounted, ref, watch } from 'vue'
import { appStore, isExtraDefault } from '@/state/app-state'
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
import {
  MAX_FRAME_MS,
  detachFrame,
  emptyFrame,
  frameIndexAt,
  frameView,
  isValidDuration,
  pruneFrames,
  storableDuration,
} from '@/faces/animation'
import { PANEL_TYPES, PanelType } from '@/state/config-types'
import VisorDisplay from '@/components/VisorDisplay.vue'
import ExpressionCanvas from '@/components/expressions/ExpressionCanvas.vue'
import ExpressionThumbnail from '@/components/expressions/ExpressionThumbnail.vue'
import ToggleSwitch from '@/components/form/ToggleSwitch.vue'
import FormField from '@/components/form/FormField.vue'
import {
  MOVEMENT_SPEEDS,
  getMovementSpeed,
  setMovementSpeed,
  type MovementSpeed,
} from '@/state/movement-speed'

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

// Movement speed (SlowMovementExpressions and VerySlowMovementExpressions)
const movementSpeed = computed({
  get: () => getMovementSpeed(selected.value.id),
  set: (speed: MovementSpeed) => setMovementSpeed(selected.value.id, speed),
})

// Extra default expressions (ExtraDefaultExpressions): shown now and then in place of the default
const extraDefault = computed({
  // The firmware skips the default expression in the list, so show it as off
  get: () => !isDefault.value && isExtraDefault(selected.value.id),
  set(include: boolean) {
    const ids = appStore.extraDefaultExpressionIds
    const index = ids.indexOf(selected.value.id)
    if (include && index === -1) ids.push(selected.value.id)
    else if (!include && index !== -1) ids.splice(index, 1)
  },
})

const tool = ref<'draw' | 'erase'>('draw')

/** Shown in the undo/redo shortcut hints */
const modKey = /Mac|iPhone|iPad/.test(navigator.platform) ? '⌘' : 'Ctrl'
const showBlink = ref(false)

// Only one eye frame to show when the expression doesn't blink
watch(selectedExpressionId, () => (showBlink.value = false))
const editingBlink = computed(
  () => showBlink.value && selected.value.eyeBlink !== null && !animated.value,
)

// Animation: frame 1 is the expression itself; later frames only keep the parts drawn differently
const selectedFrame = ref(0)
const frames = computed(() => selected.value.animation ?? [])
const animated = computed(() => frames.value.length > 1)
const loopMs = computed(() =>
  frames.value.reduce((sum, f) => sum + storableDuration(f.durationMs), 0),
)
/** What the canvas draws on: the expression, or the selected frame */
const editorFace = computed(() => frameView(selected.value, selectedFrame.value))

watch(selectedExpressionId, () => (selectedFrame.value = 0))
// Undo can remove frames
watch(
  () => frames.value.length,
  (count) => {
    if (selectedFrame.value >= count) selectedFrame.value = Math.max(0, count - 1)
  },
)

function onStrokeStart() {
  recordChange(selected.value, tool.value === 'erase' ? 'Erase' : 'Draw')
  // Give the frame its own copy of each part, so drawing on it leaves the expression alone
  detachFrame(selected.value, selectedFrame.value)
}

function onStrokeEnd() {
  // Parts drawn the same as frame 1 go back to following frame 1
  pruneFrames(selected.value)
}

function addAnimation() {
  recordChange(selected.value, 'Add animation')
  showBlink.value = false
  selected.value.animation = [emptyFrame(), emptyFrame()]
  selectedFrame.value = 1
}

function removeAnimation() {
  recordChange(selected.value, 'Remove animation')
  selected.value.animation = null
  selectedFrame.value = 0
}

/** Adds a copy of the selected frame after it, and selects the copy */
function addFrame() {
  const list = selected.value.animation
  if (!list) return
  recordChange(selected.value, 'Add frame')
  const source = list[selectedFrame.value]!
  // A copy of frame 1 follows frame 1, like the frames that haven't been drawn on
  const copy =
    selectedFrame.value === 0 ? emptyFrame(source.durationMs) : JSON.parse(JSON.stringify(source))
  list.splice(selectedFrame.value + 1, 0, copy)
  selectedFrame.value++
}

/** Frame 1 stays first: it's the expression itself */
function moveFrame(by: number) {
  const list = selected.value.animation
  const from = selectedFrame.value
  const to = from + by
  if (!list || from < 1 || to < 1 || to >= list.length) return
  recordChange(selected.value, 'Move frame')
  list.splice(to, 0, list.splice(from, 1)[0]!)
  selectedFrame.value = to
}

function removeFrame() {
  const list = selected.value.animation
  if (!list || selectedFrame.value < 1) return
  recordChange(selected.value, 'Remove frame')
  list.splice(selectedFrame.value, 1)
  if (list.length < 2) selected.value.animation = null
  selectedFrame.value = Math.min(selectedFrame.value, Math.max(0, list.length - 1))
}

// The preview plays the animation, unless the viewer prefers less motion
const reduceMotion = window.matchMedia('(prefers-reduced-motion: reduce)').matches
const playTime = ref(0)
let playTimer: ReturnType<typeof setInterval> | undefined
watch(
  animated,
  (isAnimated) => {
    clearInterval(playTimer)
    if (isAnimated && !reduceMotion)
      playTimer = setInterval(() => (playTime.value = Date.now()), 40)
  },
  { immediate: true },
)
onBeforeUnmount(() => clearInterval(playTimer))
const previewFrame = computed(() =>
  animated.value && !reduceMotion
    ? frameIndexAt(frames.value, playTime.value)
    : selectedFrame.value,
)

// Whole-face preview
const previewPanels = PANEL_TYPES.map((p) => p.id).filter((id) => id !== PanelType.SINGLE_NOSE)
const previewBitmaps = computed(() =>
  animated.value
    ? expressionToPanels(frameView(selected.value, previewFrame.value))
    : expressionToPanels(selected.value, editingBlink.value),
)

// Renaming: the name is a title with a pencil button, which swaps it for an input
const editingName = ref(false)
const nameInput = ref<HTMLInputElement>()
const nameDraft = ref('')
const nameError = ref<string | null>(null)
watch(selectedExpressionId, () => (editingName.value = false))

async function startRename() {
  if (selectedRequiredBy.value) return
  nameDraft.value = displayName(selected.value.name)
  nameError.value = null
  editingName.value = true
  await nextTick()
  nameInput.value?.focus()
  nameInput.value?.select()
}

function onNameInput() {
  nameError.value = validateName(nameFromDisplay(nameDraft.value), expressions, selected.value.id)
}

function confirmName() {
  const name = nameFromDisplay(nameDraft.value)
  nameError.value = validateName(name, expressions, selected.value.id)
  if (nameError.value) return
  selected.value.name = name
  editingName.value = false
}

function cancelRename() {
  editingName.value = false
  nameError.value = null
}

// Adding and removing
async function selectAndRename(id: number) {
  selectedExpressionId.value = id
  await nextTick()
  startRename()
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
  const usedIn = [
    appStore.boopExpressions.some((b) => b.expressionId === expression.id) && 'boop expressions',
    appStore.remoteActions.some((a) => a.expressionId === expression.id) && 'remote control combos',
  ].filter(Boolean)
  const usedNote = usedIn.length ? ` It will also be removed from the ${usedIn.join(' and ')}.` : ''
  if (!confirm(`Remove "${displayName(expression.name)}"?${usedNote} This can't be undone.`)) return
  const index = expressions.indexOf(expression)
  expressions.splice(index, 1)
  const extraIndex = appStore.extraDefaultExpressionIds.indexOf(expression.id)
  if (extraIndex !== -1) appStore.extraDefaultExpressionIds.splice(extraIndex, 1)
  setMovementSpeed(expression.id, 'normal') // Takes it out of the movement lists
  for (let i = appStore.boopExpressions.length - 1; i >= 0; i--) {
    if (appStore.boopExpressions[i]!.expressionId === expression.id) {
      appStore.boopExpressions.splice(i, 1)
    }
  }
  for (const action of appStore.remoteActions) {
    if (action.expressionId === expression.id) {
      action.action = 'none'
      action.expressionId = null
    }
  }
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
          <div v-if="editingName" class="flex items-start gap-2">
            <div class="min-w-0 flex-1 sm:max-w-80">
              <input
                ref="nameInput"
                v-model="nameDraft"
                class="form-input"
                :class="{ '!border-gold-400': nameError }"
                aria-label="Expression name"
                autocomplete="off"
                spellcheck="false"
                @input="onNameInput"
                @keydown.enter.prevent="confirmName"
                @keydown.esc="cancelRename"
              />
              <p v-if="nameError" class="mt-1 text-xs text-gold-300">{{ nameError }}</p>
              <p v-else class="mt-1 text-xs text-slate-500">
                Saved as <code class="text-slate-400">Face_{{ nameFromDisplay(nameDraft) }}</code
                >. Press <kbd class="kbd">Esc</kbd> to cancel.
              </p>
            </div>
            <button
              type="button"
              class="btn btn-icon mt-1 shrink-0"
              title="Save name"
              aria-label="Save name"
              :disabled="!!nameError"
              @click="confirmName"
            >
              <svg class="size-5" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
                <path
                  fill-rule="evenodd"
                  d="M16.704 4.153a.75.75 0 0 1 .143 1.052l-8 10.5a.75.75 0 0 1-1.127.075l-4.5-4.5a.75.75 0 0 1 1.06-1.06l3.894 3.893 7.48-9.817a.75.75 0 0 1 1.05-.143Z"
                  clip-rule="evenodd"
                />
              </svg>
            </button>
          </div>
          <div v-else class="flex items-center gap-1">
            <h2 class="min-w-0 truncate text-lg font-semibold text-white">
              {{ displayName(selected.name) }}
            </h2>
            <button
              type="button"
              class="btn btn-icon shrink-0"
              :title="
                selectedRequiredBy ? `Used by the firmware, so it can't be renamed` : 'Rename'
              "
              :aria-label="
                selectedRequiredBy
                  ? `Rename (used by the firmware, so it can't be renamed)`
                  : 'Rename'
              "
              :disabled="!!selectedRequiredBy"
              @click="startRename"
            >
              <svg class="size-4" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
                <path
                  d="m5.433 13.917 1.262-3.155A4 4 0 0 1 7.58 9.42l6.92-6.918a2.121 2.121 0 0 1 3 3l-6.92 6.918c-.383.383-.84.685-1.343.886l-3.154 1.262a.5.5 0 0 1-.65-.65Z"
                />
                <path
                  d="M3.5 5.75c0-.69.56-1.25 1.25-1.25H10A.75.75 0 0 0 10 3H4.75A2.75 2.75 0 0 0 2 5.75v9.5A2.75 2.75 0 0 0 4.75 18h9.5A2.75 2.75 0 0 0 17 15.25V10a.75.75 0 0 0-1.5 0v5.25c0 .69-.56 1.25-1.25 1.25h-9.5c-.69 0-1.25-.56-1.25-1.25v-9.5Z"
                />
              </svg>
            </button>
          </div>
          <p v-if="selectedRequiredBy" class="mt-0.5 text-sm text-slate-400">
            Used by: {{ selectedRequiredBy.join(', ') }}. You can edit its LEDs, but it can't be
            renamed or removed yet.
          </p>
          <p v-if="isDefault" class="mt-2 text-sm text-slate-400">
            Shown when no other expression is selected.
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

        <div v-if="!animated" class="flex items-center gap-2">
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
          <button type="button" class="btn btn-secondary" @click="addAnimation">
            Add animation
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

      <!-- Animation frames -->
      <div v-if="animated" class="border-b border-border px-5 py-3">
        <div class="flex flex-wrap items-center justify-between gap-2">
          <p class="text-sm text-slate-400">
            {{ frames.length }} frames, looping every {{ loopMs }} ms. Animated expressions don't
            blink.
          </p>
          <button
            type="button"
            class="btn btn-secondary hover:!border-red-400 hover:!text-red-300"
            @click="removeAnimation"
          >
            Remove animation
          </button>
        </div>

        <ol class="mt-3 flex items-start gap-2 overflow-x-auto pb-1">
          <li v-for="(frame, i) in frames" :key="i" class="shrink-0">
            <button
              type="button"
              class="block rounded-md border p-1.5 text-left transition-colors"
              :class="
                i === selectedFrame
                  ? 'border-accent bg-accent-muted'
                  : 'border-border hover:border-accent'
              "
              :aria-pressed="i === selectedFrame"
              @click="selectedFrame = i"
            >
              <ExpressionThumbnail
                :face="frameView(selected, i)"
                class="h-10 w-15 rounded-sm bg-visor-glass p-0.5"
              />
              <span class="mt-1 block text-xs text-slate-300">Frame {{ i + 1 }}</span>
            </button>
            <label class="mt-1 flex items-center gap-1 text-xs text-slate-500">
              <input
                v-model.number="frame.durationMs"
                type="number"
                min="1"
                :max="MAX_FRAME_MS"
                step="10"
                class="form-input w-16 px-1.5 py-0.5 text-xs"
                :class="{ '!border-gold-400': !isValidDuration(frame.durationMs) }"
                :aria-label="`Frame ${i + 1} duration in milliseconds`"
              />
              ms
            </label>
          </li>
          <li class="shrink-0">
            <button
              type="button"
              class="btn btn-secondary h-[4.75rem]"
              title="Add a copy of the selected frame after it"
              @click="addFrame"
            >
              + Add frame
            </button>
          </li>
        </ol>

        <div class="mt-2 flex flex-wrap items-center gap-2">
          <button
            type="button"
            class="btn btn-secondary"
            :disabled="selectedFrame < 2"
            @click="moveFrame(-1)"
          >
            Move earlier
          </button>
          <button
            type="button"
            class="btn btn-secondary"
            :disabled="selectedFrame < 1 || selectedFrame >= frames.length - 1"
            @click="moveFrame(1)"
          >
            Move later
          </button>
          <button
            type="button"
            class="btn btn-secondary hover:!border-red-400 hover:!text-red-300"
            :disabled="selectedFrame < 1"
            @click="removeFrame"
          >
            Remove frame
          </button>
          <p
            v-if="frames.some((f) => !isValidDuration(f.durationMs))"
            class="text-xs text-gold-300"
          >
            Use a whole number of milliseconds, 1 to {{ MAX_FRAME_MS }}.
          </p>
        </div>
      </div>

      <!-- Canvas -->
      <div class="p-5">
        <!--
          Capped so the whole canvas fits in the window below the name and toolbar: the canvas is
          about 1.39 times as wide as it is tall (its viewBox is 41 x 29.5), plus 2.5rem of padding.
          12rem leaves room for the name, toolbar and hint, and 20rem also for the animation frames.
          Never narrower than 20rem.
        -->
        <div
          class="mx-auto rounded-lg bg-visor-glass p-3 sm:p-5"
          :style="{
            maxWidth: `max(20rem, calc((100dvh - ${animated ? 20 : 12}rem) * 1.39 + 2.5rem))`,
          }"
        >
          <ExpressionCanvas
            :face="editorFace"
            :blink="editingBlink"
            :tool="tool"
            @stroke-start="onStrokeStart"
            @stroke-end="onStrokeEnd"
          />
        </div>

        <p class="mt-2 text-xs text-slate-500">
          <template v-if="editingBlink">
            Editing the blink frame. The eyes show this briefly every 10–15 seconds.
          </template>
          <template v-else-if="animated && selectedFrame === 0">
            Frame 1 is the expression itself. Later frames show its parts too, until you draw on
            them there.
          </template>
          <template v-else-if="animated">
            Drawing frame {{ selectedFrame + 1 }}. Anything you don't change here follows frame 1.
          </template>
          <template v-else>
            Draw the right side of the face (as seen from the front). The left side is mirrored
            automatically.
          </template>
          Right-click or hold <kbd class="kbd">Alt</kbd> to erase while drawing.
        </p>
      </div>

      <!-- Whole face preview, and the expression's settings -->
      <div class="grid gap-6 border-t border-border px-5 py-5 lg:grid-cols-2">
        <div>
          <h3 class="text-sm font-medium text-slate-200">Whole face</h3>
          <div class="mt-2 max-w-md">
            <VisorDisplay :bitmaps="previewBitmaps" :panel-ids="previewPanels" />
          </div>
        </div>

        <div class="space-y-4">
          <ToggleSwitch
            v-model="extraDefault"
            :disabled="isDefault"
            label="Show in place of the default expression"
            :description="
              isDefault
                ? 'This is the default expression. Other expressions with this turned on are shown in its place now and then.'
                : 'Every 10–14 seconds, and after a boop, the head swaps the default expression for a random one of these for 5 seconds. Turn this off for every expression to always show the default.'
            "
          />

          <FormField
            label="Movement speed"
            hint="How often the face moves up and down while this expression shows: every 200 ms (normal), 300 ms (slow) or 600 ms (very slow). Boops have their own speed. Face bobbing can be turned off on the Features tab."
          >
            <select v-model="movementSpeed" class="form-input sm:max-w-64">
              <option v-for="speed in MOVEMENT_SPEEDS" :key="speed.value" :value="speed.value">
                {{ speed.label }}
              </option>
            </select>
          </FormField>
        </div>
      </div>
    </section>
  </div>
</template>

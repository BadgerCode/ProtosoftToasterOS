import { ref } from 'vue'
import type { FaceExpressionData } from '@/faces/faces'
import { appStore, getDefaultExpression } from './app-state'
import type { Expression } from './expressions'

// Editor state that should survive switching between the Configure tabs

// Starts on the default expression, which is at the top of the list
export const selectedExpressionId = ref<number>(getDefaultExpression().id)

interface HistoryEntry {
  id: number
  face: FaceExpressionData
  description: string
}

const MAX_HISTORY = 200
export const undoStack = ref<HistoryEntry[]>([])
export const redoStack = ref<HistoryEntry[]>([])

function snapshot(e: Expression): FaceExpressionData {
  return JSON.parse(
    JSON.stringify({
      nose: e.nose,
      eye: e.eye,
      eyeBlink: e.eyeBlink,
      mouth: e.mouth,
      animation: e.animation,
    }),
  )
}

function restore(entry: HistoryEntry) {
  const expression = appStore.expressions.find((e) => e.id === entry.id)
  if (!expression) return false
  Object.assign(expression, entry.face)
  selectedExpressionId.value = entry.id
  return true
}

/** Call before changing an expression's LEDs or animation frames, so the change can be undone */
export function recordChange(expression: Expression, description: string) {
  undoStack.value.push({ id: expression.id, face: snapshot(expression), description })
  if (undoStack.value.length > MAX_HISTORY) undoStack.value.shift()
  redoStack.value = []
}

function step(from: typeof undoStack, to: typeof undoStack) {
  // Skip history for expressions that have since been removed
  while (from.value.length) {
    const entry = from.value.pop()!
    const expression = appStore.expressions.find((e) => e.id === entry.id)
    if (!expression) continue
    to.value.push({ id: entry.id, face: snapshot(expression), description: entry.description })
    restore(entry)
    return
  }
}

export const undo = () => step(undoStack, redoStack)
export const redo = () => step(redoStack, undoStack)

/** Start fresh after the expressions are replaced, e.g. by an import */
export function resetEditor() {
  selectedExpressionId.value = getDefaultExpression().id
  undoStack.value = []
  redoStack.value = []
}

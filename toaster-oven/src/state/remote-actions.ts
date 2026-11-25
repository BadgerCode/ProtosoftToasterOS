import { displayName, type Expression } from './expressions'
import type { RemoteActionEntry } from './generate-faces'

// Remote control button combos (RemoteControlActions in the firmware's Protogen_Faces.h).
// A combo is one button, then another, e.g. "AB". Combos not in the list do nothing.
// Keep the actions in sync with RemoteActionType in ToasterOS/ConfigTypes.h.

export const REMOTE_BUTTONS = ['A', 'B', 'C', 'D'] as const

/** Every combo, in order: AA, AB, ... DD */
export const REMOTE_COMBOS = REMOTE_BUTTONS.flatMap((first) =>
  REMOTE_BUTTONS.map((second) => `${first}${second}`),
)

export type RemoteActionType =
  | 'none'
  | 'REMOTE_EXPRESSION'
  | 'REMOTE_RESET_TO_DEFAULT'
  | 'REMOTE_CYCLE_BRIGHTNESS'
  | 'REMOTE_TOGGLE_BOOP_SENSOR'

/** Actions other than showing an expression, with their labels */
export const REMOTE_BUILT_IN_ACTIONS: { value: RemoteActionType; label: string }[] = [
  { value: 'REMOTE_RESET_TO_DEFAULT', label: 'Back to the default expression' },
  { value: 'REMOTE_CYCLE_BRIGHTNESS', label: 'Cycle brightness (0%, 25%, 50%, 100%)' },
  { value: 'REMOTE_TOGGLE_BOOP_SENSOR', label: 'Turn the boop sensor on or off' },
]

export function isRemoteActionType(value: string): value is RemoteActionType {
  return value === 'REMOTE_EXPRESSION' || REMOTE_BUILT_IN_ACTIONS.some((a) => a.value === value)
}

export interface RemoteAction {
  /** e.g. "AB": A, then B */
  buttons: string
  action: RemoteActionType
  /** For REMOTE_EXPRESSION */
  expressionId: number | null
}

/** One entry per combo, all doing nothing */
export function emptyRemoteActions(): RemoteAction[] {
  return REMOTE_COMBOS.map((buttons) => ({ buttons, action: 'none', expressionId: null }))
}

/**
 * Combos from a file's entries. The firmware uses the first entry for a combo, so later ones
 * are ignored. `skipped` describes entries that couldn't be used, e.g. "AB (Missing face)".
 */
export function remoteActionsFromEntries(
  entries: { buttons: string; action: string; name: string | null }[],
  expressions: Expression[],
): { actions: RemoteAction[]; skipped: string[] } {
  const actions = emptyRemoteActions()
  const skipped: string[] = []
  const done = new Set<string>()

  for (const entry of entries) {
    const target = actions.find((a) => a.buttons === entry.buttons)
    if (done.has(entry.buttons)) continue
    const expression = expressions.find((e) => e.name === entry.name)
    const usable =
      target &&
      isRemoteActionType(entry.action) &&
      (entry.action !== 'REMOTE_EXPRESSION' || expression)
    if (!usable) {
      skipped.push(entry.name ? `${entry.buttons} (${displayName(entry.name)})` : entry.buttons)
      continue
    }
    done.add(entry.buttons)
    target.action = entry.action as RemoteActionType
    target.expressionId = entry.action === 'REMOTE_EXPRESSION' ? expression!.id : null
  }

  return { actions, skipped }
}

/** Entries that go in the file, in combo order. Leaves out combos that do nothing */
export function usableRemoteActions(
  actions: RemoteAction[],
  expressions: Expression[],
): RemoteActionEntry[] {
  return actions.flatMap((a): RemoteActionEntry[] => {
    if (a.action === 'none') return []
    if (a.action !== 'REMOTE_EXPRESSION')
      return [{ buttons: a.buttons, action: a.action, name: null }]
    const expression = expressions.find((e) => e.id === a.expressionId)
    return expression ? [{ buttons: a.buttons, action: a.action, name: expression.name }] : []
  })
}

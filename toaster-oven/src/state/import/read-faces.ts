import {
  BOOP_EXPRESSION_DEFAULTS,
  DEFAULT_FACE_NAME,
  EXTRA_DEFAULT_NAMES,
  REMOTE_ACTION_DEFAULTS,
  SLOW_MOVEMENT_NAMES,
  VERY_SLOW_MOVEMENT_NAMES,
  defaultExpression,
} from '@/faces/faces'
import {
  parseBoopExpressions,
  parseDefaultFace,
  parseExpressionList,
  parseExpressions,
  parseExtraDefaults,
  parseRemoteActions,
} from '@/faces/parse-faces'
import type { BoopExpression } from '../boop-expressions'
import { remoteActionsFromEntries, type RemoteAction } from '../remote-actions'
import { REQUIRED_EXPRESSIONS } from '@/faces/required-expressions'
import { createExpression, displayName, type Expression } from '../expressions'
import { idsOf } from '../app-state'

// Reads a Protogen_Faces.h that Toaster Oven downloaded earlier.
// Expressions the firmware needs but the file doesn't have are added back from the defaults,
// so the generated file still compiles.

export interface ReadFacesResult {
  expressions: Expression[]
  /** Display names of the expressions that were added back */
  added: string[]
  /** Id of the expression to use as the default (DEFAULT_FACE) */
  defaultExpressionId: number
  /**
   * Set when the file didn't have a usable default expression and it fell back to
   * DEFAULT_FACE_NAME, e.g. files downloaded before the default could be chosen.
   * The name the file asked for, if it had one.
   */
  defaultFallback: { requested: string | null } | null
  /** Ids of the extra default expressions (ExtraDefaultExpressions) */
  extraDefaultExpressionIds: number[]
  /** Set when the file had no ExtraDefaultExpressions, so the defaults were used */
  extraDefaultsFallback: boolean
  /** Display names in ExtraDefaultExpressions that aren't expressions in the file */
  extraDefaultsSkipped: string[]
  boopExpressions: BoopExpression[]
  /** Set when the file had no BoopExpressions, so the defaults were used */
  boopExpressionsFallback: boolean
  /** Display names in BoopExpressions that aren't expressions in the file */
  boopExpressionsSkipped: string[]
  /** One entry per remote control combo */
  remoteActions: RemoteAction[]
  /** Set when the file had no RemoteControlActions, so the defaults were used */
  remoteActionsFallback: boolean
  /** RemoteControlActions entries that couldn't be used, e.g. "AB (Missing face)" */
  remoteActionsSkipped: string[]
  /** Ids in SlowMovementExpressions and VerySlowMovementExpressions. None are in both */
  slowMovementExpressionIds: number[]
  verySlowMovementExpressionIds: number[]
  /** Set when the file was missing either movement list, so the defaults were used for it */
  movementFallback: boolean
  /** Display names in the movement lists that aren't expressions in the file */
  movementSkipped: string[]
}

export function readFaces(text: string): ReadFacesResult {
  const parsed = parseExpressions(text)
  if (parsed.length === 0) throw new Error('No expressions found')

  const expressions = parsed.map((e) => createExpression(e.name, e))
  const names = new Set(parsed.map((e) => e.name))
  const added: string[] = []

  function addBack(name: string) {
    expressions.push(createExpression(name, defaultExpression(name)))
    names.add(name)
    added.push(displayName(name))
  }

  for (const name of Object.keys(REQUIRED_EXPRESSIONS)) {
    if (!names.has(name)) addBack(name)
  }

  // Older files have no DEFAULT_FACE; the firmware used Face_Neutral back then
  const requested = parseDefaultFace(text)
  let defaultName = requested
  let defaultFallback: ReadFacesResult['defaultFallback'] = null
  if (!defaultName || !names.has(defaultName)) {
    defaultName = DEFAULT_FACE_NAME
    defaultFallback = { requested }
    if (!names.has(defaultName)) addBack(defaultName)
  }

  const defaultExpressionId = expressions.find((e) => e.name === defaultName)!.id

  // Older files have no ExtraDefaultExpressions; the firmware had the defaults hard-coded
  const requestedExtras = parseExtraDefaults(text)
  const extraNames = requestedExtras ?? EXTRA_DEFAULT_NAMES
  const extraDefaultExpressionIds = expressions
    .filter((e) => extraNames.includes(e.name))
    .map((e) => e.id)
  const extraDefaultsSkipped = (requestedExtras ?? [])
    .filter((name) => !names.has(name))
    .map(displayName)

  // Older files have no BoopExpressions; the firmware had the defaults hard-coded
  const requestedBoops = parseBoopExpressions(text)
  const boopEntries = requestedBoops ?? BOOP_EXPRESSION_DEFAULTS
  const boopExpressions = boopEntries
    .filter((b) => names.has(b.name))
    .map((b) => ({
      minBoops: b.minBoops,
      expressionId: expressions.find((e) => e.name === b.name)!.id,
    }))
  const boopExpressionsSkipped = boopEntries
    .filter((b) => !names.has(b.name))
    .map((b) => displayName(b.name))

  // Older files have no RemoteControlActions; the firmware had the defaults hard-coded
  const requestedRemote = parseRemoteActions(text)
  const remote = remoteActionsFromEntries(requestedRemote ?? REMOTE_ACTION_DEFAULTS, expressions)

  // Older files have no movement lists; the firmware had them hard-coded
  const requestedSlow = parseExpressionList(text, 'SlowMovementExpressions')
  const requestedVerySlow = parseExpressionList(text, 'VerySlowMovementExpressions')
  const slowNames = requestedSlow ?? SLOW_MOVEMENT_NAMES
  const verySlowNames = (requestedVerySlow ?? VERY_SLOW_MOVEMENT_NAMES).filter(
    (name) => !slowNames.includes(name), // The firmware checks the slow list first
  )
  const movementSkipped = [...(requestedSlow ?? []), ...(requestedVerySlow ?? [])]
    .filter((name) => !names.has(name))
    .map(displayName)

  return {
    expressions,
    added,
    defaultExpressionId,
    defaultFallback,
    extraDefaultExpressionIds,
    extraDefaultsFallback: requestedExtras === null,
    extraDefaultsSkipped,
    boopExpressions,
    boopExpressionsFallback: requestedBoops === null,
    boopExpressionsSkipped,
    remoteActions: remote.actions,
    remoteActionsFallback: requestedRemote === null,
    remoteActionsSkipped: remote.skipped,
    slowMovementExpressionIds: idsOf(expressions, slowNames),
    verySlowMovementExpressionIds: idsOf(expressions, verySlowNames),
    movementFallback: requestedSlow === null || requestedVerySlow === null,
    movementSkipped,
  }
}

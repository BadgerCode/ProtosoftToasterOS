import { DEFAULT_FACE_NAME, defaultExpression } from '@/faces/faces'
import {
  parseBoopExpressions,
  parseDefaultFace,
  parseExpressionList,
  parseExpressions,
  parseRemoteActions,
} from '@/faces/parse-faces'
import { REQUIRED_EXPRESSIONS } from '@/faces/required-expressions'
import { idsOf } from '../app-state'
import type { BoopExpression } from '../boop-expressions'
import { createExpression, displayName, type Expression } from '../expressions'
import { remoteActionsFromEntries, type RemoteAction } from '../remote-actions'

// Reads a Protogen_Faces.h that Toaster Oven downloaded earlier.
// Expressions the firmware needs but the file doesn't have are added back from the defaults,
// and settings that refer to expressions the file doesn't have are skipped, so the generated
// file still compiles.

export interface ReadFacesResult {
  expressions: Expression[]
  /** Display names of the expressions that were added back */
  added: string[]
  /** What couldn't be used, e.g. "boop expression Gone face" */
  skipped: string[]
  /** Id of the expression to use as the default (DEFAULT_FACE) */
  defaultExpressionId: number
  /** Ids of the extra default expressions (ExtraDefaultExpressions) */
  extraDefaultExpressionIds: number[]
  boopExpressions: BoopExpression[]
  /** One entry per remote control combo */
  remoteActions: RemoteAction[]
  /** Ids in SlowMovementExpressions and VerySlowMovementExpressions. None are in both */
  slowMovementExpressionIds: number[]
  verySlowMovementExpressionIds: number[]
}

/** A setting the file must have. Files without it come from before these settings existed */
function required<T>(value: T | null, name: string): T {
  if (value === null)
    throw new Error(`${name} is missing. Download the file from Toaster Oven again`)
  return value
}

export function readFaces(text: string): ReadFacesResult {
  const parsed = parseExpressions(text)
  if (parsed.length === 0) throw new Error('No expressions found')

  const requestedDefault = required(parseDefaultFace(text), 'DEFAULT_FACE')
  const extraNames = required(
    parseExpressionList(text, 'ExtraDefaultExpressions'),
    'ExtraDefaultExpressions',
  )
  const boopEntries = required(parseBoopExpressions(text), 'BoopExpressions')
  const remoteEntries = required(parseRemoteActions(text), 'RemoteControlActions')
  const slowNames = required(
    parseExpressionList(text, 'SlowMovementExpressions'),
    'SlowMovementExpressions',
  )
  const verySlowNames = required(
    parseExpressionList(text, 'VerySlowMovementExpressions'),
    'VerySlowMovementExpressions',
  ).filter((name) => !slowNames.includes(name)) // The firmware checks the slow list first

  const expressions = parsed.map((e) => createExpression(e.name, e))
  const names = new Set(parsed.map((e) => e.name))
  const added: string[] = []
  const skipped: string[] = []

  function addBack(name: string) {
    expressions.push(createExpression(name, defaultExpression(name)))
    names.add(name)
    added.push(displayName(name))
  }

  /** The names that exist, noting the rest as skipped */
  function existing(list: string[], label: string) {
    for (const name of list.filter((n) => !names.has(n))) {
      skipped.push(`${label} ${displayName(name)}`)
    }
    return list.filter((n) => names.has(n))
  }

  for (const name of Object.keys(REQUIRED_EXPRESSIONS)) {
    if (!names.has(name)) addBack(name)
  }

  let defaultName = requestedDefault
  if (!names.has(defaultName)) {
    skipped.push(
      `default expression ${displayName(defaultName)} (using ${displayName(DEFAULT_FACE_NAME)})`,
    )
    defaultName = DEFAULT_FACE_NAME
    if (!names.has(defaultName)) addBack(defaultName)
  }

  const boopNames = existing(
    boopEntries.map((b) => b.name),
    'boop expression',
  )
  const remote = remoteActionsFromEntries(remoteEntries, expressions)
  skipped.push(...remote.skipped.map((s) => `remote control combo ${s}`))

  return {
    expressions,
    added,
    skipped,
    defaultExpressionId: expressions.find((e) => e.name === defaultName)!.id,
    extraDefaultExpressionIds: idsOf(expressions, existing(extraNames, 'extra default expression')),
    boopExpressions: boopEntries
      .filter((b) => boopNames.includes(b.name))
      .map((b) => ({
        minBoops: b.minBoops,
        expressionId: expressions.find((e) => e.name === b.name)!.id,
      })),
    remoteActions: remote.actions,
    slowMovementExpressionIds: idsOf(expressions, existing(slowNames, 'slow movement for')),
    verySlowMovementExpressionIds: idsOf(
      expressions,
      existing(verySlowNames, 'very slow movement for'),
    ),
  }
}

import { reactive } from 'vue'
import {
  BOOP_EXPRESSION_DEFAULTS,
  DEFAULT_FACE_NAME,
  EXTRA_DEFAULT_NAMES,
  REMOTE_ACTION_DEFAULTS,
  SLOW_MOVEMENT_NAMES,
  VERY_SLOW_MOVEMENT_NAMES,
} from '@/faces/faces'
import type { BoopExpression } from './boop-expressions'
import { remoteActionsFromEntries } from './remote-actions'
import { defaultFaceConfig, defaultFeatureConfig } from './config-defaults'
import { defaultExpressions } from './expressions'

const expressions = defaultExpressions()

export const appStore = reactive({
  example: 'Example state' as string | null,
  faceConfig: defaultFaceConfig(),
  featureConfig: defaultFeatureConfig(),
  expressions,
  /** Id of the expression the head shows when idle (DEFAULT_FACE in the firmware) */
  defaultExpressionId: expressions.find((e) => e.name === DEFAULT_FACE_NAME)!.id,
  /**
   * Ids of the expressions shown now and then in place of the default one
   * (ExtraDefaultExpressions in the firmware). Update in place.
   */
  extraDefaultExpressionIds: expressions
    .filter((e) => EXTRA_DEFAULT_NAMES.includes(e.name))
    .map((e) => e.id),
  /** Expressions shown when booped (BoopExpressions in the firmware). Update in place. */
  boopExpressions: BOOP_EXPRESSION_DEFAULTS.map((b): BoopExpression => ({
    minBoops: b.minBoops,
    expressionId: expressions.find((e) => e.name === b.name)!.id,
  })),
  /**
   * What each remote control button combo does (RemoteControlActions in the firmware).
   * One entry per combo, AA to DD. Update in place.
   */
  remoteActions: remoteActionsFromEntries(REMOTE_ACTION_DEFAULTS, expressions).actions,
  /**
   * Ids of the expressions that move up and down slowly and very slowly
   * (SlowMovementExpressions and VerySlowMovementExpressions in the firmware). Update in place.
   */
  slowMovementExpressionIds: idsOf(expressions, SLOW_MOVEMENT_NAMES),
  verySlowMovementExpressionIds: idsOf(expressions, VERY_SLOW_MOVEMENT_NAMES),
})

/** Ids of the named expressions that exist, in expression order */
export function idsOf(list: { id: number; name: string }[], names: string[]) {
  return list.filter((e) => names.includes(e.name)).map((e) => e.id)
}

/** Names of the expressions with these ids, in expression order, for the generated file */
export function namesOf(ids: number[]) {
  return appStore.expressions.filter((e) => ids.includes(e.id)).map((e) => e.name)
}

/** The expression the head shows when idle, falling back to the first one */
export function getDefaultExpression() {
  return (
    appStore.expressions.find((e) => e.id === appStore.defaultExpressionId) ??
    appStore.expressions[0]!
  )
}

export function isExtraDefault(id: number) {
  return appStore.extraDefaultExpressionIds.includes(id)
}

/**
 * The extra default expressions, in list order. Leaves out the default expression, which
 * the firmware would skip anyway.
 */
export function getExtraDefaultExpressions() {
  const defaultId = getDefaultExpression().id
  return appStore.expressions.filter((e) => e.id !== defaultId && isExtraDefault(e.id))
}

/** An expression by its C++ name (without Face_), falling back to the first one */
export function findExpression(name: string) {
  return appStore.expressions.find((e) => e.name === name) ?? appStore.expressions[0]!
}

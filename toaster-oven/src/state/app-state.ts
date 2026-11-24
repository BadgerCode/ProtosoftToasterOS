import { reactive } from 'vue'
import { BOOP_EXPRESSION_DEFAULTS, DEFAULT_FACE_NAME, EXTRA_DEFAULT_NAMES } from '@/faces/faces'
import type { BoopExpression } from './boop-expressions'
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
})

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

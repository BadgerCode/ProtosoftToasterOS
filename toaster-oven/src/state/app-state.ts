import { reactive } from 'vue'
import { DEFAULT_FACE_NAME } from '@/faces/faces'
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
})

/** The expression the head shows when idle, falling back to the first one */
export function getDefaultExpression() {
  return (
    appStore.expressions.find((e) => e.id === appStore.defaultExpressionId) ??
    appStore.expressions[0]!
  )
}

/** An expression by its C++ name (without Face_), falling back to the first one */
export function findExpression(name: string) {
  return appStore.expressions.find((e) => e.name === name) ?? appStore.expressions[0]!
}

import { reactive } from 'vue'
import { defaultFaceConfig, defaultFeatureConfig } from './config-defaults'
import { defaultExpressions } from './expressions'

export const appStore = reactive({
  example: 'Example state' as string | null,
  faceConfig: defaultFaceConfig(),
  featureConfig: defaultFeatureConfig(),
  expressions: defaultExpressions(),
})

/** An expression by its C++ name (without Face_), falling back to the first one */
export function findExpression(name: string) {
  return appStore.expressions.find((e) => e.name === name) ?? appStore.expressions[0]!
}

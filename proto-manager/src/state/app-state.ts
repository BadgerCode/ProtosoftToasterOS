import { reactive } from 'vue'
import { defaultFaceConfig, defaultFeatureConfig } from './config-defaults'

export const appStore = reactive({
  example: 'Example state' as string | null,
  faceConfig: defaultFaceConfig(),
  featureConfig: defaultFeatureConfig(),
})

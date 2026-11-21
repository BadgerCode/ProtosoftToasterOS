import { computed, ref } from 'vue'
import { appStore } from './app-state'
import { defaultFaceConfig, defaultFeatureConfig } from './config-defaults'
import { downloadTextFile, generateConfigHeader } from './generate-config'

/** The Config.h contents as of the last download. Starts as the ToasterOS defaults. */
const lastDownloadedHeader = ref(generateConfigHeader(defaultFaceConfig(), defaultFeatureConfig()))

export const currentConfigHeader = computed(() =>
  generateConfigHeader(appStore.faceConfig, appStore.featureConfig),
)

/** True when the config has changed since Config.h was last downloaded */
export const hasUndownloadedChanges = computed(
  () => currentConfigHeader.value !== lastDownloadedHeader.value,
)

export function downloadConfigHeader() {
  const header = currentConfigHeader.value
  downloadTextFile('Config.h', header)
  lastDownloadedHeader.value = header
}

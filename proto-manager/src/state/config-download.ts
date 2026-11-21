import { computed, ref } from 'vue'
import { appStore } from './app-state'
import { defaultFaceConfig, defaultFeatureConfig } from './config-defaults'
import { defaultExpressions } from './expressions'
import { downloadBlob, generateConfigHeader } from './generate-config'
import { generateFacesHeader } from './generate-faces'

export type DownloadKind = 'config' | 'expressions'

export const DOWNLOADS: Record<DownloadKind, { fileName: string; label: string }> = {
  config: { fileName: 'Config.h', label: 'config' },
  expressions: { fileName: 'Protogen_Faces.h', label: 'expressions' },
}

const KINDS = Object.keys(DOWNLOADS) as DownloadKind[]

function generateFiles(
  face: typeof appStore.faceConfig,
  features: typeof appStore.featureConfig,
  expressions: typeof appStore.expressions,
): Record<DownloadKind, string> {
  return {
    config: generateConfigHeader(face, features),
    expressions: generateFacesHeader(expressions),
  }
}

/** Each file as of its last download. Starts as the ToasterOS defaults. */
const lastDownloaded = ref(
  generateFiles(defaultFaceConfig(), defaultFeatureConfig(), defaultExpressions()),
)

const currentFiles = computed(() =>
  generateFiles(appStore.faceConfig, appStore.featureConfig, appStore.expressions),
)

/** Files that have changed since they were last downloaded */
export const changedDownloads = computed(() =>
  KINDS.filter((kind) => currentFiles.value[kind] !== lastDownloaded.value[kind]),
)

export const hasUndownloadedChanges = computed(() => changedDownloads.value.length > 0)

export function hasChanged(kind: DownloadKind) {
  return changedDownloads.value.includes(kind)
}

export function downloadFile(kind: DownloadKind) {
  const contents = currentFiles.value[kind]
  downloadBlob(DOWNLOADS[kind].fileName, new Blob([contents], { type: 'text/plain' }))
  lastDownloaded.value = { ...lastDownloaded.value, [kind]: contents }
}

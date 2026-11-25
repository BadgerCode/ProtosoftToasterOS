import { appStore } from '../app-state'
import { hasChanged, markDownloaded, type DownloadKind } from '../config-download'
import { resetEditor } from '../expression-editor'
import { readConfig } from './read-config'
import { readFaces } from './read-faces'

// Imports Config.h and/or Protogen_Faces.h files downloaded from Toaster Oven earlier.
// Each file replaces its part of the app state.

export interface ImportSummary {
  /** Names of the files that were imported */
  imported: string[]
  /** Things the user should know, e.g. settings that fell back to defaults */
  notes: string[]
  errors: string[]
}

/** Works out which file this is from its contents, as it may have been renamed */
export function detectKind(text: string): DownloadKind | null {
  if (text.includes('ProtoConfigStruct') || text.includes('FaceConfig::FaceConfig')) {
    return 'config'
  }
  if (text.includes('struct FaceExpression Face_')) return 'expressions'
  return null
}

type Read =
  | { kind: 'config'; name: string; result: ReturnType<typeof readConfig> }
  | { kind: 'expressions'; name: string; result: ReturnType<typeof readFaces> }

function read(kind: DownloadKind, name: string, text: string): Read {
  return kind === 'config'
    ? { kind, name, result: readConfig(text) }
    : { kind, name, result: readFaces(text) }
}

/** Replace an array's contents in place: the Configure views hold references to them */
function replaceAll<T>(array: T[], items: T[]) {
  array.splice(0, array.length, ...items)
}

function defaultedNote(fileName: string, labels: string[]): string {
  if (labels.length === 1) {
    return `1 setting wasn't in ${fileName}, so it uses the default: ${labels[0]}`
  }
  return `${labels.length} settings weren't in ${fileName}, so they use defaults: ${labels.join(', ')}`
}

/**
 * Reads the files and applies them. `confirmReplace` is asked first if an import would
 * replace changes that haven't been downloaded; returning false cancels the whole import.
 */
export function importFiles(
  files: { name: string; text: string }[],
  confirmReplace: () => boolean,
): ImportSummary {
  const errors: string[] = []
  const reads = new Map<DownloadKind, Read>()

  for (const { name, text } of files) {
    const kind = detectKind(text)
    if (!kind) {
      errors.push(`Couldn't find a config or expressions in ${name}.`)
      continue
    }
    try {
      reads.set(kind, read(kind, name, text)) // A later file of the same kind wins
    } catch (e) {
      errors.push(`Couldn't read ${name}: ${e instanceof Error ? e.message : String(e)}`)
    }
  }

  if ([...reads.keys()].some((kind) => hasChanged(kind)) && !confirmReplace()) {
    return { imported: [], notes: [], errors: [] }
  }

  const imported: string[] = []
  const notes: string[] = []

  for (const r of reads.values()) {
    imported.push(r.name)
    let upToDate: boolean

    // Update in place: the Configure views hold references to these objects
    if (r.kind === 'config') {
      appStore.faceConfig.connections = r.result.faceConfig.connections
      Object.assign(appStore.featureConfig, r.result.featureConfig)
      if (r.result.defaulted.length) notes.push(defaultedNote(r.name, r.result.defaulted))
      upToDate = r.result.defaulted.length === 0
    } else {
      const result = r.result
      replaceAll(appStore.expressions, result.expressions)
      appStore.defaultExpressionId = result.defaultExpressionId
      replaceAll(appStore.extraDefaultExpressionIds, result.extraDefaultExpressionIds)
      replaceAll(appStore.boopExpressions, result.boopExpressions)
      replaceAll(appStore.remoteActions, result.remoteActions)
      replaceAll(appStore.slowMovementExpressionIds, result.slowMovementExpressionIds)
      replaceAll(appStore.verySlowMovementExpressionIds, result.verySlowMovementExpressionIds)
      resetEditor()

      if (result.added.length) {
        notes.push(`Added missing expressions the firmware needs: ${result.added.join(', ')}`)
      }
      if (result.skipped.length) {
        notes.push(
          `Skipped settings in ${r.name} that couldn't be used: ${result.skipped.join(', ')}`,
        )
      }
      upToDate = result.added.length === 0 && result.skipped.length === 0
    }

    // The user already has this file, unless something had to be filled in
    if (upToDate) markDownloaded(r.kind)
  }

  return { imported, notes, errors }
}

import { DEFAULT_FACE_NAME } from '@/faces/faces'
import { appStore } from '../app-state'
import { hasChanged, markDownloaded, type DownloadKind } from '../config-download'
import { resetEditor } from '../expression-editor'
import { displayName } from '../expressions'
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
      appStore.expressions.splice(0, appStore.expressions.length, ...r.result.expressions)
      appStore.defaultExpressionId = r.result.defaultExpressionId
      appStore.extraDefaultExpressionIds.splice(
        0,
        appStore.extraDefaultExpressionIds.length,
        ...r.result.extraDefaultExpressionIds,
      )
      appStore.boopExpressions.splice(
        0,
        appStore.boopExpressions.length,
        ...r.result.boopExpressions,
      )
      appStore.remoteActions.splice(0, appStore.remoteActions.length, ...r.result.remoteActions)
      appStore.slowMovementExpressionIds.splice(
        0,
        appStore.slowMovementExpressionIds.length,
        ...r.result.slowMovementExpressionIds,
      )
      appStore.verySlowMovementExpressionIds.splice(
        0,
        appStore.verySlowMovementExpressionIds.length,
        ...r.result.verySlowMovementExpressionIds,
      )
      resetEditor()
      if (r.result.added.length) {
        notes.push(`Added missing expressions the firmware needs: ${r.result.added.join(', ')}`)
      }
      const fallback = r.result.defaultFallback
      if (fallback) {
        const used = displayName(DEFAULT_FACE_NAME)
        notes.push(
          fallback.requested
            ? `The default expression, ${displayName(fallback.requested)}, wasn't in ${r.name}, so it uses ${used}`
            : `${r.name} didn't set a default expression, so it uses ${used}`,
        )
      }
      if (r.result.extraDefaultsFallback) {
        notes.push(`${r.name} didn't list extra default expressions, so it uses the usual ones`)
      }
      if (r.result.extraDefaultsSkipped.length) {
        notes.push(
          `Skipped extra default expressions that weren't in ${r.name}: ${r.result.extraDefaultsSkipped.join(', ')}`,
        )
      }
      if (r.result.boopExpressionsFallback) {
        notes.push(`${r.name} didn't list boop expressions, so it uses the usual ones`)
      }
      if (r.result.boopExpressionsSkipped.length) {
        notes.push(
          `Skipped boop expressions that weren't in ${r.name}: ${r.result.boopExpressionsSkipped.join(', ')}`,
        )
      }
      if (r.result.remoteActionsFallback) {
        notes.push(`${r.name} didn't list remote control combos, so it uses the usual ones`)
      }
      if (r.result.remoteActionsSkipped.length) {
        notes.push(
          `Skipped remote control combos that couldn't be used: ${r.result.remoteActionsSkipped.join(', ')}`,
        )
      }
      if (r.result.movementFallback) {
        notes.push(`${r.name} didn't list movement speeds, so it uses the usual ones`)
      }
      if (r.result.movementSkipped.length) {
        notes.push(
          `Skipped movement speeds for expressions that weren't in ${r.name}: ${r.result.movementSkipped.join(', ')}`,
        )
      }
      upToDate =
        !r.result.movementFallback &&
        r.result.movementSkipped.length === 0 &&
        !r.result.remoteActionsFallback &&
        r.result.remoteActionsSkipped.length === 0 &&
        !r.result.boopExpressionsFallback &&
        r.result.boopExpressionsSkipped.length === 0 &&
        r.result.added.length === 0 &&
        !fallback &&
        !r.result.extraDefaultsFallback &&
        r.result.extraDefaultsSkipped.length === 0
    }

    // The user already has this file, unless something had to be filled in
    if (upToDate) markDownloaded(r.kind)
  }

  return { imported, notes, errors }
}

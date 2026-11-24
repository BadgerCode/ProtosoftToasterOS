import { DEFAULT_FACE_NAME, defaultExpression } from '@/faces/faces'
import { parseDefaultFace, parseExpressions } from '@/faces/parse-faces'
import { REQUIRED_EXPRESSIONS } from '@/faces/required-expressions'
import { createExpression, displayName, type Expression } from '../expressions'

// Reads a Protogen_Faces.h that Toaster Oven downloaded earlier.
// Expressions the firmware needs but the file doesn't have are added back from the defaults,
// so the generated file still compiles.

export interface ReadFacesResult {
  expressions: Expression[]
  /** Display names of the expressions that were added back */
  added: string[]
  /** Id of the expression to use as the default (DEFAULT_FACE) */
  defaultExpressionId: number
  /**
   * Set when the file didn't have a usable default expression and it fell back to
   * DEFAULT_FACE_NAME, e.g. files downloaded before the default could be chosen.
   * The name the file asked for, if it had one.
   */
  defaultFallback: { requested: string | null } | null
}

export function readFaces(text: string): ReadFacesResult {
  const parsed = parseExpressions(text)
  if (parsed.length === 0) throw new Error('No expressions found')

  const expressions = parsed.map((e) => createExpression(e.name, e))
  const names = new Set(parsed.map((e) => e.name))
  const added: string[] = []

  function addBack(name: string) {
    expressions.push(createExpression(name, defaultExpression(name)))
    names.add(name)
    added.push(displayName(name))
  }

  for (const name of Object.keys(REQUIRED_EXPRESSIONS)) {
    if (!names.has(name)) addBack(name)
  }

  // Older files have no DEFAULT_FACE; the firmware used Face_Neutral back then
  const requested = parseDefaultFace(text)
  let defaultName = requested
  let defaultFallback: ReadFacesResult['defaultFallback'] = null
  if (!defaultName || !names.has(defaultName)) {
    defaultName = DEFAULT_FACE_NAME
    defaultFallback = { requested }
    if (!names.has(defaultName)) addBack(defaultName)
  }

  const defaultExpressionId = expressions.find((e) => e.name === defaultName)!.id
  return { expressions, added, defaultExpressionId, defaultFallback }
}

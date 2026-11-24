import { defaultExpression } from '@/faces/faces'
import { parseExpressions } from '@/faces/parse-faces'
import { REQUIRED_EXPRESSIONS } from '@/faces/required-expressions'
import { createExpression, displayName, type Expression } from '../expressions'

// Reads a Protogen_Faces.h that Toaster Oven downloaded earlier.
// Expressions the firmware needs but the file doesn't have are added back from the defaults,
// so the generated file still compiles.

export function readFaces(text: string): { expressions: Expression[]; added: string[] } {
  const parsed = parseExpressions(text)
  if (parsed.length === 0) throw new Error('No expressions found')

  const expressions = parsed.map((e) => createExpression(e.name, e))
  const names = new Set(parsed.map((e) => e.name))
  const added: string[] = []

  for (const name of Object.keys(REQUIRED_EXPRESSIONS)) {
    if (names.has(name)) continue
    expressions.push(createExpression(name, defaultExpression(name)))
    added.push(displayName(name))
  }

  return { expressions, added }
}

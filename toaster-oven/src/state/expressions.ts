import { DEFAULT_EXPRESSIONS, emptyPanel, type FaceExpressionData } from '@/faces/faces'

export interface Expression extends FaceExpressionData {
  id: number
  /** C++ name without the Face_ prefix, e.g. Happy_Closed_eyes */
  name: string
}

let nextId = 1

// JSON rather than structuredClone: the expressions are Vue proxies, which can't be structured-cloned
function clone(face: FaceExpressionData): FaceExpressionData {
  return JSON.parse(
    JSON.stringify({
      nose: face.nose,
      eye: face.eye,
      eyeBlink: face.eyeBlink,
      mouth: face.mouth,
      animation: face.animation,
    }),
  )
}

export function defaultExpressions(): Expression[] {
  return DEFAULT_EXPRESSIONS.map((e) => ({ id: nextId++, name: e.name, ...clone(e) }))
}

export function createExpression(name: string, from?: FaceExpressionData): Expression {
  const face = from
    ? clone(from)
    : {
        nose: emptyPanel(),
        eye: [emptyPanel(), emptyPanel()] as [number[], number[]],
        eyeBlink: null,
        mouth: [emptyPanel(), emptyPanel(), emptyPanel(), emptyPanel()] as [
          number[],
          number[],
          number[],
          number[],
        ],
        animation: null,
      }
  return { id: nextId++, name, ...face }
}

/** "Happy_Closed_eyes" -> "Happy Closed eyes" */
export function displayName(name: string): string {
  return name.replaceAll('_', ' ')
}

/** "Happy Closed eyes" -> "Happy_Closed_eyes" */
export function nameFromDisplay(display: string): string {
  return display.trim().replace(/\s+/g, '_')
}

/** An error message if the name can't be used, otherwise null */
export function validateName(name: string, expressions: Expression[], selfId?: number) {
  if (!name) return 'Give the expression a name.'
  if (!/^[A-Za-z][A-Za-z0-9_]*$/.test(name)) {
    return 'Use letters, numbers and spaces, starting with a letter.'
  }
  const taken = expressions.some(
    (e) => e.id !== selfId && e.name.toLowerCase() === name.toLowerCase(),
  )
  if (taken) return 'Another expression already has this name.'
  return null
}

/** "New expression", "New expression 2", ... */
export function uniqueName(base: string, expressions: Expression[]): string {
  const names = new Set(expressions.map((e) => e.name.toLowerCase()))
  if (!names.has(base.toLowerCase())) return base
  for (let i = 2; ; i++) {
    const candidate = `${base}_${i}`
    if (!names.has(candidate.toLowerCase())) return candidate
  }
}

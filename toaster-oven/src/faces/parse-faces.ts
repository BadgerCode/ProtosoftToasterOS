import type { FaceExpressionData, PanelBitmap } from './faces'
import { normaliseFirstFrame, type AnimationFrame } from './animation'

// Reads expressions out of the firmware's Protogen_Faces.h, e.g.
//
// struct FaceExpression Face_Neutral = {
//   .Nose = { { B00110000, ... }  // },
//   .Eye = { {...}, {...} },
//   .HasBlink = true,
//   .Eye_Blink = { {...}, {...} },
//   .Mouth = { {...}, {...}, {...}, {...} },
//   .Animation = &Neutral_Animation  // Optional, see parseAnimation()
// };

export interface ParsedExpression extends FaceExpressionData {
  /** Name without the Face_ prefix, e.g. Happy_Closed_eyes */
  name: string
}

const EXPRESSION_PATTERN = /struct FaceExpression Face_(\w+)\s*=\s*\{([\s\S]*?)\n\};/g
const SECTION_PATTERN = /\.(Nose|Eye_Blink|Eye|Mouth)\s*=/g
const PANEL_PATTERN = /\{\s*((?:B[01]{8}\s*,?\s*){8})\}/g

export function parsePanel(text: string): PanelBitmap {
  const rows = [...text.matchAll(/B([01]{8})/g)].map((m) => parseInt(m[1]!, 2))
  if (rows.length !== 8) throw new Error(`Expected 8 rows in panel, found ${rows.length}`)
  return rows
}

function parseExpressionBody(name: string, body: string): ParsedExpression {
  const sections = [...body.matchAll(SECTION_PATTERN)].map((m) => ({
    key: m[1]!,
    index: m.index!,
  }))

  const panels: Record<string, PanelBitmap[]> = { Nose: [], Eye: [], Eye_Blink: [], Mouth: [] }
  for (const match of body.matchAll(PANEL_PATTERN)) {
    const section = sections.filter((s) => s.index < match.index!).at(-1)
    if (!section) continue
    panels[section.key]!.push(parsePanel(match[1]!))
  }

  const hasBlink = /\.HasBlink\s*=\s*true/.test(body)
  const { Nose: nose, Eye: eye, Eye_Blink: eyeBlink, Mouth: mouth } = panels
  if (nose!.length !== 1 || eye!.length !== 2 || mouth!.length !== 4) {
    throw new Error(`Face_${name} doesn't have 1 nose, 2 eye and 4 mouth panels`)
  }

  return {
    name,
    nose: nose![0]!,
    eye: eye as [PanelBitmap, PanelBitmap],
    eyeBlink: hasBlink && eyeBlink!.length === 2 ? (eyeBlink as [PanelBitmap, PanelBitmap]) : null,
    mouth: mouth as [PanelBitmap, PanelBitmap, PanelBitmap, PanelBitmap],
    animation: null,
  }
}

/** The panels in a `const EyeFrame NAME = {...};` or `const byte NAME[n][8] = {...};` */
function parseNamedPanels(header: string, name: string, count: number): PanelBitmap[] {
  const match = header.match(
    new RegExp(`const\\s+(?:EyeFrame|byte)\\s+${name}\\b[^=]*=\\s*(\\{[\\s\\S]*?\\});`),
  )
  if (!match) throw new Error(`Couldn't find ${name}`)
  const panels = [...match[1]!.matchAll(PANEL_PATTERN)].map((m) => parsePanel(m[1]!))
  if (panels.length !== count) throw new Error(`Expected ${count} panels in ${name}`)
  return panels
}

/**
 * An expression's animation, from
 *   const AnimationFrame X_Frames[] = { { 200 }, { 200, &X_Frame2_Eye, X_Frame2_Mouth, NULL } };
 *   const FaceAnimation X_Animation = ANIMATION(X_Frames);
 * Each frame is a duration, then the eyes, mouth and nose it replaces (NULL or left out: none).
 */
function parseAnimation(header: string, name: string, animationName: string): AnimationFrame[] {
  const animation = header.match(
    new RegExp(
      `const\\s+FaceAnimation\\s+${animationName}\\s*=\\s*(?:ANIMATION\\(\\s*(\\w+)\\s*\\)|\\{\\s*(\\w+)\\s*,)`,
    ),
  )
  if (!animation) throw new Error(`Couldn't find ${animationName} for Face_${name}`)
  const framesName = animation[1] ?? animation[2]!
  const list = header.match(
    new RegExp(`const\\s+AnimationFrame\\s+${framesName}\\[\\]\\s*=\\s*\\{([\\s\\S]*?)\\};`),
  )?.[1]
  if (list === undefined) throw new Error(`Couldn't find ${framesName} for Face_${name}`)

  const reference = (value: string | undefined) => {
    const ref = value?.trim().replace(/^&/, '')
    return !ref || ref === 'NULL' || ref === 'nullptr' || ref === '0' ? null : ref
  }
  const frames = [...list.matchAll(/\{\s*(\d+)\s*((?:,\s*&?\w+\s*)*)\}/g)].map((m) => {
    const [eye, mouth, nose] = m[2]!.split(',').slice(1).map(reference)
    return {
      durationMs: parseInt(m[1]!, 10),
      eye: eye ? (parseNamedPanels(header, eye, 2) as AnimationFrame['eye']) : null,
      mouth: mouth ? (parseNamedPanels(header, mouth, 4) as AnimationFrame['mouth']) : null,
      nose: nose ? parseNamedPanels(header, nose, 1)[0]! : null,
    }
  })
  if (frames.length === 0) throw new Error(`${framesName} has no frames`)
  return frames
}

export function parseExpressions(header: string): ParsedExpression[] {
  return [...header.matchAll(EXPRESSION_PATTERN)].map((m) => {
    const expression = parseExpressionBody(m[1]!, m[2]!)
    const animationName = m[2]!.match(/\.Animation\s*=\s*&(\w+)/)?.[1]
    if (animationName) {
      expression.animation = parseAnimation(header, expression.name, animationName)
      normaliseFirstFrame(expression)
    }
    return expression
  })
}

/** The expression DEFAULT_FACE points at, without the Face_ prefix, or null if there isn't one */
export function parseDefaultFace(header: string): string | null {
  return header.match(/DEFAULT_FACE\s*=\s*&Face_(\w+)\s*;/)?.[1] ?? null
}

/**
 * The expressions in a `FaceExpression* const <listName>[] = { &Face_X, ... };` list, without
 * the Face_ prefix, or null if the list isn't there. An empty list ({}) gives [].
 */
export function parseExpressionList(header: string, listName: string): string[] | null {
  const list = header.match(new RegExp(`\\b${listName}\\[\\]\\s*=\\s*\\{([^}]*)\\}`))?.[1]
  if (list === undefined) return null
  return [...list.matchAll(/&Face_(\w+)/g)].map((m) => m[1]!)
}

/**
 * The entries in BoopExpressions, e.g. `{ 3, &Face_Angry }`, or null if the list isn't there.
 * An empty list ({}) gives [].
 */
export function parseBoopExpressions(header: string): { minBoops: number; name: string }[] | null {
  const list = header.match(/BoopExpressions\[\]\s*=\s*\{([\s\S]*?)\};/)?.[1]
  if (list === undefined) return null
  return [...list.matchAll(/\{\s*(\d+)\s*,\s*&Face_(\w+)\s*\}/g)].map((m) => ({
    minBoops: parseInt(m[1]!, 10),
    name: m[2]!,
  }))
}

/**
 * The entries in RemoteControlActions, e.g. `{ "AB", REMOTE_EXPRESSION, &Face_Blep }`, or null
 * if the list isn't there. `name` is the expression, without the Face_ prefix. An empty list gives [].
 */
export function parseRemoteActions(
  header: string,
): { buttons: string; action: string; name: string | null }[] | null {
  const list = header.match(/RemoteControlActions\[\]\s*=\s*\{([\s\S]*?)\};/)?.[1]
  if (list === undefined) return null
  return [...list.matchAll(/\{\s*"([^"]*)"\s*,\s*(\w+)\s*(?:,\s*&Face_(\w+)\s*)?\}/g)].map((m) => ({
    buttons: m[1]!,
    action: m[2]!,
    name: m[3] ?? null,
  }))
}

/** A top-level declaration copied as-is, e.g. `byte Face_Nose_Single[8] = { ... };` */
export function extractDeclaration(header: string, startsWith: string): string {
  const start = header.indexOf(startsWith)
  if (start === -1) throw new Error(`Couldn't find "${startsWith}" in Protogen_Faces.h`)
  const end = header.indexOf('};', start)
  return header.slice(start, end + 2)
}

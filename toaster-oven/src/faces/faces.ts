// Face bitmaps, read from a copy of ToasterOS/Protogen_Faces.h (src/assets/Protogen_Faces.h).
// Like the firmware, every face is drawn for the right side of the face
// (as seen from the front) and mirrored for the left side.
// Each panel is 8 rows; the most significant bit is the leftmost LED.

import facesHeader from '@/assets/Protogen_Faces.h?raw'
import { PanelType } from '@/state/config-types'
import { extractDeclaration, parseDefaultFace, parseExpressions, parsePanel } from './parse-faces'

export type PanelBitmap = number[]

export interface FaceExpressionData {
  nose: PanelBitmap
  /** [front, back] */
  eye: [PanelBitmap, PanelBitmap]
  /** Eyes while blinking [front, back], or null if this expression doesn't blink */
  eyeBlink: [PanelBitmap, PanelBitmap] | null
  /** [front, mid-front, mid-back, back] */
  mouth: [PanelBitmap, PanelBitmap, PanelBitmap, PanelBitmap]
}

/** The starting pack of expressions that ships with ToasterOS */
export const DEFAULT_EXPRESSIONS = parseExpressions(facesHeader)

/** The expression the head shows when idle (DEFAULT_FACE), without the Face_ prefix */
export const DEFAULT_FACE_NAME = parseDefaultFace(facesHeader) ?? 'Neutral'

/** Firmware declarations that aren't expressions, copied into the generated file as-is */
export const NOSE_SINGLE_DECLARATION = extractDeclaration(facesHeader, 'byte Face_Nose_Single')

export const NOSE_SINGLE: PanelBitmap = parsePanel(NOSE_SINGLE_DECLARATION)

export function defaultExpression(name: string): FaceExpressionData {
  const face = DEFAULT_EXPRESSIONS.find((e) => e.name === name)
  if (!face) throw new Error(`No default expression called Face_${name}`)
  return face
}

export function emptyPanel(): PanelBitmap {
  return [0, 0, 0, 0, 0, 0, 0, 0]
}

export function mirror(bitmap: PanelBitmap): PanelBitmap {
  return bitmap.map((row) => {
    let out = 0
    for (let bit = 0; bit < 8; bit++) if (row & (1 << bit)) out |= 1 << (7 - bit)
    return out
  })
}

/** What each panel shows for an expression, matching ExpressionManager::Render() */
export function expressionToPanels(
  face: FaceExpressionData,
  blinking = false,
): Record<number, PanelBitmap> {
  const eyes = blinking && face.eyeBlink ? face.eyeBlink : face.eye
  return {
    [PanelType.RIGHT_MOUTH_FRONT]: face.mouth[0],
    [PanelType.RIGHT_MOUTH_MID_FRONT]: face.mouth[1],
    [PanelType.RIGHT_MOUTH_MID_BACK]: face.mouth[2],
    [PanelType.RIGHT_MOUTH_BACK]: face.mouth[3],
    [PanelType.LEFT_MOUTH_FRONT]: mirror(face.mouth[0]),
    [PanelType.LEFT_MOUTH_MID_FRONT]: mirror(face.mouth[1]),
    [PanelType.LEFT_MOUTH_MID_BACK]: mirror(face.mouth[2]),
    [PanelType.LEFT_MOUTH_BACK]: mirror(face.mouth[3]),
    [PanelType.RIGHT_NOSE]: face.nose,
    [PanelType.LEFT_NOSE]: mirror(face.nose),
    [PanelType.SINGLE_NOSE]: NOSE_SINGLE,
    [PanelType.RIGHT_EYE_FRONT]: eyes[0],
    [PanelType.RIGHT_EYE_BACK]: eyes[1],
    [PanelType.LEFT_EYE_FRONT]: mirror(eyes[0]),
    [PanelType.LEFT_EYE_BACK]: mirror(eyes[1]),
  }
}

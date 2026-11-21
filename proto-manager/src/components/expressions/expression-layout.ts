import type { FaceExpressionData, PanelBitmap } from '@/faces/faces'

// The right side of the face (as seen from the front), which is what expressions are drawn for.
// Positions match VisorDisplay, in LEDs.

export interface EditablePanel {
  key: string
  label: string
  x: number
  y: number
  bitmap: (face: FaceExpressionData, blink: boolean) => PanelBitmap
}

const eyes = (face: FaceExpressionData, blink: boolean) =>
  blink && face.eyeBlink ? face.eyeBlink : face.eye

export const EDITABLE_PANELS: EditablePanel[] = [
  { key: 'nose', label: 'Nose', x: 0, y: 7, bitmap: (f) => f.nose },
  { key: 'eye-front', label: 'Eye front', x: 23, y: 0, bitmap: (f, b) => eyes(f, b)[0] },
  { key: 'eye-back', label: 'Eye back', x: 31, y: 0, bitmap: (f, b) => eyes(f, b)[1] },
  { key: 'mouth-front', label: 'Mouth front', x: 5, y: 18, bitmap: (f) => f.mouth[0] },
  { key: 'mouth-mid-front', label: 'Mid-front', x: 13, y: 18, bitmap: (f) => f.mouth[1] },
  { key: 'mouth-mid-back', label: 'Mid-back', x: 21, y: 18, bitmap: (f) => f.mouth[2] },
  { key: 'mouth-back', label: 'Mouth back', x: 29, y: 18, bitmap: (f) => f.mouth[3] },
]

export const LAYOUT_WIDTH = 39
export const LAYOUT_HEIGHT = 26

export function isLit(bitmap: PanelBitmap, row: number, col: number) {
  return !!(bitmap[row]! & (0x80 >> col))
}

export function setLed(bitmap: PanelBitmap, row: number, col: number, on: boolean) {
  const mask = 0x80 >> col
  bitmap[row] = on ? bitmap[row]! | mask : bitmap[row]! & ~mask
}

/** The panel and LED under a point in layout coordinates, if any */
export function ledAt(x: number, y: number) {
  for (const panel of EDITABLE_PANELS) {
    const col = Math.floor(x - panel.x)
    const row = Math.floor(y - panel.y)
    if (col >= 0 && col < 8 && row >= 0 && row < 8) return { panel, row, col }
  }
  return null
}

// Face bitmaps ported from ToasterOS/Protogen_Faces.h.
// Like the firmware, every face is drawn for the right side of the face
// (as seen from the front) and mirrored for the left side.
// Each panel is 8 rows; the most significant bit is the leftmost LED.

import { PanelType } from '@/state/config-types'

export type PanelBitmap = number[]

export interface FaceExpression {
  nose: PanelBitmap
  /** [front, back] */
  eye: [PanelBitmap, PanelBitmap]
  eyeBlink?: [PanelBitmap, PanelBitmap]
  /** [front, mid-front, mid-back, back] */
  mouth: [PanelBitmap, PanelBitmap, PanelBitmap, PanelBitmap]
}

const NOSE = [
  0b00110000, 0b01110000, 0b11100000, 0b11000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
]

const EMPTY = [0, 0, 0, 0, 0, 0, 0, 0]

export const NOSE_SINGLE: PanelBitmap = [
  0b00000000, 0b00000000, 0b01111110, 0b00111100, 0b00011000, 0b00000000, 0b00000000, 0b00000000,
]

export const FACE_NEUTRAL: FaceExpression = {
  nose: NOSE,
  eye: [
    [
      0b00000000, 0b00000011, 0b00001111, 0b00111111, 0b01111111, 0b11111111, 0b11111110,
      0b11100000,
    ],
    [
      0b00000000, 0b11100000, 0b11111000, 0b11111100, 0b11111110, 0b11111111, 0b00000000,
      0b00000000,
    ],
  ],
  eyeBlink: [
    [
      0b00000000, 0b00000000, 0b00000000, 0b00000111, 0b00001111, 0b00111111, 0b11100000,
      0b00000000,
    ],
    [
      0b00000000, 0b00000000, 0b00000000, 0b11000000, 0b11100000, 0b11111111, 0b00000000,
      0b00000000,
    ],
  ],
  mouth: [
    EMPTY,
    [
      0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b01110001, 0b00011111, 0b00000011,
      0b00000000,
    ],
    [
      0b00000000, 0b00000000, 0b00000000, 0b00100000, 0b11111111, 0b11100111, 0b10000000,
      0b00000000,
    ],
    [
      0b00000000, 0b00000000, 0b00000111, 0b01111100, 0b11110000, 0b11000000, 0b00000000,
      0b00000000,
    ],
  ],
}

export const FACE_SURPRISED: FaceExpression = {
  nose: NOSE,
  eye: [
    [
      0b00000001, 0b00000011, 0b00000110, 0b00001100, 0b00001100, 0b00000110, 0b00000011,
      0b00000001,
    ],
    [
      0b10000000, 0b11000000, 0b01100000, 0b00110000, 0b00110000, 0b01100000, 0b11000000,
      0b10000000,
    ],
  ],
  mouth: [
    EMPTY,
    [
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000011, 0b01111111, 0b00000011,
      0b00000000,
    ],
    [
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111111, 0b11100111, 0b10000000,
      0b00000000,
    ],
    [
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11110000, 0b11000000, 0b00000000,
      0b00000000,
    ],
  ],
}

export const FACE_ANGRY: FaceExpression = {
  nose: NOSE,
  eye: [
    [
      0b00000001, 0b00000011, 0b00000111, 0b00001111, 0b00011111, 0b00111111, 0b00111100,
      0b00011000,
    ],
    [
      0b00000000, 0b10000000, 0b10000000, 0b10000000, 0b10000000, 0b00000000, 0b00000000,
      0b00000000,
    ],
  ],
  mouth: [
    [
      0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00000011, 0b00000110, 0b00001100,
      0b00000000,
    ],
    [
      0b00000000, 0b00111000, 0b01111100, 0b11000110, 0b10000011, 0b00000001, 0b00000000,
      0b00000000,
    ],
    [
      0b00000000, 0b00000000, 0b00000000, 0b00001110, 0b00011111, 0b10110011, 0b11100001,
      0b00000000,
    ],
    [
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b10000000, 0b11000000,
      0b00000000,
    ],
  ],
}

export const FACE_X_X: FaceExpression = {
  nose: NOSE,
  eye: [
    [
      0b00000000, 0b00001100, 0b00000110, 0b00000011, 0b00000001, 0b00000011, 0b00000110,
      0b00001100,
    ],
    [
      0b00000000, 0b00110000, 0b01100000, 0b11000000, 0b10000000, 0b11000000, 0b01100000,
      0b00110000,
    ],
  ],
  mouth: [
    EMPTY,
    [
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01110101, 0b00011111, 0b00000000,
      0b00000000,
    ],
    [
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111111, 0b11100111, 0b00000000,
      0b00000000,
    ],
    [
      0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11110000, 0b11000000, 0b00000000,
      0b00000000,
    ],
  ],
}

function mirror(bitmap: PanelBitmap): PanelBitmap {
  return bitmap.map((row) => {
    let out = 0
    for (let bit = 0; bit < 8; bit++) if (row & (1 << bit)) out |= 1 << (7 - bit)
    return out
  })
}

/** What each panel shows for an expression, matching ExpressionManager::Render() */
export function expressionToPanels(
  face: FaceExpression,
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

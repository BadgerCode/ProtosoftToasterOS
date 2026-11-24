import { NOSE_SINGLE_DECLARATION, type PanelBitmap } from '@/faces/faces'
import type { Expression } from './expressions'

// Generates ToasterOS/Protogen_Faces.h from the app's expressions.
// Keep the layout in sync with the Protogen_Faces.h in the ToasterOS project.

function panelRow(panel: PanelBitmap): string {
  return panel.map((row) => `B${row.toString(2).padStart(8, '0')}`).join(', ')
}

/** Panels inside a section, each ending in `//` like the hand-written file */
function panels(list: PanelBitmap[]): string {
  return list
    .map((panel, i) => {
      const comma = i < list.length - 1 ? ',' : ' '
      return `    { ${panelRow(panel)} }${comma}  //`
    })
    .join('\n')
}

function generateExpression(e: Expression): string {
  const blink = e.eyeBlink
    ? `{
${panels(e.eyeBlink)}
  }`
    : '{}'

  return `struct FaceExpression Face_${e.name} = {
  .Nose = {
${panels([e.nose])}
  },
  .Eye = {
${panels(e.eye)}
  },
  .HasBlink = ${e.eyeBlink !== null},
  .Eye_Blink = ${blink},
  .Mouth = {
${panels(e.mouth)}
  }
};`
}

export function generateFacesHeader(expressions: Expression[]): string {
  return `// All of the faces and digits are designed to work on the right face & be flipped on the left face


typedef byte EyeFrame[2][8];

struct FaceExpression {
  byte Nose[1][8];
  EyeFrame Eye;
  bool HasBlink;
  EyeFrame Eye_Blink;
  byte Mouth[4][8];
};


${NOSE_SINGLE_DECLARATION}


// Expressions
${expressions.map(generateExpression).join('\n\n')}
`
}

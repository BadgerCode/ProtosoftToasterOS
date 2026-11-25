import { NOSE_SINGLE_DECLARATION, type PanelBitmap } from '@/faces/faces'
import { storableDuration, type AnimationFrame } from '@/faces/animation'
import type { Expression } from './expressions'

/** A RemoteControlActions entry, by expression name */
export interface RemoteActionEntry {
  buttons: string
  action: string
  name: string | null
}

/** A BoopExpressions entry, by expression name */
export interface BoopExpressionEntry {
  minBoops: number
  name: string
}

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

/** `const EyeFrame X = {...};` or `const byte X[4][8] = {...};` for a part a frame replaces */
function partDeclaration(type: 'EyeFrame' | 'byte', name: string, list: PanelBitmap[]): string {
  const declaration =
    type === 'EyeFrame' ? `const EyeFrame ${name}` : `const byte ${name}[${list.length}][8]`
  return `${declaration} = {\n${list.map((p) => `  { ${panelRow(p)} }`).join(',\n')}\n};`
}

/** The frames and parts of an animated expression, written just before it */
function generateAnimation(e: Expression, frames: AnimationFrame[]): string {
  const declarations: string[] = []
  const entries = frames.map((frame, i) => {
    const prefix = `${e.name}_Frame${i + 1}`
    const refs: string[] = []
    if (frame.eye) {
      declarations.push(partDeclaration('EyeFrame', `${prefix}_Eye`, frame.eye))
      refs[0] = `&${prefix}_Eye`
    }
    if (frame.mouth) {
      declarations.push(partDeclaration('byte', `${prefix}_Mouth`, frame.mouth))
      refs[1] = `${prefix}_Mouth`
    }
    if (frame.nose) {
      declarations.push(partDeclaration('byte', `${prefix}_Nose`, [frame.nose]))
      refs[2] = `${prefix}_Nose`
    }
    const parts = Array.from({ length: refs.length }, (_, p) => refs[p] ?? 'NULL')
    return `  { ${[storableDuration(frame.durationMs), ...parts].join(', ')} }`
  })

  return `// ${e.name} animation
${declarations.map((d) => d + '\n').join('\n')}
const AnimationFrame ${e.name}_Frames[] = {
${entries.join(',\n')}
};
const FaceAnimation ${e.name}_Animation = ANIMATION(${e.name}_Frames);

`
}

function generateExpression(e: Expression): string {
  const blink = e.eyeBlink
    ? `{
${panels(e.eyeBlink)}
  }`
    : '{}'

  const animated = e.animation !== null && e.animation.length > 1
  const animation = animated ? generateAnimation(e, e.animation!) : ''

  return `${animation}struct FaceExpression Face_${e.name} = {
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
  }${animated ? `,\n  .Animation = &${e.name}_Animation` : ''}
};`
}

/** A `FaceExpression* const <listName>[]` list and its Num<listName> count */
function generateExpressionList(listName: string, names: string[]): string {
  const list = names.length ? `{\n${names.map((n) => `  &Face_${n}`).join(',\n')}\n}` : '{}'
  return `FaceExpression* const ${listName}[] = ${list};
const int Num${listName} = sizeof(${listName}) / sizeof(${listName}[0]);`
}

function generateBoopExpressions(entries: BoopExpressionEntry[]): string {
  const list = entries.length
    ? `{\n${entries.map((b) => `  { ${b.minBoops}, &Face_${b.name} }`).join(',\n')}\n}`
    : '{}'
  return `const BoopExpressionConfig BoopExpressions[] = ${list};
const int NumBoopExpressions = sizeof(BoopExpressions) / sizeof(BoopExpressions[0]);`
}

function generateRemoteActions(entries: RemoteActionEntry[]): string {
  const entry = (a: RemoteActionEntry) =>
    a.name
      ? `  { "${a.buttons}", ${a.action}, &Face_${a.name} }`
      : `  { "${a.buttons}", ${a.action} }`
  const list = entries.length ? `{\n${entries.map(entry).join(',\n')}\n}` : '{}'
  return `const RemoteControlActionConfig RemoteControlActions[] = ${list};
const int NumRemoteControlActions = sizeof(RemoteControlActions) / sizeof(RemoteControlActions[0]);`
}

export function generateFacesHeader(
  expressions: Expression[],
  defaultName: string,
  extraDefaultNames: string[],
  boopExpressions: BoopExpressionEntry[],
  remoteActions: RemoteActionEntry[],
  movement: { slow: string[]; verySlow: string[] },
): string {
  return `// All of the faces and digits are designed to work on the right face & be flipped on the left face


typedef byte EyeFrame[2][8];

struct FaceAnimation;

struct FaceExpression {
  byte Nose[1][8];
  EyeFrame Eye;
  bool HasBlink;
  EyeFrame Eye_Blink;
  byte Mouth[4][8];
  const FaceAnimation* Animation;  // NULL for expressions that don't animate
};

// One frame of an animated expression. Parts that are left out (NULL) use the expression's own
struct AnimationFrame {
  unsigned int DurationMs;
  const EyeFrame* Eye;
  const byte (*Mouth)[8];  // 4 panels
  const byte (*Nose)[8];   // 1 panel
};

// An animated expression loops through its frames, and doesn't blink
struct FaceAnimation {
  const AnimationFrame* Frames;
  int NumFrames;
};
#define ANIMATION(frames) { frames, sizeof(frames) / sizeof(frames[0]) }


${NOSE_SINGLE_DECLARATION}


// Expressions
${expressions.map(generateExpression).join('\n\n')}


// Face usage configuration
FaceExpression* const DEFAULT_FACE = &Face_${defaultName};

// Additional Neutral/Happy expressions to randomly change to when the default expression is in use
// Leave the list empty ({}) to always show the default expression
${generateExpressionList('ExtraDefaultExpressions', extraDefaultNames)}

// Expressions to show when booped, by the minimum number of boops in a row
// The entry with the highest number that has been reached is shown. E.g. 4 boops shows Face_Angry
// Leave the list empty ({}) to keep the current expression when booped
${generateBoopExpressions(boopExpressions)}

// Remote control button combos: press one button, then another. E.g. "AB" is A then B
// Actions: REMOTE_EXPRESSION (with an expression), REMOTE_RESET_TO_DEFAULT, REMOTE_CYCLE_BRIGHTNESS, REMOTE_TOGGLE_BOOP_SENSOR
// Combos that aren't in the list do nothing
${generateRemoteActions(remoteActions)}

// Expressions that move up and down slowly (every 300ms, instead of every 200ms)
${generateExpressionList('SlowMovementExpressions', movement.slow)}

// Expressions that move up and down very slowly (every 600ms)
${generateExpressionList('VerySlowMovementExpressions', movement.verySlow)}
`
}

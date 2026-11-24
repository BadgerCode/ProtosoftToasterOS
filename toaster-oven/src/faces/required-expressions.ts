// Expressions the firmware refers to by name. Removing or renaming one of these
// would stop ToasterOS compiling, so they can be edited but not renamed or removed.
// Keep in sync with ExpressionManager.h and RemoteControl.h.
// The default expression (DEFAULT_FACE) and extra default expressions (ExtraDefaultExpressions)
// aren't listed: the user chooses them in Protogen_Faces.h. The default can't be removed while
// it's the default, but it can be renamed.

const BOOP = 'Boop reactions'
const HEART = 'Heart animation'
const SPIRAL = 'Spiral animation'
const REMOTE = 'Remote control'

export const REQUIRED_EXPRESSIONS: Record<string, string[]> = {
  Surprised: [BOOP, REMOTE],
  Angry: [BOOP, REMOTE],
  X_X: [BOOP, REMOTE],
  Heart: [BOOP, HEART, REMOTE],
  Heart_Small: [HEART],
  Spiral: [SPIRAL, REMOTE],
  Spiral2: [SPIRAL],
  Spiral3: [SPIRAL],
  Spiral4: [SPIRAL],
  Blep: [REMOTE],
  Smirk: [REMOTE],
  Sleepy: [REMOTE],
  Excited: [REMOTE],
  Sad: [REMOTE],
  BoyKisser: [REMOTE],
  AmongUs: [REMOTE],
}

export function requiredBy(name: string): string[] | undefined {
  return REQUIRED_EXPRESSIONS[name]
}

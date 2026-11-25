// Expressions the firmware refers to by name. Removing or renaming one of these
// would stop ToasterOS compiling, so they can be edited but not renamed or removed.
// Keep in sync with ExpressionManager.h.
// The default expression (DEFAULT_FACE), extra default expressions (ExtraDefaultExpressions),
// boop expressions (BoopExpressions) and remote control combos (RemoteControlActions) aren't
// listed: the user chooses them in Protogen_Faces.h. The default can't be removed while
// it's the default, but it can be renamed.

const NOSE_RUB = 'Nose rub'
const HEART = 'Heart animation'
const SPIRAL = 'Spiral animation'
const MOVEMENT = 'Movement speed'

export const REQUIRED_EXPRESSIONS: Record<string, string[]> = {
  Heart: [NOSE_RUB, HEART, MOVEMENT],
  Heart_Small: [HEART],
  Spiral: [SPIRAL, MOVEMENT],
  Spiral2: [SPIRAL],
  Spiral3: [SPIRAL],
  Spiral4: [SPIRAL],
  Smirk: [MOVEMENT],
  Sleepy: [MOVEMENT],
  X_X: [MOVEMENT],
  AmongUs: [MOVEMENT],
}

export function requiredBy(name: string): string[] | undefined {
  return REQUIRED_EXPRESSIONS[name]
}

// Expressions the firmware refers to by name. Removing or renaming one of these
// would stop ToasterOS compiling, so they can be edited but not renamed or removed.
// Keep in sync with ExpressionManager.h.
// The default expression (DEFAULT_FACE), extra default expressions (ExtraDefaultExpressions),
// boop expressions (BoopExpressions), remote control combos (RemoteControlActions) and movement
// speeds (SlowMovementExpressions, VerySlowMovementExpressions) aren't listed: the user chooses
// them in Protogen_Faces.h. The default can't be removed while
// it's the default, but it can be renamed.

const NOSE_RUB = 'Nose rub'

export const REQUIRED_EXPRESSIONS: Record<string, string[]> = {
  Heart: [NOSE_RUB],
}

export function requiredBy(name: string): string[] | undefined {
  return REQUIRED_EXPRESSIONS[name]
}

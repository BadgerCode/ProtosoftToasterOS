import type { Expression } from './expressions'

// Expressions shown when booped (BoopExpressions in the firmware's Protogen_Faces.h).
// The entry with the highest minBoops that has been reached is shown, as in
// ExpressionManager::GetBoopExpression(). Rubbing the nose always shows hearts.

export interface BoopExpression {
  /** Shown once there have been at least this many boops in a row */
  minBoops: number
  expressionId: number
}

export function isValidMinBoops(minBoops: number) {
  return Number.isInteger(minBoops) && minBoops >= 1
}

/** Entries that can go in the file: valid counts and existing expressions, by count */
export function usableBoopExpressions(entries: BoopExpression[], expressions: Expression[]) {
  return entries
    .filter((b) => isValidMinBoops(b.minBoops) && expressions.some((e) => e.id === b.expressionId))
    .map((b) => ({
      minBoops: b.minBoops,
      expression: expressions.find((e) => e.id === b.expressionId)!,
    }))
    .sort((a, b) => a.minBoops - b.minBoops)
}

/** The expression to show after this many boops in a row, or null to keep the current one */
export function boopExpressionFor(
  boops: number,
  entries: BoopExpression[],
  expressions: Expression[],
): Expression | null {
  let match: Expression | null = null
  for (const b of usableBoopExpressions(entries, expressions)) {
    if (boops >= b.minBoops) match = b.expression
  }
  return match
}

/** Counts used by more than one entry. Only one of them would ever show */
export function duplicateMinBoops(entries: BoopExpression[]): number[] {
  const counts = entries.map((b) => b.minBoops).filter(isValidMinBoops)
  return [...new Set(counts.filter((c, i) => counts.indexOf(c) !== i))]
}

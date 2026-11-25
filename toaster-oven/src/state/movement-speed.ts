import { appStore } from './app-state'

// How fast each expression moves up and down (SlowMovementExpressions and
// VerySlowMovementExpressions in the firmware's Protogen_Faces.h), as in
// ExpressionManager::GetMovementDelay(). An expression is in at most one of the lists.

export type MovementSpeed = 'normal' | 'slow' | 'verySlow'

export const MOVEMENT_SPEEDS: { value: MovementSpeed; label: string }[] = [
  { value: 'normal', label: 'Normal' },
  { value: 'slow', label: 'Slow' },
  { value: 'verySlow', label: 'Very slow' },
]

export function getMovementSpeed(id: number): MovementSpeed {
  // The firmware checks the slow list first
  if (appStore.slowMovementExpressionIds.includes(id)) return 'slow'
  if (appStore.verySlowMovementExpressionIds.includes(id)) return 'verySlow'
  return 'normal'
}

function removeId(ids: number[], id: number) {
  const index = ids.indexOf(id)
  if (index !== -1) ids.splice(index, 1)
}

export function setMovementSpeed(id: number, speed: MovementSpeed) {
  removeId(appStore.slowMovementExpressionIds, id)
  removeId(appStore.verySlowMovementExpressionIds, id)
  if (speed === 'slow') appStore.slowMovementExpressionIds.push(id)
  if (speed === 'verySlow') appStore.verySlowMovementExpressionIds.push(id)
}

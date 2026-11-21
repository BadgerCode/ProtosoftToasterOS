import type { FaceConfig, FeatureConfig } from './config-types'

export interface PinUse {
  pin: number
  usedBy: string
}

/** Every digital pin in use by the current config (disabled features are ignored) */
export function digitalPinUsage(face: FaceConfig, features: FeatureConfig): PinUse[] {
  const uses: PinUse[] = []

  face.connections.forEach((c, i) => {
    const name = `Face connection ${i + 1}`
    uses.push({ pin: c.pinDataIn, usedBy: `${name} DIN` })
    uses.push({ pin: c.pinCS, usedBy: `${name} CS` })
    uses.push({ pin: c.pinCLK, usedBy: `${name} CLK` })
  })

  if (features.enableSideLEDs) {
    uses.push({ pin: features.pinLeftLEDStripData, usedBy: 'Left LED strip' })
    uses.push({ pin: features.pinRightLEDStripData, usedBy: 'Right LED strip' })
  }

  if (features.enableRemoteControl) {
    uses.push({ pin: features.pinRemoteButtonA, usedBy: 'Remote button A' })
    uses.push({ pin: features.pinRemoteButtonB, usedBy: 'Remote button B' })
    uses.push({ pin: features.pinRemoteButtonC, usedBy: 'Remote button C' })
    uses.push({ pin: features.pinRemoteButtonD, usedBy: 'Remote button D' })
    if (features.enableRemoteControlOutputLEDs) {
      uses.push({ pin: features.pinRemoteControlOutput, usedBy: 'Remote output LED' })
    }
  }

  return uses
}

/** Pins assigned to more than one thing, e.g. "Pin 9: Left LED strip, Face connection 1 CS" */
export function pinConflicts(face: FaceConfig, features: FeatureConfig): string[] {
  const byPin = new Map<number, string[]>()
  for (const use of digitalPinUsage(face, features)) {
    if (typeof use.pin !== 'number' || Number.isNaN(use.pin)) continue
    byPin.set(use.pin, [...(byPin.get(use.pin) ?? []), use.usedBy])
  }

  return [...byPin.entries()]
    .filter(([, users]) => users.length > 1)
    .sort(([a], [b]) => a - b)
    .map(([pin, users]) => `Pin ${pin}: ${users.join(', ')}`)
}

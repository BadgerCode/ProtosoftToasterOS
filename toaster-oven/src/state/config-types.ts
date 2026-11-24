// Mirrors ToasterOS/ConfigTypes.h

export const EMPTY_PANEL = -1

export const PanelType = {
  LEFT_MOUTH_BACK: 0,
  LEFT_MOUTH_MID_BACK: 1,
  LEFT_MOUTH_MID_FRONT: 2,
  LEFT_MOUTH_FRONT: 3,
  LEFT_NOSE: 4,
  LEFT_EYE_BACK: 5,
  LEFT_EYE_FRONT: 6,
  RIGHT_MOUTH_BACK: 7,
  RIGHT_MOUTH_MID_BACK: 8,
  RIGHT_MOUTH_MID_FRONT: 9,
  RIGHT_MOUTH_FRONT: 10,
  RIGHT_NOSE: 11,
  RIGHT_EYE_BACK: 12,
  RIGHT_EYE_FRONT: 13,
  SINGLE_NOSE: 14,
} as const

export type PanelTypeId = (typeof PanelType)[keyof typeof PanelType]

export interface PanelTypeInfo {
  id: PanelTypeId
  /** C++ #define name, e.g. PANEL_LEFT_MOUTH_BACK */
  define: string
  label: string
  group: 'Left' | 'Right' | 'Centre'
}

export const PANEL_TYPES: PanelTypeInfo[] = [
  { id: 0, define: 'PANEL_LEFT_MOUTH_BACK', label: 'Mouth back', group: 'Left' },
  { id: 1, define: 'PANEL_LEFT_MOUTH_MID_BACK', label: 'Mouth mid-back', group: 'Left' },
  { id: 2, define: 'PANEL_LEFT_MOUTH_MID_FRONT', label: 'Mouth mid-front', group: 'Left' },
  { id: 3, define: 'PANEL_LEFT_MOUTH_FRONT', label: 'Mouth front', group: 'Left' },
  { id: 4, define: 'PANEL_LEFT_NOSE', label: 'Nose', group: 'Left' },
  { id: 5, define: 'PANEL_LEFT_EYE_BACK', label: 'Eye back', group: 'Left' },
  { id: 6, define: 'PANEL_LEFT_EYE_FRONT', label: 'Eye front', group: 'Left' },
  { id: 7, define: 'PANEL_RIGHT_MOUTH_BACK', label: 'Mouth back', group: 'Right' },
  { id: 8, define: 'PANEL_RIGHT_MOUTH_MID_BACK', label: 'Mouth mid-back', group: 'Right' },
  { id: 9, define: 'PANEL_RIGHT_MOUTH_MID_FRONT', label: 'Mouth mid-front', group: 'Right' },
  { id: 10, define: 'PANEL_RIGHT_MOUTH_FRONT', label: 'Mouth front', group: 'Right' },
  { id: 11, define: 'PANEL_RIGHT_NOSE', label: 'Nose', group: 'Right' },
  { id: 12, define: 'PANEL_RIGHT_EYE_BACK', label: 'Eye back', group: 'Right' },
  { id: 13, define: 'PANEL_RIGHT_EYE_FRONT', label: 'Eye front', group: 'Right' },
  { id: 14, define: 'PANEL_SINGLE_NOSE', label: 'Single nose', group: 'Centre' },
]

export function panelTypeName(id: number): string {
  if (id === EMPTY_PANEL) return 'Empty'
  const info = PANEL_TYPES.find((p) => p.id === id)
  if (!info) return `Unknown (${id})`
  return info.group === 'Centre' ? info.label : `${info.group} ${info.label.toLowerCase()}`
}

/** Max panels on one MAX7219 chain that ToasterOS supports */
export const MAX_PANELS_PER_CONNECTION = 16

export interface PanelConfig {
  /** A PanelTypeId, or EMPTY_PANEL */
  panelType: number
  upsideDown: boolean
}

export interface FaceLEDConnection {
  pinDataIn: number
  pinCS: number
  pinCLK: number
  panels: PanelConfig[]
}

export interface FaceConfig {
  connections: FaceLEDConnection[]
}

export const DEBUG_MODES = [
  { value: 0, label: 'Off' },
  { value: 1, label: 'Log FPS' },
  { value: 2, label: 'Log max frame duration' },
  { value: 3, label: 'Render 7s on all panels' },
]

export interface FeatureConfig {
  // General
  brightness: number
  debugMode: number
  rapidPanelAutoRecovery: boolean

  // Boop sensor
  enableBoopSensor: boolean
  pinAnalogBoopSensor: number
  numBoopsToActivateGame: number

  // Side LED strips
  enableSideLEDs: boolean
  pinLeftLEDStripData: number
  pinRightLEDStripData: number
  ledStripNumLEDs: number

  // Remote control
  enableRemoteControl: boolean
  pinRemoteButtonA: number
  pinRemoteButtonB: number
  pinRemoteButtonC: number
  pinRemoteButtonD: number
  enableRemoteControlOutputLEDs: boolean
  pinRemoteControlOutput: number
}

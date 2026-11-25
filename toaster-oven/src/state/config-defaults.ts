import { PanelType, type FaceConfig, type FeatureConfig, type PanelConfig } from './config-types'

// Defaults match ToasterOS/Config.h

export interface FaceLayoutPreset {
  id: string
  name: string
  description: string
  build: () => FaceConfig
}

const panel = (panelType: number, upsideDown = false): PanelConfig => ({ panelType, upsideDown })

// Panels that are upside down in the two-connection preset (and ToasterOS/Config.h).
// The one-connection presets use the same orientation for each panel.
const UPSIDE_DOWN_PANELS = new Set<number>([
  PanelType.RIGHT_MOUTH_BACK,
  PanelType.RIGHT_MOUTH_MID_BACK,
  PanelType.RIGHT_MOUTH_MID_FRONT,
  PanelType.RIGHT_MOUTH_FRONT,
  PanelType.LEFT_NOSE,
  PanelType.LEFT_EYE_FRONT,
  PanelType.LEFT_EYE_BACK,
])

function oneConnection(panelTypes: number[]): FaceConfig {
  const panels = panelTypes.map((t) => panel(t, UPSIDE_DOWN_PANELS.has(t)))
  return { connections: [{ pinDataIn: 3, pinCS: 4, pinCLK: 5, panels }] }
}

export const FACE_LAYOUT_PRESETS: FaceLayoutPreset[] = [
  {
    id: '14-one',
    name: '14 panels, one connection',
    description: 'Two nose panels. Every panel on one chain.',
    // Right eye back -> front, right mouth back -> front, noses, left mouth front -> back,
    // left eye front -> back
    build: () =>
      oneConnection([
        PanelType.RIGHT_EYE_BACK,
        PanelType.RIGHT_EYE_FRONT,
        PanelType.RIGHT_MOUTH_BACK,
        PanelType.RIGHT_MOUTH_MID_BACK,
        PanelType.RIGHT_MOUTH_MID_FRONT,
        PanelType.RIGHT_MOUTH_FRONT,
        PanelType.RIGHT_NOSE,
        PanelType.LEFT_NOSE,
        PanelType.LEFT_MOUTH_FRONT,
        PanelType.LEFT_MOUTH_MID_FRONT,
        PanelType.LEFT_MOUTH_MID_BACK,
        PanelType.LEFT_MOUTH_BACK,
        PanelType.LEFT_EYE_FRONT,
        PanelType.LEFT_EYE_BACK,
      ]),
  },
  {
    id: '14-two',
    name: '14 panels, two connections',
    description: 'Two nose panels. Left and right sides wired separately.',
    build: () => {
      return {
        connections: [
          {
            pinDataIn: 3,
            pinCS: 4,
            pinCLK: 5,
            panels: [
              panel(PanelType.RIGHT_MOUTH_BACK, true),
              panel(PanelType.RIGHT_MOUTH_MID_BACK, true),
              panel(PanelType.RIGHT_MOUTH_MID_FRONT, true),
              panel(PanelType.RIGHT_MOUTH_FRONT, true),
              panel(PanelType.RIGHT_NOSE),
              panel(PanelType.RIGHT_EYE_FRONT),
              panel(PanelType.RIGHT_EYE_BACK),
            ],
          },
          {
            pinDataIn: 6,
            pinCS: 7,
            pinCLK: 8,
            panels: [
              panel(PanelType.LEFT_MOUTH_BACK),
              panel(PanelType.LEFT_MOUTH_MID_BACK),
              panel(PanelType.LEFT_MOUTH_MID_FRONT),
              panel(PanelType.LEFT_MOUTH_FRONT),
              panel(PanelType.LEFT_NOSE, true),
              panel(PanelType.LEFT_EYE_FRONT, true),
              panel(PanelType.LEFT_EYE_BACK, true),
            ],
          },
        ],
      }
    },
  },
  {
    id: '13-one',
    name: '13 panels, one connection',
    description: 'One centre nose panel. Every panel on one chain.',
    // Same order as the 14 panel preset, with one nose panel
    build: () =>
      oneConnection([
        PanelType.RIGHT_EYE_BACK,
        PanelType.RIGHT_EYE_FRONT,
        PanelType.RIGHT_MOUTH_BACK,
        PanelType.RIGHT_MOUTH_MID_BACK,
        PanelType.RIGHT_MOUTH_MID_FRONT,
        PanelType.RIGHT_MOUTH_FRONT,
        PanelType.SINGLE_NOSE,
        PanelType.LEFT_MOUTH_FRONT,
        PanelType.LEFT_MOUTH_MID_FRONT,
        PanelType.LEFT_MOUTH_MID_BACK,
        PanelType.LEFT_MOUTH_BACK,
        PanelType.LEFT_EYE_FRONT,
        PanelType.LEFT_EYE_BACK,
      ]),
  },
]

export function defaultFaceConfig(): FaceConfig {
  return FACE_LAYOUT_PRESETS[0]!.build()
}

export function defaultFeatureConfig(): FeatureConfig {
  return {
    brightness: 0.27,
    debugMode: 0,
    enableFaceBobbing: true,
    rapidPanelAutoRecovery: true,

    enableBoopSensor: false,
    pinAnalogBoopSensor: 0,
    numBoopsToActivateGame: 15,

    enableSideLEDs: false,
    sideLEDColourA: '#000071',
    sideLEDColourB: '#0000f5',
    pinLeftLEDStripData: 9,
    pinRightLEDStripData: 10,
    ledStripNumLEDs: 15,

    enableRemoteControl: false,
    pinRemoteButtonA: 18,
    pinRemoteButtonB: 17,
    pinRemoteButtonC: 16,
    pinRemoteButtonD: 15,
    enableRemoteControlOutputLEDs: false,
    pinRemoteControlOutput: 11,
  }
}

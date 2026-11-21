import { PanelType, type FaceConfig, type FeatureConfig } from './config-types'

// Defaults match ToasterOS/Config.h

export function defaultFaceConfig(): FaceConfig {
  return {
    connections: [
      {
        pinDataIn: 3,
        pinCS: 4,
        pinCLK: 5,
        panels: [
          { panelType: PanelType.RIGHT_MOUTH_BACK, upsideDown: true },
          { panelType: PanelType.RIGHT_MOUTH_MID_BACK, upsideDown: true },
          { panelType: PanelType.RIGHT_MOUTH_MID_FRONT, upsideDown: true },
          { panelType: PanelType.RIGHT_MOUTH_FRONT, upsideDown: true },
          { panelType: PanelType.RIGHT_NOSE, upsideDown: false },
          { panelType: PanelType.RIGHT_EYE_FRONT, upsideDown: false },
          { panelType: PanelType.RIGHT_EYE_BACK, upsideDown: false },
        ],
      },
      {
        pinDataIn: 6,
        pinCS: 7,
        pinCLK: 8,
        panels: [
          { panelType: PanelType.LEFT_MOUTH_BACK, upsideDown: false },
          { panelType: PanelType.LEFT_MOUTH_MID_BACK, upsideDown: false },
          { panelType: PanelType.LEFT_MOUTH_MID_FRONT, upsideDown: false },
          { panelType: PanelType.LEFT_MOUTH_FRONT, upsideDown: false },
          { panelType: PanelType.LEFT_NOSE, upsideDown: true },
          { panelType: PanelType.LEFT_EYE_FRONT, upsideDown: true },
          { panelType: PanelType.LEFT_EYE_BACK, upsideDown: true },
        ],
      },
    ],
  }
}

export function defaultFeatureConfig(): FeatureConfig {
  return {
    brightness: 0.25,
    debugMode: 0,
    rapidPanelAutoRecovery: true,

    enableBoopSensor: true,
    pinAnalogBoopSensor: 0,
    numBoopsToActivateGame: 15,

    enableSideLEDs: true,
    pinLeftLEDStripData: 9,
    pinRightLEDStripData: 10,
    ledStripNumLEDs: 15,

    enableRemoteControl: true,
    pinRemoteButtonA: 18,
    pinRemoteButtonB: 17,
    pinRemoteButtonC: 16,
    pinRemoteButtonD: 15,
    enableRemoteControlOutputLEDs: true,
    pinRemoteControlOutput: 11,
  }
}

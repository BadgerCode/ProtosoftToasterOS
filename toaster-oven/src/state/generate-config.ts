import {
  EMPTY_PANEL,
  PANEL_TYPES,
  type FaceConfig,
  type FaceLEDConnection,
  type FeatureConfig,
} from './config-types'

// Generates ToasterOS/Config.h from the app's config state.
// Keep the layout in sync with the Config.h in the ToasterOS project.

function panelTypeDefine(panelType: number): string {
  if (panelType === EMPTY_PANEL) return String(EMPTY_PANEL)
  const info = PANEL_TYPES.find((p) => p.id === panelType)
  if (!info) throw new Error(`Unknown panel type: ${panelType}`)
  return info.define
}

function generateConnection(connection: FaceLEDConnection): string {
  const numPanels = connection.panels.length
  const panels = connection.panels
    .map(
      (panel) =>
        `        { .PanelType = ${panelTypeDefine(panel.panelType)}, .UpsideDown = ${panel.upsideDown} },`,
    )
    .join('\n')

  return `    {
      // Pin configuration
      .PIN_DataIn = ${connection.pinDataIn},
      .PIN_CS = ${connection.pinCS},
      .PIN_CLK = ${connection.pinCLK},

      // Define the order of the panels
      .NumPanels = ${numPanels},
      .Panels = new PanelConfig[${numPanels}]{
${panels}
      },
    }`
}

export function generateConfigHeader(face: FaceConfig, features: FeatureConfig): string {
  const f = features

  return `// CONSTANT CONFIG
// PINS: LED STRIPS (have to be constant)
#define PIN_LEFT_LEDSTRIP_DATA ${f.pinLeftLEDStripData}
#define PIN_RIGHT_LEDSTRIP_DATA ${f.pinRightLEDStripData}
#define LEDSTRIP_NUM_LEDS ${f.ledStripNumLEDs}

// PINS: Remote control
#define PIN_REMOTECONTROL_OUTPUT ${f.pinRemoteControlOutput}


// Panel auto-recovery
// Automatically fixes panels with loose connections, at the slight cost to performance
// 1 to enable. 0 to disable
#define RAPID_PANEL_AUTORECOVERY ${f.rapidPanelAutoRecovery ? 1 : 0}

// Config
static ProtoConfigStruct ProtoConfig {
  // General config
  .Brightness = ${f.brightness}, // Percentage. E.g. 0.25 = 25%
  .DebugMode = ${f.debugMode}, // 0 = off, 1 = FPS log, 2 = max frame duration log, 3 = render numbers on all panels

  // Boop sensor config
  .EnableBoopSensor = ${f.enableBoopSensor},
  .PinAnalogBoopSensor = ${f.pinAnalogBoopSensor},
  .NumBoopsToActivateGame = ${f.numBoopsToActivateGame},

  // LED Strip config
  .EnableSideLEDs = ${f.enableSideLEDs},

  // Remote control config
  .EnableRemoteControl = ${f.enableRemoteControl},
  .PinRemoteButtonA = ${f.pinRemoteButtonA},
  .PinRemoteButtonB = ${f.pinRemoteButtonB},
  .PinRemoteButtonC = ${f.pinRemoteButtonC},
  .PinRemoteButtonD = ${f.pinRemoteButtonD},
  .EnableRemoteControlOutputLEDs = ${f.enableRemoteControlOutputLEDs},
};


// Configure the pins and layout of the face LED panels
// See README for more info
FaceConfig::FaceConfig() {
  NumConnections = ${face.connections.length};

  Connections = new FaceLEDConnection[NumConnections]{
${face.connections.map(generateConnection).join(',\n')}
  };
}
`
}

export function downloadBlob(fileName: string, blob: Blob) {
  const url = URL.createObjectURL(blob)
  const link = document.createElement('a')
  link.href = url
  link.download = fileName
  link.click()
  URL.revokeObjectURL(url)
}

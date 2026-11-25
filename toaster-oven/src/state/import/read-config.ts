import { defaultFaceConfig, defaultFeatureConfig } from '../config-defaults'
import {
  EMPTY_PANEL,
  PANEL_TYPES,
  type FaceConfig,
  type FaceLEDConnection,
  type FeatureConfig,
  type PanelConfig,
} from '../config-types'

// Reads a Config.h that Toaster Oven downloaded earlier, possibly from an older version.
//
// Each setting has a list of readers, newest format first. When the generated format
// changes, add a reader for the new format at the front and keep the old ones, so older
// downloads still import. Settings that no reader finds fall back to the default.

type Reader<T> = (text: string) => T | undefined
type Parser<T> = (value: string) => T | undefined

const number: Parser<number> = (v) => {
  const n = Number(v)
  return v !== '' && Number.isFinite(n) ? n : undefined
}

const int: Parser<number> = (v) => (/^-?\d+$/.test(v) ? Number(v) : undefined)

const bool: Parser<boolean> = (v) => {
  if (v === 'true' || v === '1') return true
  if (v === 'false' || v === '0') return false
  return undefined
}

/** `.Name = value,` inside a struct initialiser */
function structField<T>(name: string, parse: Parser<T>): Reader<T> {
  const pattern = new RegExp(`\\.${name}\\s*=\\s*([^,\\s}]+)`)
  return (text) => {
    const match = pattern.exec(text)
    return match ? parse(match[1]!) : undefined
  }
}

/** `#define NAME value` */
function define<T>(name: string, parse: Parser<T>): Reader<T> {
  const pattern = new RegExp(`^[ \\t]*#define[ \\t]+${name}[ \\t]+(\\S+)`, 'm')
  return (text) => {
    const match = pattern.exec(text)
    return match ? parse(match[1]!) : undefined
  }
}

interface Setting<K extends keyof FeatureConfig> {
  key: K
  /** Shown in the import summary when the setting falls back to its default */
  label: string
  readers: Reader<FeatureConfig[K]>[]
}

function setting<K extends keyof FeatureConfig>(
  key: K,
  label: string,
  readers: Reader<FeatureConfig[K]>[],
): Setting<K> {
  return { key, label, readers }
}

const SETTINGS = [
  setting('pinLeftLEDStripData', 'Left strip data pin', [define('PIN_LEFT_LEDSTRIP_DATA', int)]),
  setting('pinRightLEDStripData', 'Right strip data pin', [define('PIN_RIGHT_LEDSTRIP_DATA', int)]),
  setting('ledStripNumLEDs', 'LEDs per strip', [define('LEDSTRIP_NUM_LEDS', int)]),
  setting('pinRemoteControlOutput', 'Output LED pin', [define('PIN_REMOTECONTROL_OUTPUT', int)]),
  setting('rapidPanelAutoRecovery', 'Rapid panel auto-recovery', [
    define('RAPID_PANEL_AUTORECOVERY', bool),
  ]),
  setting('brightness', 'Brightness', [structField('Brightness', number)]),
  setting('debugMode', 'Debug mode', [structField('DebugMode', int)]),
  setting('enableFaceBobbing', 'Face bobbing', [structField('EnableFaceBobbing', bool)]),
  setting('enableBoopSensor', 'Boop sensor', [structField('EnableBoopSensor', bool)]),
  setting('pinAnalogBoopSensor', 'Boop sensor analog pin', [
    structField('PinAnalogBoopSensor', int),
  ]),
  setting('numBoopsToActivateGame', 'Boops to activate game', [
    structField('NumBoopsToActivateGame', int),
  ]),
  setting('enableSideLEDs', 'Side LED strips', [structField('EnableSideLEDs', bool)]),
  setting('enableRemoteControl', 'Remote control', [structField('EnableRemoteControl', bool)]),
  setting('pinRemoteButtonA', 'Remote button A pin', [structField('PinRemoteButtonA', int)]),
  setting('pinRemoteButtonB', 'Remote button B pin', [structField('PinRemoteButtonB', int)]),
  setting('pinRemoteButtonC', 'Remote button C pin', [structField('PinRemoteButtonC', int)]),
  setting('pinRemoteButtonD', 'Remote button D pin', [structField('PinRemoteButtonD', int)]),
  setting('enableRemoteControlOutputLEDs', 'Remote control output LEDs', [
    structField('EnableRemoteControlOutputLEDs', bool),
  ]),
]

// Fails the build if a FeatureConfig setting has no readers, so it can't be silently defaulted
type SettingsWithoutReaders = Exclude<keyof FeatureConfig, (typeof SETTINGS)[number]['key']>
const everySettingHasReaders: [SettingsWithoutReaders] extends [never]
  ? true
  : SettingsWithoutReaders = true
void everySettingHasReaders

const PANEL_LAYOUT_LABEL = 'Panel layout'

function readPanelType(value: string): number | undefined {
  if (/^-?\d+$/.test(value)) {
    const id = Number(value)
    return id === EMPTY_PANEL || PANEL_TYPES.some((p) => p.id === id) ? id : undefined
  }
  return PANEL_TYPES.find((p) => p.define === value)?.id
}

/** The panel connections in `FaceConfig::FaceConfig() { ... }`, or undefined if unreadable */
function readConnections(text: string): FaceLEDConnection[] | undefined {
  const start = text.indexOf('FaceConfig::FaceConfig')
  if (start === -1) return undefined

  // Each connection starts at its .PIN_DataIn and runs until the next one
  const blocks = text
    .slice(start)
    .split(/(?=\.PIN_DataIn\s*=)/)
    .slice(1)
  if (blocks.length === 0) return undefined

  const connections: FaceLEDConnection[] = []
  for (const block of blocks) {
    const pinDataIn = structField('PIN_DataIn', int)(block)
    const pinCS = structField('PIN_CS', int)(block)
    const pinCLK = structField('PIN_CLK', int)(block)
    if (pinDataIn === undefined || pinCS === undefined || pinCLK === undefined) return undefined

    const panels: PanelConfig[] = []
    for (const m of block.matchAll(
      /\{\s*\.PanelType\s*=\s*([\w-]+)\s*,\s*\.UpsideDown\s*=\s*(\w+)\s*,?\s*\}/g,
    )) {
      const panelType = readPanelType(m[1]!)
      const upsideDown = bool(m[2]!)
      if (panelType === undefined || upsideDown === undefined) return undefined
      panels.push({ panelType, upsideDown })
    }
    connections.push({ pinDataIn, pinCS, pinCLK, panels })
  }
  return connections
}

export function readConfig(text: string): {
  faceConfig: FaceConfig
  featureConfig: FeatureConfig
  defaulted: string[]
} {
  const featureConfig = defaultFeatureConfig()
  const defaulted: string[] = []

  for (const { key, label, readers } of SETTINGS) {
    let value: FeatureConfig[typeof key] | undefined
    for (const read of readers) {
      value = read(text)
      if (value !== undefined) break
    }
    if (value === undefined) defaulted.push(label)
    else (featureConfig as Record<typeof key, typeof value>)[key] = value
  }

  const connections = readConnections(text)
  if (!connections) defaulted.push(PANEL_LAYOUT_LABEL)

  return {
    faceConfig: connections ? { connections } : defaultFaceConfig(),
    featureConfig,
    defaulted,
  }
}

<script setup lang="ts">
import { computed } from 'vue'
import { getDefaultExpression } from '@/state/app-state'
import {
  EMPTY_PANEL,
  PANEL_TYPES,
  PanelType,
  panelTypeName,
  type FaceLEDConnection,
} from '@/state/config-types'
import { expressionToPanels } from '@/faces/faces'
import VisorDisplay, { type PanelMarker } from '@/components/VisorDisplay.vue'

// A map of the face panels for a panel layout: which panels are used, their place in each
// connection (e.g. C1 #3), and the wiring from the Arduino through each connection's panels.
// Used on the Panels page for your layout, and in the guides for the presets.

const props = defineProps<{ connections: FaceLEDConnection[] }>()

/** How many times each panel type appears across all connections */
const usageCounts = computed(() => {
  const counts = new Map<number, number>()
  for (const connection of props.connections) {
    for (const panel of connection.panels) {
      if (panel.panelType === EMPTY_PANEL) continue
      counts.set(panel.panelType, (counts.get(panel.panelType) ?? 0) + 1)
    }
  }
  return counts
})

// Lights each assigned panel with its part of the default expression
const defaultFace = computed(() => expressionToPanels(getDefaultExpression()))

const mapPanelIds = computed(() => {
  const usesSideNose =
    usageCounts.value.has(PanelType.LEFT_NOSE) || usageCounts.value.has(PanelType.RIGHT_NOSE)
  const showSingleNose = usageCounts.value.has(PanelType.SINGLE_NOSE) || !usesSideNose
  return PANEL_TYPES.map((p) => p.id).filter((id) => id !== PanelType.SINGLE_NOSE || showSingleNose)
})

const mapBitmaps = computed(() =>
  Object.fromEntries(
    Object.entries(defaultFace.value).filter(([id]) => usageCounts.value.has(Number(id))),
  ),
)

const mapMarkers = computed(() => {
  const positions = new Map<number, { connection: number; position: number }[]>()
  props.connections.forEach((connection, ci) =>
    connection.panels.forEach((panel, pi) => {
      if (panel.panelType === EMPTY_PANEL) return
      positions.set(panel.panelType, [
        ...(positions.get(panel.panelType) ?? []),
        { connection: ci + 1, position: pi + 1 },
      ])
    }),
  )

  const describe = (p: { connection: number; position: number }) =>
    `C${p.connection} #${p.position}`

  const markers: Record<number, PanelMarker> = {}
  for (const type of PANEL_TYPES) {
    const uses = positions.get(type.id) ?? []
    const name = panelTypeName(type.id)
    if (uses.length === 0) {
      markers[type.id] = { state: 'unused', title: `${name}: not in use` }
    } else if (uses.length === 1) {
      markers[type.id] = {
        state: 'used',
        label: describe(uses[0]!),
        title: `${name}: connection ${uses[0]!.connection}, panel ${uses[0]!.position}`,
      }
    } else {
      markers[type.id] = {
        state: 'attention',
        label: `Used ${uses.length}×`,
        title: `${name}: used more than once (${uses.map(describe).join(', ')})`,
      }
    }
  }
  return markers
})

// Wiring on the map: each connection's panels in order, starting from the Arduino
const mapChains = computed(() =>
  props.connections
    .map((connection, ci) => ({
      label: `Connection ${ci + 1}`,
      title: `Connection ${ci + 1} from the Arduino: DIN pin ${connection.pinDataIn}, CS pin ${connection.pinCS}, CLK pin ${connection.pinCLK}`,
      panelIds: connection.panels.map((p) => p.panelType).filter((t) => t !== EMPTY_PANEL),
    }))
    .filter((chain) => chain.panelIds.length > 0),
)
</script>

<template>
  <VisorDisplay
    :bitmaps="mapBitmaps"
    :panel-ids="mapPanelIds"
    :markers="mapMarkers"
    :chains="mapChains"
  />
</template>

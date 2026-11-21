<script setup lang="ts">
import { computed } from 'vue'
import { appStore } from '@/state/app-state'
import {
  EMPTY_PANEL,
  MAX_PANELS_PER_CONNECTION,
  PANEL_TYPES,
  PanelType,
  panelTypeName,
  type FaceLEDConnection,
  type PanelTypeInfo,
} from '@/state/config-types'
import { FACE_NEUTRAL, expressionToPanels } from '@/faces/faces'
import VisorDisplay, { type PanelMarker } from '@/components/VisorDisplay.vue'
import SettingsCard from '@/components/form/SettingsCard.vue'
import FormField from '@/components/form/FormField.vue'
import PinConflictWarning from '@/components/PinConflictWarning.vue'

const face = appStore.faceConfig

const groups: PanelTypeInfo['group'][] = ['Left', 'Centre', 'Right']
const panelTypesByGroup = groups.map((group) => ({
  group,
  types: PANEL_TYPES.filter((p) => p.group === group),
}))

/** How many times each panel type appears across all connections */
const usageCounts = computed(() => {
  const counts = new Map<number, number>()
  for (const connection of face.connections) {
    for (const panel of connection.panels) {
      if (panel.panelType === EMPTY_PANEL) continue
      counts.set(panel.panelType, (counts.get(panel.panelType) ?? 0) + 1)
    }
  }
  return counts
})

const duplicatePanels = computed(() =>
  PANEL_TYPES.filter((p) => (usageCounts.value.get(p.id) ?? 0) > 1),
)

// Visor map: lights each assigned panel with its part of the neutral face
const neutralFace = expressionToPanels(FACE_NEUTRAL)

const mapPanelIds = computed(() => {
  const usesSideNose =
    usageCounts.value.has(PanelType.LEFT_NOSE) || usageCounts.value.has(PanelType.RIGHT_NOSE)
  const showSingleNose = usageCounts.value.has(PanelType.SINGLE_NOSE) || !usesSideNose
  return PANEL_TYPES.map((p) => p.id).filter((id) => id !== PanelType.SINGLE_NOSE || showSingleNose)
})

const mapBitmaps = computed(() =>
  Object.fromEntries(
    Object.entries(neutralFace).filter(([id]) => usageCounts.value.has(Number(id))),
  ),
)

const mapMarkers = computed(() => {
  const positions = new Map<number, { connection: number; position: number }[]>()
  face.connections.forEach((connection, ci) =>
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

function addConnection() {
  face.connections.push({ pinDataIn: 0, pinCS: 0, pinCLK: 0, panels: [] })
}

function removeConnection(index: number) {
  face.connections.splice(index, 1)
}

function addPanel(connection: FaceLEDConnection) {
  const firstUnused = PANEL_TYPES.find((p) => !usageCounts.value.has(p.id))
  connection.panels.push({ panelType: firstUnused?.id ?? EMPTY_PANEL, upsideDown: false })
}

function removePanel(connection: FaceLEDConnection, index: number) {
  connection.panels.splice(index, 1)
}

function movePanel(connection: FaceLEDConnection, index: number, offset: number) {
  const target = index + offset
  if (target < 0 || target >= connection.panels.length) return
  const [panel] = connection.panels.splice(index, 1)
  connection.panels.splice(target, 0, panel!)
}
</script>

<template>
  <div class="space-y-6">
    <PinConflictWarning />

    <!-- Panels in use -->
    <SettingsCard
      title="Panels in use"
      description="Every face panel and whether it's been assigned to a connection. Unused panels are simply not rendered."
    >
      <div class="mx-auto max-w-3xl">
        <VisorDisplay :bitmaps="mapBitmaps" :panel-ids="mapPanelIds" :markers="mapMarkers" />
      </div>

      <p class="mt-4 text-sm text-slate-300">
        <span class="rounded bg-page px-1.5 py-0.5 font-semibold text-white">C1 #3</span>
        means the 3rd panel on Connection 1.
      </p>

      <ul class="mt-3 flex flex-wrap gap-x-6 gap-y-2 text-sm text-slate-400">
        <li class="flex items-center gap-2">
          <span class="size-3 rounded-sm border border-slate-500 bg-led/30" />
          In use
        </li>
        <li class="flex items-center gap-2">
          <span class="size-3 rounded-sm border border-dashed border-slate-600" />
          Not in use
        </li>
        <li class="flex items-center gap-2">
          <span class="size-3 rounded-sm border border-gold-400 bg-gold-400/30" />
          Used more than once
        </li>
      </ul>

      <p v-if="duplicatePanels.length" class="mt-4 text-sm text-gold-300">
        Used more than once: {{ duplicatePanels.map((p) => panelTypeName(p.id)).join(', ') }}
      </p>
    </SettingsCard>

    <!-- Connections -->
    <SettingsCard
      v-for="(connection, ci) in face.connections"
      :key="ci"
      :title="`Connection ${ci + 1} (C${ci + 1})`"
      description="A chain of MAX7219 panels wired to the same three pins."
    >
      <template #actions>
        <button
          type="button"
          class="btn btn-secondary"
          :disabled="face.connections.length <= 1"
          @click="removeConnection(ci)"
        >
          Remove connection
        </button>
      </template>

      <div class="grid gap-4 sm:grid-cols-3">
        <FormField label="Data in (DIN) pin">
          <input v-model.number="connection.pinDataIn" type="number" min="0" class="form-input" />
        </FormField>
        <FormField label="Chip select (CS) pin">
          <input v-model.number="connection.pinCS" type="number" min="0" class="form-input" />
        </FormField>
        <FormField label="Clock (CLK) pin">
          <input v-model.number="connection.pinCLK" type="number" min="0" class="form-input" />
        </FormField>
      </div>

      <div class="mt-6">
        <div class="mb-2 flex items-baseline justify-between gap-4">
          <h3 class="text-sm font-medium text-slate-200">Panel order</h3>
          <span
            class="text-xs"
            :class="
              connection.panels.length > MAX_PANELS_PER_CONNECTION
                ? 'text-gold-300'
                : 'text-slate-500'
            "
          >
            {{ connection.panels.length }} / {{ MAX_PANELS_PER_CONNECTION }} panels
          </span>
        </div>
        <p class="mb-3 text-xs text-slate-500">
          List panels in the order they're wired, starting with the one connected to the Arduino.
        </p>

        <ol
          v-if="connection.panels.length"
          class="divide-y divide-border rounded-md border border-border"
        >
          <li
            v-for="(panel, pi) in connection.panels"
            :key="pi"
            class="flex flex-wrap items-center gap-3 px-3 py-2"
          >
            <span class="w-6 text-right text-sm font-medium text-slate-500 tabular-nums">
              {{ pi + 1 }}
            </span>

            <select
              v-model.number="panel.panelType"
              class="form-input min-w-0 flex-1 sm:max-w-64"
              :aria-label="`Panel ${pi + 1} type`"
            >
              <option :value="EMPTY_PANEL">Empty (not used)</option>
              <optgroup v-for="g in panelTypesByGroup" :key="g.group" :label="g.group">
                <option v-for="type in g.types" :key="type.id" :value="type.id">
                  {{ panelTypeName(type.id) }}
                </option>
              </optgroup>
            </select>

            <label class="flex cursor-pointer items-center gap-2 text-sm text-slate-300">
              <input v-model="panel.upsideDown" type="checkbox" class="size-4 accent-accent" />
              Upside down
            </label>

            <div class="ml-auto flex items-center gap-1">
              <button
                type="button"
                class="btn btn-icon"
                title="Move up"
                :disabled="pi === 0"
                @click="movePanel(connection, pi, -1)"
              >
                <svg class="size-4" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
                  <path
                    fill-rule="evenodd"
                    d="M14.77 12.79a.75.75 0 0 1-1.06-.02L10 8.83l-3.71 3.94a.75.75 0 1 1-1.08-1.04l4.25-4.5a.75.75 0 0 1 1.08 0l4.25 4.5a.75.75 0 0 1-.02 1.06Z"
                  />
                </svg>
              </button>
              <button
                type="button"
                class="btn btn-icon"
                title="Move down"
                :disabled="pi === connection.panels.length - 1"
                @click="movePanel(connection, pi, 1)"
              >
                <svg class="size-4" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
                  <path
                    fill-rule="evenodd"
                    d="M5.23 7.21a.75.75 0 0 1 1.06.02L10 11.17l3.71-3.94a.75.75 0 1 1 1.08 1.04l-4.25 4.5a.75.75 0 0 1-1.08 0l-4.25-4.5a.75.75 0 0 1 .02-1.06Z"
                  />
                </svg>
              </button>
              <button
                type="button"
                class="btn btn-danger-icon"
                title="Remove panel"
                @click="removePanel(connection, pi)"
              >
                <svg class="size-4" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
                  <path
                    d="M6.28 5.22a.75.75 0 0 0-1.06 1.06L8.94 10l-3.72 3.72a.75.75 0 1 0 1.06 1.06L10 11.06l3.72 3.72a.75.75 0 1 0 1.06-1.06L11.06 10l3.72-3.72a.75.75 0 0 0-1.06-1.06L10 8.94 6.28 5.22Z"
                  />
                </svg>
              </button>
            </div>
          </li>
        </ol>
        <p
          v-else
          class="rounded-md border border-dashed border-border px-3 py-6 text-center text-sm text-slate-500"
        >
          No panels on this connection yet.
        </p>

        <button type="button" class="btn btn-secondary mt-3" @click="addPanel(connection)">
          + Add panel
        </button>
      </div>
    </SettingsCard>

    <button type="button" class="btn btn-primary" @click="addConnection">+ Add connection</button>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { appStore } from '@/state/app-state'
import { DEBUG_MODES } from '@/state/config-types'
import { displayName } from '@/state/expressions'
import { duplicateMinBoops, isValidMinBoops } from '@/state/boop-expressions'
import {
  REMOTE_BUILT_IN_ACTIONS,
  REMOTE_BUTTONS,
  type RemoteAction,
  type RemoteActionType,
} from '@/state/remote-actions'
import SettingsCard from '@/components/form/SettingsCard.vue'
import FormField from '@/components/form/FormField.vue'
import ToggleSwitch from '@/components/form/ToggleSwitch.vue'
import PinConflictWarning from '@/components/PinConflictWarning.vue'

const config = appStore.featureConfig

const brightnessPercent = computed(() => Math.round(config.brightness * 100))

// Boop expressions: shown by the number of boops in a row
const boopExpressions = appStore.boopExpressions
const duplicateBoops = computed(() => duplicateMinBoops(boopExpressions))

function boopWarning(minBoops: number): string | null {
  if (!isValidMinBoops(minBoops)) return 'Use a whole number, 1 or more.'
  if (duplicateBoops.value.includes(minBoops)) {
    return `Another expression also uses ${minBoops}. Only one of them will show.`
  }
  if (minBoops >= config.numBoopsToActivateGame) {
    return `The game starts at ${config.numBoopsToActivateGame} boops, so this won't show.`
  }
  return null
}

// Remote control combos: one select per combo, for an action or an expression
const remoteActions = appStore.remoteActions

function remoteActionsStartingWith(button: string) {
  return remoteActions.filter((a) => a.buttons[0] === button)
}

function remoteSelectValue(action: RemoteAction) {
  return action.action === 'REMOTE_EXPRESSION' ? `expression:${action.expressionId}` : action.action
}

function setRemoteAction(action: RemoteAction, value: string) {
  if (value.startsWith('expression:')) {
    action.action = 'REMOTE_EXPRESSION'
    action.expressionId = Number(value.slice('expression:'.length))
  } else {
    action.action = value as RemoteActionType
    action.expressionId = null
  }
}

function addBoopExpression() {
  const highest = Math.max(0, ...boopExpressions.map((b) => b.minBoops).filter(isValidMinBoops))
  boopExpressions.push({ minBoops: highest + 1, expressionId: appStore.expressions[0]!.id })
}

const remoteButtons = [
  { key: 'pinRemoteButtonA', label: 'Button A pin' },
  { key: 'pinRemoteButtonB', label: 'Button B pin' },
  { key: 'pinRemoteButtonC', label: 'Button C pin' },
  { key: 'pinRemoteButtonD', label: 'Button D pin' },
] as const
</script>

<template>
  <div class="space-y-6">
    <PinConflictWarning />

    <!-- General -->
    <SettingsCard title="General">
      <div class="space-y-5">
        <FormField label="Brightness" hint="Default brightness of the face panels and LED strips.">
          <span class="flex items-center gap-4">
            <input
              v-model.number="config.brightness"
              type="range"
              min="0"
              max="1"
              step="0.0666"
              class="flex-1 accent-accent"
            />
            <span class="w-12 text-right text-sm font-medium text-accent tabular-nums">
              {{ brightnessPercent }}%
            </span>
          </span>
        </FormField>

        <ToggleSwitch
          v-model="config.enableFaceBobbing"
          label="Face bobbing"
          description="Move the face up and down. Each expression's speed is set in the Expressions tab."
        />

        <ToggleSwitch
          v-model="config.rapidPanelAutoRecovery"
          label="Rapid panel auto-recovery"
          description="Prevent panels with loose connections from getting stuck (and needing a reboot), at a slight cost to performance."
        />

        <FormField
          label="Debug mode"
          hint="Use 'Render numbers' to check panel order and orientation."
        >
          <select v-model.number="config.debugMode" class="form-input sm:max-w-64">
            <option v-for="mode in DEBUG_MODES" :key="mode.value" :value="mode.value">
              {{ mode.label }}
            </option>
          </select>
        </FormField>
      </div>
    </SettingsCard>

    <!-- Boop sensor -->
    <SettingsCard title="Boop sensor" description="IR obstacle sensor in the nose.">
      <template #actions>
        <ToggleSwitch v-model="config.enableBoopSensor" label="Enabled" />
      </template>

      <fieldset :disabled="!config.enableBoopSensor" class="space-y-5">
        <div class="grid gap-4 sm:grid-cols-2">
          <FormField label="Analog pin" hint="E.g. 0 for A0.">
            <input
              v-model.number="config.pinAnalogBoopSensor"
              type="number"
              min="0"
              class="form-input"
            />
          </FormField>
          <FormField label="Boops to activate game">
            <input
              v-model.number="config.numBoopsToActivateGame"
              type="number"
              min="1"
              class="form-input"
            />
          </FormField>
        </div>

        <div>
          <h3 class="text-sm font-medium text-slate-200">Boop expressions</h3>
          <p class="mt-0.5 text-xs text-slate-500">
            Shown when booped, by the number of boops in a row. The highest number reached is used.
            With none, the current expression stays. Rubbing the nose always shows hearts.
          </p>

          <ul v-if="boopExpressions.length" class="mt-3 space-y-2">
            <li v-for="(boop, i) in boopExpressions" :key="i">
              <div class="flex items-center gap-2">
                <input
                  v-model.number="boop.minBoops"
                  type="number"
                  min="1"
                  step="1"
                  class="form-input w-20 shrink-0"
                  :class="{ '!border-gold-400': boopWarning(boop.minBoops) }"
                  :aria-label="`Boops for expression ${i + 1}`"
                />
                <span class="shrink-0 text-sm text-slate-400">or more</span>
                <select
                  v-model.number="boop.expressionId"
                  class="form-input min-w-0 flex-1 sm:max-w-64"
                  :aria-label="`Expression ${i + 1}`"
                >
                  <option
                    v-for="expression in appStore.expressions"
                    :key="expression.id"
                    :value="expression.id"
                  >
                    {{ displayName(expression.name) }}
                  </option>
                </select>
                <button
                  type="button"
                  class="btn btn-danger-icon shrink-0"
                  title="Remove boop expression"
                  @click="boopExpressions.splice(i, 1)"
                >
                  <svg class="size-4" viewBox="0 0 20 20" fill="currentColor" aria-hidden="true">
                    <path
                      d="M6.28 5.22a.75.75 0 0 0-1.06 1.06L8.94 10l-3.72 3.72a.75.75 0 1 0 1.06 1.06L10 11.06l3.72 3.72a.75.75 0 1 0 1.06-1.06L11.06 10l3.72-3.72a.75.75 0 0 0-1.06-1.06L10 8.94 6.28 5.22Z"
                    />
                  </svg>
                </button>
              </div>
              <p v-if="boopWarning(boop.minBoops)" class="mt-1 text-xs text-gold-300">
                {{ boopWarning(boop.minBoops) }}
              </p>
            </li>
          </ul>

          <button type="button" class="btn btn-secondary mt-3" @click="addBoopExpression">
            + Add expression
          </button>
        </div>
      </fieldset>
    </SettingsCard>

    <!-- Side LEDs -->
    <SettingsCard
      title="Side LED strips"
      description="RGB LED strips in each side panel/headphone."
    >
      <template #actions>
        <ToggleSwitch v-model="config.enableSideLEDs" label="Enabled" />
      </template>

      <fieldset :disabled="!config.enableSideLEDs" class="grid gap-4 sm:grid-cols-3">
        <FormField label="Left strip data pin">
          <input
            v-model.number="config.pinLeftLEDStripData"
            type="number"
            min="0"
            class="form-input"
          />
        </FormField>
        <FormField label="Right strip data pin">
          <input
            v-model.number="config.pinRightLEDStripData"
            type="number"
            min="0"
            class="form-input"
          />
        </FormField>
        <FormField label="LEDs per strip">
          <input v-model.number="config.ledStripNumLEDs" type="number" min="1" class="form-input" />
        </FormField>
      </fieldset>
    </SettingsCard>

    <!-- Remote control -->
    <SettingsCard title="Remote control" description="4-button remote receiver.">
      <template #actions>
        <ToggleSwitch v-model="config.enableRemoteControl" label="Enabled" />
      </template>

      <fieldset :disabled="!config.enableRemoteControl" class="space-y-5">
        <div class="grid gap-4 sm:grid-cols-4">
          <FormField v-for="button in remoteButtons" :key="button.key" :label="button.label">
            <input v-model.number="config[button.key]" type="number" min="0" class="form-input" />
          </FormField>
        </div>

        <div class="grid items-end gap-4 sm:grid-cols-2">
          <ToggleSwitch
            v-model="config.enableRemoteControlOutputLEDs"
            label="Output LEDs"
            description="LED to feedback which button has been pressed, the current expression and boop status."
          />
          <FormField label="Output LED pin">
            <input
              v-model.number="config.pinRemoteControlOutput"
              type="number"
              min="0"
              class="form-input"
              :disabled="!config.enableRemoteControlOutputLEDs"
            />
          </FormField>
        </div>

        <div>
          <h3 class="text-sm font-medium text-slate-200">Button combos</h3>
          <p class="mt-0.5 text-xs text-slate-500">
            Press one button, then another within 3 seconds, to show an expression or change a
            setting.
          </p>

          <div class="mt-3 grid gap-x-8 gap-y-5 sm:grid-cols-2">
            <div v-for="first in REMOTE_BUTTONS" :key="first" class="space-y-2">
              <label
                v-for="action in remoteActionsStartingWith(first)"
                :key="action.buttons"
                class="flex items-center gap-3"
              >
                <span class="flex shrink-0 items-center gap-1 text-xs text-slate-500">
                  <kbd class="kbd">{{ action.buttons[0] }}</kbd>
                  then
                  <kbd class="kbd">{{ action.buttons[1] }}</kbd>
                </span>
                <select
                  :value="remoteSelectValue(action)"
                  class="form-input min-w-0 flex-1"
                  @change="setRemoteAction(action, ($event.target as HTMLSelectElement).value)"
                >
                  <option value="none">Nothing</option>
                  <optgroup label="Settings">
                    <option
                      v-for="builtIn in REMOTE_BUILT_IN_ACTIONS"
                      :key="builtIn.value"
                      :value="builtIn.value"
                    >
                      {{ builtIn.label }}
                    </option>
                  </optgroup>
                  <optgroup label="Expressions">
                    <option
                      v-for="expression in appStore.expressions"
                      :key="expression.id"
                      :value="`expression:${expression.id}`"
                    >
                      {{ displayName(expression.name) }}
                    </option>
                  </optgroup>
                </select>
              </label>
            </div>
          </div>
        </div>
      </fieldset>
    </SettingsCard>
  </div>
</template>

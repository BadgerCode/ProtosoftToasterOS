<script setup lang="ts">
import { computed } from 'vue'
import { appStore } from '@/state/app-state'
import { DEBUG_MODES } from '@/state/config-types'
import SettingsCard from '@/components/form/SettingsCard.vue'
import FormField from '@/components/form/FormField.vue'
import ToggleSwitch from '@/components/form/ToggleSwitch.vue'
import PinConflictWarning from '@/components/PinConflictWarning.vue'

const config = appStore.featureConfig

const brightnessPercent = computed(() => Math.round(config.brightness * 100))

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
          v-model="config.rapidPanelAutoRecovery"
          label="Rapid panel auto-recovery"
          description="Prevent panels with loose connections from getting stuck (and needing a reboot), at a slight cost to performance."
        />

        <FormField label="Debug mode" hint="Use 'Render 7s' to check panel orientation.">
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

      <fieldset :disabled="!config.enableBoopSensor" class="grid gap-4 sm:grid-cols-2">
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
      </fieldset>
    </SettingsCard>
  </div>
</template>

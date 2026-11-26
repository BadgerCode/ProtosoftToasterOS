import { h, type VNode } from 'vue'
import PanelLayoutMap from '@/components/PanelLayoutMap.vue'
import { FACE_LAYOUT_PRESETS } from '@/state/config-defaults'

// Parts of the site that guides can show, with a line like `::panel-layout 14-one`.
// Each takes the rest of the line (here "14-one") and returns what to show.

const problem = (message: string) => h('p', { class: 'guide-embed-problem' }, message)

export const GUIDE_EMBEDS: Record<string, (args: string) => VNode> = {
  /**
   * The panel layout map for a preset, with each panel's place in its connection and the wiring.
   * The presets are FACE_LAYOUT_PRESETS in state/config-defaults.ts: 14-one, 14-two and 13-one.
   */
  'panel-layout': (args) => {
    const preset = FACE_LAYOUT_PRESETS.find((p) => p.id === args.trim())
    if (!preset) {
      const ids = FACE_LAYOUT_PRESETS.map((p) => p.id).join(', ')
      return problem(`There's no panel layout preset called "${args}". Use one of: ${ids}`)
    }
    return h(PanelLayoutMap, { connections: preset.build().connections })
  },
}

export function guideEmbed(name: string, args: string): VNode {
  const embed = GUIDE_EMBEDS[name]
  return embed ? embed(args) : problem(`There's nothing called "${name}" to show here`)
}

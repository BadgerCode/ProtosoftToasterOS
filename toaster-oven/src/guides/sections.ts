// The sections of the guides page, in order. Each section's guides are the .md files in the
// folder of the same name, e.g. src/guides/toaster-os/. See guides.ts for how to write a guide.

export interface GuideSection {
  folder: string
  title: string
  description: string
  /** Topics still to be written, shown as "coming soon" */
  comingSoon: string[]
  /**
   * Groups whose guides aren't numbered, for groups where an order doesn't make sense.
   * Every other list of guides is numbered from 1
   */
  unnumberedGroups: string[]
}

/** The sections of the guides page, in order. A section needs a folder here to show */
export const GUIDE_SECTIONS: GuideSection[] = [
  {
    folder: 'toaster-os',
    title: 'Toaster OS',
    description: 'Software for protogens using MAX7219 8x8 LED panels on Arduino or ESP32.',
    comingSoon: [],
    unnumberedGroups: ['Enabling features'],
  },
  {
    folder: 'building',
    title: 'Building a protogen',
    description: 'From the head base to the fur, the body and armour.',
    comingSoon: [
      'Choosing a head base',
      'Ears',
      'Sanding and painting',
      'The fur',
      'Electronics and software',
      'Extra features',
      'Paws',
      'Full suit',
      'Armour',
    ],
    unnumberedGroups: [],
  },
]

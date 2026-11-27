import { renderMarkdown } from './markdown'

// The guides are .md files in a folder per section, e.g. toaster-os/setting-up.md.
// Each starts with a header like:
//
// ---
// title: Setting up
// description: Wire up the LED panels and put Toaster OS on your microcontroller.
// order: 1
// group: Enabling features   (optional: shows the guide under this heading)
// ---
//
// The path is the guide's address: toaster-os/setting-up.md -> /guides/toaster-os/setting-up.
// Pictures go in ./images and are linked as ![Description](images/picture.png).
// The sections themselves (titles, descriptions, coming-soon topics) are set in sections.ts.

export interface Guide {
  /** e.g. "toaster-os/setting-up" */
  slug: string
  /** The section's folder, e.g. "toaster-os" */
  section: string
  title: string
  description: string
  order: number
  /** Shown under this heading in the section, e.g. "Enabling features" */
  group: string | null
  markdown: string
}

const files = import.meta.glob('./*/*.md', {
  query: '?raw',
  import: 'default',
  eager: true,
}) as Record<string, string>
const images = import.meta.glob('./images/*', {
  query: '?url',
  import: 'default',
  eager: true,
}) as Record<string, string>

function readGuide(path: string, text: string): Guide {
  const slug = path.replace(/^\.\//, '').replace(/\.md$/, '')
  const header = text.match(/^---\r?\n([\s\S]*?)\r?\n---\r?\n/)
  const fields: Record<string, string> = {}
  for (const line of header?.[1]?.split(/\r?\n/) ?? []) {
    const field = line.match(/^(\w+):\s*(.*)$/)
    if (field) fields[field[1]!] = field[2]!.trim()
  }
  return {
    slug,
    section: slug.split('/')[0]!,
    title: fields.title ?? slug,
    description: fields.description ?? '',
    order: Number(fields.order ?? 999),
    group: fields.group || null,
    markdown: header ? text.slice(header[0].length) : text,
  }
}

/** Every guide, in order */
export const GUIDES: Guide[] = Object.entries(files)
  .map(([path, text]) => readGuide(path, text))
  .sort((a, b) => a.order - b.order || a.title.localeCompare(b.title))

export function findGuide(slug: string) {
  return GUIDES.find((guide) => guide.slug === slug)
}

/** A section's guides, in order */
export function sectionGuides(folder: string) {
  return GUIDES.filter((guide) => guide.section === folder)
}

/**
 * A section's guides as headings with their guides: guides without a group stand alone, and a
 * group appears where its first guide comes in the order
 */
export function sectionContents(folder: string) {
  const contents: { group: string | null; guides: Guide[] }[] = []
  for (const guide of sectionGuides(folder)) {
    const existing = guide.group && contents.find((c) => c.group === guide.group)
    if (existing) existing.guides.push(guide)
    else contents.push({ group: guide.group, guides: [guide] })
  }
  return contents
}

export function renderGuide(guide: Guide) {
  return renderMarkdown(guide.markdown, (src) => images[`./${src}`] ?? src)
}

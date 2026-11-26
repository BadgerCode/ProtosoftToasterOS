// A small Markdown renderer for the guides, so the site doesn't need another package.
//
// Supports: # to #### headings, paragraphs, **bold**, *italic*, `code`, [links](url),
// ![images](images/file.png), - and 1. lists (indent by 2+ spaces to nest), ``` code blocks,
// > quotes, > [!NOTE] / [!TIP] / [!WARNING] callouts, | tables |, --- rules, and line breaks:
// end a line with \ to carry on straight underneath, without starting a new paragraph.
// HTML in the Markdown is shown as text, not run.

export interface GuideHeading {
  id: string
  text: string
  level: number
}

export interface RenderedMarkdown {
  html: string
  headings: GuideHeading[]
}

const escapeHtml = (text: string) =>
  text.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;')

/** "Setting up the code" -> "setting-up-the-code" */
export function slugify(text: string) {
  return text
    .toLowerCase()
    .replace(/[`*_[\]()!]/g, '')
    .replace(/[^a-z0-9]+/g, '-')
    .replace(/^-+|-+$/g, '')
}

type ResolveImage = (src: string) => string

function inline(text: string, resolveImage: ResolveImage): string {
  // Keep code spans exactly as written
  const codes: string[] = []
  let html = text.replace(/`([^`]+)`/g, (_, code: string) => {
    codes.push(escapeHtml(code))
    return `\u0000${codes.length - 1}\u0000`
  })
  html = escapeHtml(html)
  html = html.replace(
    /!\[([^\]]*)\]\(([^)\s]+)\)/g,
    (_, alt: string, src: string) =>
      `<img src="${escapeHtml(resolveImage(src.replace(/&amp;/g, '&')))}" alt="${alt}" loading="lazy">`,
  )
  html = html.replace(/\[([^\]]+)\]\(([^)\s]+)\)/g, (_, label: string, href: string) => {
    const external = /^https?:\/\//.test(href)
    return `<a href="${href}"${external ? ' target="_blank" rel="noopener"' : ''}>${label}</a>`
  })
  html = html
    .replace(/\*\*(.+?)\*\*/g, '<strong>$1</strong>')
    .replace(/(^|[^\w*])\*(?!\s)(.+?)\*(?!\w)/g, '$1<em>$2</em>')
  return html
    .replace(/\u0000(\d+)\u0000/g, (_, i: string) => `<code>${codes[Number(i)]}</code>`)
    .replace(/\u0001/g, '<br>')
}

/** Joins lines into one, with a line break after any line that ends with \ */
function joinLines(lines: string[]) {
  return lines.reduce((text, line) =>
    text.endsWith('\\') ? `${text.slice(0, -1).trimEnd()}\u0001${line}` : `${text} ${line}`,
  )
}

const FENCE = /^```(\w*)\s*$/
const HEADING = /^(#{1,4})\s+(.+?)\s*#*\s*$/
const RULE = /^(-{3,}|\*{3,})\s*$/
const LIST_ITEM = /^(\s*)([-*]|\d+\.)\s+(.*)$/
const TABLE_DIVIDER = /^\s*\|?\s*:?-+:?\s*(\|\s*:?-+:?\s*)*\|?\s*$/
const CALLOUT = /^\[!(NOTE|TIP|WARNING)\]\s*$/i

const CALLOUT_TITLES: Record<string, string> = { note: 'Note', tip: 'Tip', warning: 'Warning' }

function startsBlock(lines: string[], i: number) {
  const line = lines[i]!
  return (
    FENCE.test(line) ||
    HEADING.test(line) ||
    RULE.test(line) ||
    LIST_ITEM.test(line) ||
    line.startsWith('>') ||
    (line.includes('|') && TABLE_DIVIDER.test(lines[i + 1] ?? ''))
  )
}

const tableCells = (line: string) =>
  line
    .trim()
    .replace(/^\|/, '')
    .replace(/\|$/, '')
    .split('|')
    .map((cell) => cell.trim())

interface ListItem {
  indent: number
  ordered: boolean
  /** For numbered items, the number written, so a list split by a code block carries on */
  number: number
  text: string
}

function renderList(
  items: ListItem[],
  start: number,
  resolveImage: ResolveImage,
): [string, number] {
  const indent = items[start]!.indent
  const first = items[start]!
  const tag = first.ordered ? 'ol' : 'ul'
  let html = first.ordered && first.number !== 1 ? `<ol start="${first.number}">` : `<${tag}>`
  let i = start
  while (i < items.length && items[i]!.indent >= indent) {
    html += `<li>${inline(items[i]!.text, resolveImage)}`
    i++
    if (i < items.length && items[i]!.indent > indent) {
      const [nested, next] = renderList(items, i, resolveImage)
      html += nested
      i = next
    }
    html += '</li>'
  }
  return [`${html}</${tag}>`, i]
}

export function renderMarkdown(
  markdown: string,
  resolveImage: ResolveImage = (src) => src,
): RenderedMarkdown {
  const headings: GuideHeading[] = []
  const usedIds = new Set<string>()

  function renderBlocks(lines: string[]): string {
    const out: string[] = []
    let i = 0
    while (i < lines.length) {
      const line = lines[i]!
      if (!line.trim()) {
        i++
        continue
      }

      // ``` code block
      const fence = line.match(FENCE)
      if (fence) {
        const code: string[] = []
        i++
        while (i < lines.length && !/^```\s*$/.test(lines[i]!)) code.push(lines[i++]!)
        i++
        const language = fence[1] ? ` class="language-${fence[1]}"` : ''
        out.push(`<pre><code${language}>${escapeHtml(code.join('\n'))}</code></pre>`)
        continue
      }

      const heading = line.match(HEADING)
      if (heading) {
        const level = heading[1]!.length
        const text = heading[2]!
        let id = slugify(text) || 'section'
        for (let n = 2; usedIds.has(id); n++) id = `${slugify(text)}-${n}`
        usedIds.add(id)
        headings.push({ id, text: text.replace(/[`*_]/g, ''), level })
        out.push(`<h${level} id="${id}">${inline(text, resolveImage)}</h${level}>`)
        i++
        continue
      }

      if (RULE.test(line)) {
        out.push('<hr>')
        i++
        continue
      }

      // > quote, or a callout when it starts with [!NOTE], [!TIP] or [!WARNING]
      if (line.startsWith('>')) {
        const quoted: string[] = []
        while (i < lines.length && lines[i]!.startsWith('>')) {
          quoted.push(lines[i++]!.replace(/^>\s?/, ''))
        }
        const callout = quoted[0]?.match(CALLOUT)
        if (callout) {
          const kind = callout[1]!.toLowerCase()
          const body = renderBlocks(quoted.slice(1))
          out.push(
            `<aside class="callout callout-${kind}"><p class="callout-title">${CALLOUT_TITLES[kind]}</p>${body}</aside>`,
          )
        } else {
          out.push(`<blockquote>${renderBlocks(quoted)}</blockquote>`)
        }
        continue
      }

      // | table |, with a |---| line under the header
      if (line.includes('|') && TABLE_DIVIDER.test(lines[i + 1] ?? '')) {
        const header = tableCells(line)
        i += 2
        const rows: string[][] = []
        while (i < lines.length && lines[i]!.includes('|') && lines[i]!.trim()) {
          rows.push(tableCells(lines[i++]!))
        }
        const cell = (tag: string, text: string) => `<${tag}>${inline(text, resolveImage)}</${tag}>`
        out.push(
          '<div class="table-wrap"><table><thead><tr>' +
            header.map((h) => cell('th', h)).join('') +
            '</tr></thead><tbody>' +
            rows.map((r) => `<tr>${r.map((c) => cell('td', c)).join('')}</tr>`).join('') +
            '</tbody></table></div>',
        )
        continue
      }

      // - and 1. lists. Indented lines that aren't items continue the item above
      if (LIST_ITEM.test(line)) {
        const items: ListItem[] = []
        while (i < lines.length && lines[i]!.trim()) {
          const item = lines[i]!.match(LIST_ITEM)
          if (item) {
            items.push({
              indent: item[1]!.length,
              ordered: /\d/.test(item[2]!),
              number: parseInt(item[2]!, 10) || 1,
              text: item[3]!,
            })
          } else if (/^\s+/.test(lines[i]!) && items.length) {
            const item = items[items.length - 1]!
            item.text = joinLines([item.text, lines[i]!.trim()])
          } else break
          i++
        }
        let html = ''
        for (let start = 0; start < items.length;) {
          const [list, next] = renderList(items, start, resolveImage)
          html += list
          start = next
        }
        out.push(html)
        continue
      }

      // Paragraph: lines up to a blank line or the start of another block
      const paragraph: string[] = [line.trim()]
      i++
      while (i < lines.length && lines[i]!.trim() && !startsBlock(lines, i)) {
        paragraph.push(lines[i++]!.trim())
      }
      out.push(`<p>${inline(joinLines(paragraph), resolveImage)}</p>`)
    }
    return out.join('\n')
  }

  const html = renderBlocks(markdown.replace(/\r\n?/g, '\n').split('\n'))
  return { html, headings }
}

import type { FaceExpressionData, PanelBitmap } from './faces'

// Animated expressions (FaceAnimation in the firmware's Protogen_Faces.h).
// Frame 1 is the expression itself. Later frames replace some of its parts; the parts a frame
// doesn't replace follow the expression, so editing frame 1 updates them too. The animation
// loops, and animated expressions don't blink.

export type EyeBitmaps = [PanelBitmap, PanelBitmap]
export type MouthBitmaps = [PanelBitmap, PanelBitmap, PanelBitmap, PanelBitmap]

export interface AnimationFrame {
  durationMs: number
  /** Parts this frame replaces. null uses the expression's own */
  eye: EyeBitmaps | null
  mouth: MouthBitmaps | null
  nose: PanelBitmap | null
}

export const DEFAULT_FRAME_MS = 200
/** The firmware stores durations as an unsigned int, which is 16 bits on the Arduino */
export const MAX_FRAME_MS = 65535

export function isValidDuration(ms: number) {
  return Number.isInteger(ms) && ms >= 1 && ms <= MAX_FRAME_MS
}

export function emptyFrame(durationMs = DEFAULT_FRAME_MS): AnimationFrame {
  return { durationMs, eye: null, mouth: null, nose: null }
}

const copy = <T>(value: T): T => JSON.parse(JSON.stringify(value))
const same = (a: unknown, b: unknown) => JSON.stringify(a) === JSON.stringify(b)

/**
 * What a frame shows, as a face. It reads the expression and frame live, so it stays up to date,
 * and drawing on a part the frame replaces changes the frame. Frame 0 is the expression itself.
 */
export function frameView(face: FaceExpressionData, index: number): FaceExpressionData {
  const frame = index > 0 ? face.animation?.[index] : undefined
  if (!frame) return face
  return {
    get nose() {
      return frame.nose ?? face.nose
    },
    get eye() {
      return frame.eye ?? face.eye
    },
    eyeBlink: null,
    get mouth() {
      return frame.mouth ?? face.mouth
    },
    animation: null,
  }
}

/** Give a frame its own copy of each part before it's drawn on, so the expression doesn't change */
export function detachFrame(face: FaceExpressionData, index: number) {
  const frame = index > 0 ? face.animation?.[index] : undefined
  if (!frame) return
  frame.nose ??= copy(face.nose)
  frame.eye ??= copy(face.eye)
  frame.mouth ??= copy(face.mouth)
}

/** Stop frames replacing parts with identical copies, so those parts follow the expression again */
export function pruneFrames(face: FaceExpressionData) {
  for (const frame of face.animation?.slice(1) ?? []) {
    if (frame.nose && same(frame.nose, face.nose)) frame.nose = null
    if (frame.eye && same(frame.eye, face.eye)) frame.eye = null
    if (frame.mouth && same(frame.mouth, face.mouth)) frame.mouth = null
  }
}

/** A durations the firmware can store: whole milliseconds, 1 to 65535 */
export function storableDuration(ms: number) {
  return isValidDuration(ms)
    ? ms
    : Math.min(MAX_FRAME_MS, Math.max(1, Math.round(ms) || DEFAULT_FRAME_MS))
}

/** The frame showing at this time, like ExpressionManager::GetAnimationFrame() */
export function frameIndexAt(frames: AnimationFrame[], timeMs: number): number {
  const durations = frames.map((f) => storableDuration(f.durationMs))
  const total = durations.reduce((sum, d) => sum + d, 0)
  let timeInLoop = timeMs % total
  for (let i = 0; i < durations.length; i++) {
    if (timeInLoop < durations[i]!) return i
    timeInLoop -= durations[i]!
  }
  return durations.length - 1
}

/**
 * Makes frame 1 the expression itself, as the editor expects, for files where the first frame
 * replaces parts: those parts move into the expression, and later frames that didn't replace
 * them keep showing the old ones. What the animation shows doesn't change.
 */
export function normaliseFirstFrame(face: FaceExpressionData) {
  const frames = face.animation
  const first = frames?.[0]
  if (!frames || !first) return
  for (const part of ['nose', 'eye', 'mouth'] as const) {
    const replacement = first[part]
    if (!replacement) continue
    const old = face[part]
    for (const frame of frames.slice(1)) {
      if (!frame[part]) (frame as Record<typeof part, unknown>)[part] = copy(old)
    }
    ;(face as Record<typeof part, unknown>)[part] = replacement
    first[part] = null
  }
  pruneFrames(face)
}

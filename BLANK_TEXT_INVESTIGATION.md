# BBC Blank-Text Investigation — Handoff Note (paused)

## Status: UNSOLVED, but deterministically reproducible and heavily characterized.

## Deterministic repro (the key asset)
- /tmp/bbc_nojs.html  = bbc.co.uk/news server HTML with all <script> stripped, inline CSS intact.
  Reproduces blank text identically to live BBC, offline, no JS, no timing/network noise.
- /tmp/bbc_nocss.html = same with ALL CSS removed -> text VISIBLE (proves CSS-triggered).
- Regenerate: curl -sL -A "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_6_8) AppleWebKit/610.1.15" https://www.bbc.co.uk/news -o snap.html ; strip <script> blocks.

## The two URLs are DIFFERENT BUILDS (not a redirect race; SL does not redirect)
- bbc.co.uk/news : 1.22MB, 1 inline style, 9 <img> + 9 <picture>, html class="no-js" -> TEXT BLANK, images show.
- bbc.com/news   : 424KB, 2 inline styles, 1 <img>, 0 <picture>, no no-js class -> TEXT SHOWS, images blank.
- .com text rendering is a WORKING counter-example.

## RULED OUT via ground-truth (getComputedStyle / getBoundingClientRect / elementFromPoint on the blank page):
- Color: real content text computes BLACK rgb(0,0,0) / links blue rgb(0,0,238) on transparent/white bg. Correct, should be visible.
- opacity=1, visibility=visible, display=block. Not hidden.
- clip=auto, clip-path=none, transform=none. Not clipped/transformed.
- Geometry SANE: elements have real rects (e.g. 1280x64, 132x60), on-screen; body scrollHeight=7102 (full layout).
- elementFromPoint at headline locations returns the TEXT element itself (PromoLink "Instagram"/"Live. England"), on top, NOT covered by any overlay.
- prefers-color-scheme: 0 uses; darkMatch=false. Not a dark-mode theme issue.
- Web fonts: WOFF2 (BBC Reith) renders VISIBLY in a minimal local test. Not the cause.
- Local minimal pages (black text, colored text, white-on-blue, opacity+z-index layers): ALL render correctly. Primitives work.
- Property neutralization on bbc_nojs.html (removed ALL of each, text stayed blank): clip-path, transform, opacity, filter, mix-blend-mode, visibility, overflow. None restored text.
- Force override `* { color:#000!important; opacity:1!important; visibility:visible!important; -webkit-text-fill-color:#000!important }` : text STILL blank -> NOT hidden-by-color/opacity/visibility.
- Ancestor chain of a blank link: only position:relative + one z-index:1 (NAV). No will-change/transform/opacity/isolation/contain/blend compositing trigger.
- CSS bisection (both rule-split and char-range split at } boundaries): BOTH halves blank -> culprit is a PERVASIVE repeated property (styled-components emits many near-identical rules).

## CONCLUSION / remaining hypothesis
Text is correctly colored (black), correctly sized, on-screen, on top, uncovered, unclipped, opaque, visible per computed style -- but is NOT rasterized. This is a PAINT-EXECUTION failure specific to bbc.co.uk's styled-components CSS at scale. DOM/computed-style inspection is exhausted; the cause lives in WebKit paint execution that JS cannot observe.

## NEXT SESSION: instrument the WebKit paint path against /tmp/bbc_nojs.html (now reliable/deterministic).
Earlier paint instrumentation found (live BBC, was noisy): glyphs reach FontCascadeCocoa showGlyphsWithAdvances with HEALTHY matrices, m_impl=0 (direct, not display-list), fonts ready, RenderLineBoxList::paint anyIntersect=1, InlineFlowBox::paint intersect=1 for real boxes, child loop painted text children -- but InlineTextBox::paint instrumentation inconsistently fired (run-to-run). Re-run all of that against the DETERMINISTIC repro to remove the inconsistency. Focus: why on-screen, correctly-styled InlineTextBox content does not rasterize when the surrounding styled-components CSS is present.

## SOLID WINS THIS SESSION (committed, tree clean):
- sources_610 sl-port-610: 82d6725071 (39, WOFF2+brotli vendor), 4b11222627 (40, MediaAccessibility crash), e68eb77741 (41, NSGraphicsContext 10.6 API swaps). All verified working.
- NOTE: LEOPARD_WEBKIT macro is DEAD (never defined as compiler flag). Use __MAC_OS_X_VERSION_MIN_REQUIRED < 1070 for 10.6 guards. sRGBColorSpaceRef() returns VALID (non-null) on 10.6 -- not a null-colorspace bug.

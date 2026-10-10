# Portfolio — Todo

## Done
- [x] Hero: profile picture displayed on the side (`profile_picture.jpg`, responsive, stacks above text under 860px).
- [x] Typing effect for the hero sentences (lede → heading → sub, blinking caret; skipped for reduced-motion).
- [x] New projects: ERP for seeCar, ERP for Tapai Ko Bazar, E-commerce for Baltra (framed as software businesses use to run daily operations).
- [x] Codesc Nepal responsibilities added: requirement analysis/gathering, tech support, on-site client visits, client meetings, client collaboration, iOS & Android deployment, App Store Connect & Google Play Console management, user/access control in App Store and Play Store apps.
- [x] Sprite (Pip) moved to the bottom-left; speech bubble tail adjusted for the left side.
- [x] Messaging icon (FAB) added bottom-right; opens a chatbot panel.
- [x] Chatbot panel with `window.PipChat.send` hook.
- [x] Header made 100% opaque (`background:var(--bg)` instead of 78% color-mix).
- [x] Parrot + speech bubble shrunk (bird 150→104px, bubble 270→200px wide; mobile 92→74px) so it blocks less area.
- [x] Chatbot is now a pure-JS FAQ bot (skills, each project, Codesc responsibilities, education, contact, fallback answer) — no server, no model, safe & free on GitHub Pages. LLM integration can return later by overriding `window.PipChat.send`.
- [x] Added `.github/workflows/deploy.yml` — deploys `web/` to GitHub Pages on push to main/master.


- [x] Header bug fixed: sticky nav was being painted over by `main` (same z-index, later in DOM) → looked transparent and blocked clicks. `.nav` now `z-index:100` + solid `var(--bg)`, no backdrop-filter.
- [x] Contact section: mailto form only (Name, Email Address, Subject, Submit) → opens visitor's mail app pre-filled to pratikmaharjan323@gmail.com.
- [x] Social media row added just above the profile picture: GitHub, GitLab, LinkedIn, Facebook.
- [x] Dark mode refined: calm slate + soft blue accent (was murky green); no longer auto-activates from system preference — light by default, dark only via toggle.
- [x] Typing effect on scroll: every section heading, paragraph and responsibility bullet types out when it enters the viewport (IntersectionObserver, skipped for reduced-motion).
- [x] Parrot now narrates each section **in order** (intro → Project 1 → 2 → … → 6), waiting 4s after each sentence finishes typing; only advances while the visitor stays in the section; re-entering restarts from the item at screen middle (e.g., scroll to Project 2, leave, come back → starts at Project 2 → n).
- [x] Skills split: **Tech skills** only, with "Main skill: Mobile App Development" banner; new **Other skills** section (client communication, requirement gathering, meetings, on-site visits, tech support, deployment, App Store Connect / Play Console, access control, Agile/Scrum, QA, spoken languages).
- [x] Parrot voice: Web Speech API (`speechSynthesis`) speaks every `say()` line — English voice preferred, rate 1.05, pitch 1.3; narration waits for speech to finish (plus 4s) before the next item, with stale-event guard and 30s stall cap.
- [x] Hero: the 3 sentences now type in parallel (350ms head-start stagger).
- [x] Scroll-reveal animations: project cards tilt-up 3D (staggered), timeline slides in from left with rotateY + dot pop ring, chips pop-in (staggered, WASM takes over after), contact/headings fade-up.
- [x] Skills prettified: uppercase accent group titles, chips hover-tint, "Spoken languages" chip bold + accent colored.
- [x] Footer: 3-part layout — copyright, "Crafted in Lalitpur…" note, live Nepal date/time clock (Asia/Kathmandu, updates every second).
- [x] Mermaid background swimmer: new `web/mermaid.js` + layer in `index.html` + CSS block; 5 poses from `web/secondary_asset/` (paths adjusted — assets were in `secondary_asset/`, not `mermaid/`), steering wander with spring smoothing, S-glide + bob, rest moments, 3D tilt, delayed facing flip, pose-by-velocity with 600ms hysteresis, depth breathing, transparent (0.5 light / 0.42 dark), pixelated rendering, transform-only RAF, pauses when hidden, static pose under reduced-motion. Loaded before app.js; nothing existing touched.
- [x] Skills sections redesigned: card-based layout (`.skill-card` with surface bg, accent-dot titles, compact chips); main-skill banner now a left-accent bar with a "MAIN SKILL" tag; Other skills grouped into 4 cards (Client work / Releases and stores / Process and quality / Spoken languages as 4 bold chips).

## Next
- [ ] Create the GitHub repo, push this folder, and set Pages source to "GitHub Actions" (Settings → Pages).
- [ ] Optional later: override `window.PipChat.send` with a hosted/official LLM endpoint if ever needed.

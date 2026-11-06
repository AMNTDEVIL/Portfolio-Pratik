
(() => {
"use strict";
const $ = (s) => document.querySelector(s);
const reduce = matchMedia("(prefers-reduced-motion: reduce)").matches;
const X = 65536;                       // 16.16 fixed point
const fx = (v) => Math.round(v * X);   // JS number -> 16.16
$("#yr").textContent = new Date().getFullYear();

/* ---------- Footer clock (Nepal time) ---------- */
const clock = $("#clock");
const tickClock = () => {
  const d = new Date(), o = { timeZone: "Asia/Kathmandu" };
  clock.textContent =
    d.toLocaleDateString("en-GB", { ...o, weekday: "short", day: "numeric", month: "short", year: "numeric" }) +
    " · " + d.toLocaleTimeString("en-GB", { ...o, hour: "2-digit", minute: "2-digit", second: "2-digit" }) + " NPT";
};
tickClock(); setInterval(tickClock, 1000);

/* ---------- Parrot voice (Web Speech API) ---------- */
const synth = window.speechSynthesis;
let voice = null, speechDone = true, speechId = 0;   // speechDone gates the narration sequence
function pickVoice() {
  const voices = synth.getVoices();
  voice = voices.find(v => v.lang.startsWith("en") && /female|google|natural/i.test(v.name))
       || voices.find(v => v.lang.startsWith("en")) || null;
}
if (synth) { pickVoice(); synth.addEventListener("voiceschanged", pickVoice); }
function speak(text) {
  if (!synth) return;
  synth.cancel();                          // stop anything still playing
  const id = ++speechId;                   // ignore late events from the cancelled utterance
  const u = new SpeechSynthesisUtterance(text);
  if (voice) u.voice = voice;
  u.rate = 1.05;                           // 0.1 to 10
  u.pitch = 1.3;                           // higher pitch suits a parrot
  speechDone = false;
  u.onend = u.onerror = () => { if (id === speechId) speechDone = true; };
  synth.speak(u);
}

/* ---------- Theme (always light unless explicitly toggled) ---------- */
const root = document.documentElement;
const isDark = () => root.dataset.theme === "dark";
$("#theme").addEventListener("click", () => {
  const next = isDark() ? "light" : "dark";
  root.dataset.theme = next;
  try { localStorage.setItem("theme", next); } catch (_) {}
});

/* ---------- Sprite + scroll guide (sheet rows: idle / wave / talk / think) ---------- */
const sprite = $("#sprite"), bubble = $("#bubble"), bird = $("#bird");
const FPS = [5, 9, 8, 7];
let state = 0, frame = 0, last = 0, typeTimer = 0, current = null, guideWait = 0;
function drawFrame() {
  sprite.style.backgroundPosition = `${(frame / 5) * 100}% ${(state / 3) * 100}%`;
}
function setState(i) { state = i; frame = 0; drawFrame(); }
function spriteTick(now) {
  if (!reduce && now - last > 1000 / FPS[state]) { last = now; frame = (frame + 1) % 6; drawFrame(); }
}
function say(text, mood) {
  clearInterval(typeTimer);
  bird.setAttribute("aria-label", "Pip says: " + text + " Click to hear it again.");
  speak(text);
  if (reduce) { bubble.textContent = text; setState(0); return; }
  setState(mood); bubble.textContent = "";
  let i = 0;
  typeTimer = setInterval(() => {
    bubble.textContent = text.slice(0, ++i);
    if (i >= text.length) { clearInterval(typeTimer); setTimeout(() => setState(0), 1200); }
  }, 22);
}
/* Each section is a sequence: the parrot narrates its [data-say] items in order
   (Project 1 → 2 → … → n), waiting 4s after each sentence finishes typing, and
   only while the visitor stays in that section. Re-entering a section restarts
   from whichever item sits at the middle of the screen. */
const seqs = [...document.querySelectorAll("main > section")].map((sec) => ({ sec, items: [sec, ...sec.querySelectorAll("[data-say]")] }));
let curSeq = null, curIdx = -1, seqTimer = 0;
const midY = () => innerHeight * 0.45;
function middleIndex(items) {
  const y = midY();
  let best = 0, bestD = Infinity;
  for (let i = 0; i < items.length; i++) {
    const r = items[i].getBoundingClientRect();
    if (r.top <= y && r.bottom >= y) return i;
    const d = r.top > y ? r.top - y : y - r.bottom;
    if (d < bestD) { bestD = d; best = i; }
  }
  return best;
}
const typeTime = (el) => (el.dataset.say.length + 2) * 22;   // matches say()'s 22ms/char
function nextStep(delay) {                                    // wait for typing+4s AND for speech to end
  const deadline = performance.now() + delay + 30000;         // hard cap so a missing onend can't stall it
  seqTimer = setTimeout(function wait() {
    if ((!speechDone) && performance.now() < deadline) { seqTimer = setTimeout(wait, 500); return; }
    if (curSeq && curIdx + 1 < curSeq.items.length) { curIdx++; speakStep(); }
  }, delay);
}
function speakStep() {
  clearTimeout(seqTimer);
  const item = curSeq.items[curIdx];
  current = item;
  say(item.dataset.say, +item.dataset.mood || 2);
  nextStep(typeTime(item) + 4000);
}
function guide() {
  guideWait = 0;
  const hit = document.elementFromPoint(innerWidth / 2, midY());
  const sec = hit && hit.closest("main > section");
  if (!sec) return;
  const seq = seqs.find((s) => s.sec === sec);
  if (!seq || (curSeq && curSeq.sec === sec)) return;   // already narrating this section
  curSeq = seq;
  curIdx = middleIndex(seq.items);
  speakStep();
}
addEventListener("scroll", () => { if (!guideWait) guideWait = setTimeout(guide, 180); }, { passive: true });
bird.addEventListener("click", () => {                   // replay the current item, keep the sequence going
  if (!current) return;
  clearTimeout(seqTimer);
  say(current.dataset.say, 1);
  nextStep(typeTime(current) + 4000);
});
setState(0);
guide();

/* ---------- Typing effect for the hero sentences (all three type in parallel) ---------- */
try {
  const typeEls = [...document.querySelectorAll("[data-type]")];
  if (!reduce && typeEls.length) {
    typeEls.forEach((el, idx) => {
      const text = el.textContent;
      el.textContent = "";
      el.classList.add("typing");
      let i = 0;
      setTimeout(() => {                                   // small head-start so the lede still leads
        const t = setInterval(() => {
          el.textContent = text.slice(0, ++i);
          if (i >= text.length) { clearInterval(t); el.textContent = text; el.classList.remove("typing"); }
        }, 24);
      }, idx * 350);
    });
  }
} catch (_) {}

/* ---------- Typing effect for each section's text when it scrolls into view ---------- */
try {
  if (!reduce && "IntersectionObserver" in window) {
    const SEL = "main section:not(.hero) h2, main section:not(.hero) h3, main section:not(.hero) p, .resp li";
    const targets = [...new Set(document.querySelectorAll(SEL))];
    targets.forEach((el) => { el.dataset.rest = el.textContent; el.textContent = ""; });
    const io = new IntersectionObserver((entries, obs) => {
      for (const en of entries) {
        if (!en.isIntersecting) continue;
        obs.unobserve(en.target);
        const el = en.target, text = el.dataset.rest;
        delete el.dataset.rest;
        el.classList.add("typing");
        let i = 0;
        const t = setInterval(() => {
          el.textContent = text.slice(0, ++i);
          if (i >= text.length) { clearInterval(t); el.textContent = text; el.classList.remove("typing"); }
        }, 16);
      }
    }, { threshold: 0.2 });
    targets.forEach((el) => io.observe(el));
  }
} catch (_) {}

/* ---------- Scroll-reveal animations (entrance effects per section) ----------
   CSS animations override the WASM inline transform while they run, then the
   WASM layer takes the elements over again — a nice hand-off. */
try {
  if ("IntersectionObserver" in window) {
    const groups = [
      { sel: ".grid .card",           anim: "tilt",  step: 110 },
      { sel: ".timeline li",          anim: "slide", step: 170 },
      { sel: ".chips li",             anim: "pop",   step: 45  },
      { sel: "main section h2, .sub-h, .main-skill, #contact .sub, .mailform, .plain, .skillgrid h3", anim: "fade", step: 0 }
    ];
    const all = [];
    for (const g of groups) {
      [...document.querySelectorAll(g.sel)].forEach((el, i) => {
        el.classList.add("reveal");
        el.dataset.anim = g.anim;
        if (g.step && !reduce) el.style.animationDelay = (i * g.step) + "ms";
        all.push(el);
      });
    }
    if (reduce) {
      all.forEach((el) => el.classList.add("in"));
    } else {
      const ro = new IntersectionObserver((entries, obs) => {
        for (const en of entries) {
          if (!en.isIntersecting) continue;
          obs.unobserve(en.target);
          en.target.classList.add("in");
        }
      }, { threshold: 0.15 });
      all.forEach((el) => ro.observe(el));
    }
  }
} catch (_) {}

/* ---------- Contact: EmailJS form (sends direct email, no mail app) ---------- */
/* Configure: emailjs.init("YOUR_PUBLIC_KEY") and set SERVICE_ID below */
emailjs.init("YOUR_PUBLIC_KEY");

const SERVICE_ID = "YOUR_SERVICE_ID";
const CONTACT_TEMPLATE_ID = "template_1hevg0c";   // "Contact Us" template
const AUTO_REPLY_TEMPLATE_ID = "template_zb9xwa8"; // "Auto-Reply" template

const mailForm = $("#mailForm");
if (mailForm) mailForm.addEventListener("submit", async (e) => {
  e.preventDefault();
  const btn = mailForm.querySelector("button[type=submit]");
  const originalText = btn.textContent;
  btn.textContent = "Sending…";
  btn.disabled = true;

  const templateParams = {
    from_name: $("#mName").value.trim(),
    from_email: $("#mEmail").value.trim(),
    subject: $("#mSubject").value.trim(),
    message: $("#mMessage").value.trim(),
    reply_to: $("#mEmail").value.trim(),
  };

  try {
    // 1) Send notification to you (pratikmaharjan323@gmail.com)
    await emailjs.send(SERVICE_ID, CONTACT_TEMPLATE_ID, templateParams);

    // 2) Auto-reply to the visitor
    await emailjs.send(SERVICE_ID, AUTO_REPLY_TEMPLATE_ID, templateParams);

    btn.textContent = "Sent!";
    btn.style.background = "var(--accent)";
    mailForm.reset();
    setTimeout(() => {
      btn.textContent = originalText;
      btn.disabled = false;
      btn.style.background = "";
    }, 3000);
  } catch (err) {
    console.error(err);
    btn.textContent = "Failed — try again";
    btn.disabled = false;
  }
});

/* ---------- Chatbot panel (FAQ bot inside; swap window.PipChat.send for an LLM later) ---------- */
const chat = $("#chat"), chatLog = $("#chatLog"), chatForm = $("#chatForm"),
      chatInput = $("#chatInput"), chatOpen = $("#chatOpen"), chatClose = $("#chatClose");

function addMsg(text, who) {
  const m = document.createElement("div");
  m.className = "chat-msg " + who;
  m.textContent = text;
  chatLog.appendChild(m);
  chatLog.scrollTop = chatLog.scrollHeight;
  return m;
}

/* ---------- FAQ bot ----------
   Pure-JS keyword matching against a small FAQ list — no server, no model,
   safe to host on GitHub Pages. Want a real LLM later? Just override
   window.PipChat.send again. */
const FAQ = [
  { keys: ["hi", "hello", "hey", "namaste", "hola", "greetings"],
    answer: "Hello! I'm Pip, Pratik's guide. Ask me about his skills, projects, experience, education or how to contact him." },
  { keys: ["who", "introduce", "pratik", "yourself"],
    answer: "Pratik Maharjan is a Flutter developer in Lalitpur, Nepal, who builds Android and iOS apps that businesses rely on every day — and he's growing into backend work with Spring Boot, media pipelines and C through FFI." },
  { keys: ["skill", "stack", "technology", "technologies", "tools", "know"],
    answer: "Main skill: Mobile App Development — Flutter apps for Android and iOS. Also Spring Boot, REST APIs, RabbitMQ, FFmpeg, Backblaze B2, MySQL, PostgreSQL, Firebase, C, JavaScript, PHP, Git and Docker." },
  { keys: ["flutter", "dart"],
    answer: "Flutter and Dart are Pratik's main tools — every app on this page (StackFood, the video platform, and the ERP projects) is built with them for both Android and iOS." },
  { keys: ["backend", "spring", "java", "rest", "api"],
    answer: "On the backend Pratik works with Spring Boot and REST APIs, plus RabbitMQ for job queues and FFmpeg for video transcoding — all part of the streaming platform he built." },
  { keys: ["project", "built", "portfolio", "work has"],
    answer: "Six projects: the StackFood delivery app, a full video streaming platform, the BYD dealer ERP, ERP for seeCar, ERP for Tapai Ko Bazar, and an e-commerce app for Baltra. Scroll to the Projects section to see them all!" },
  { keys: ["stackfood", "food", "delivery"],
    answer: "StackFood is a food delivery app with login, Google Maps tracking, restaurant opening hours, automated chat replies, restaurant sign-up and rider applications. Built with Flutter and Firebase." },
  { keys: ["video", "stream", "hls", "transcode"],
    answer: "The video platform does secure uploads and adaptive HLS playback: a RabbitMQ pipeline transcodes to 360p/480p/720p with thumbnails, files sit in private Backblaze B2 behind signed URLs, and an admin login is hardware-locked in C via FFI." },
  { keys: ["byd", "dealer"],
    answer: "The BYD dealer ERP is a cross-platform app used daily by 100+ employees of BYD dealers across Nepal (CIMEX, EcoDrive, Plugin Motors) for sales, service and operations. Pratik leads it." },
  { keys: ["seecar", "see car"],
    answer: "ERP for seeCar is business software for daily operations — sales, inventory, service records and reporting — so staff can rely on it on both Android and iOS." },
  { keys: ["tapai", "bazar", "market", "grocery"],
    answer: "ERP for Tapai Ko Bazar helps the store run its business: stock and inventory, billing, orders and reports in one place, so the team can serve customers without juggling spreadsheets." },
  { keys: ["baltra", "e-commerce", "ecommerce", "shop", "online"],
    answer: "The Baltra e-commerce app lets the business sell online: product catalog, cart and checkout, order tracking and admin management — shipped to both the App Store and Play Store." },
  { keys: ["erp", "business"],
    answer: "All of Pratik's ERP projects (BYD dealers, seeCar, Tapai Ko Bazar) are apps businesses use to perform their day-to-day business — sales, inventory, billing and reports." },
  { keys: ["experience", "job", "company", "codesc", "intern", "bluefox", "worked"],
    answer: "Currently a Junior Flutter Developer at Codesc Nepal (Nov 2025 – present), leading the BYD ERP app. Before that, a Flutter Intern at Bluefox (Dec 2024 – Apr 2025), working on StackFood." },
  { keys: ["responsibilit", "role", "duties", "day-to-day"],
    answer: "At Codesc Nepal: requirement analysis and gathering, tech support, on-site client visits, client meetings and collaboration, iOS & Android deployment, App Store Connect and Google Play Console management, and user/access control in the store apps." },
  { keys: ["client", "requirement", "visit", "deploy", "play console", "app store"],
    answer: "Pratik works directly with clients: gathering requirements, tech support, on-site visits and meetings, then ships to iOS/Android and manages App Store Connect, Google Play Console and app user/access control." },
  { keys: ["education", "study", "degree", "college", "bachelor", "school", "training", "workshop"],
    answer: "Bachelor in Computer Application (Neseld International College, 2020–2025) and +2 Management (DAV, 2018–2020), plus workshops in Software QA & Testing (Jest) and Agile/Scrum." },
  { keys: ["contact", "email", "hire", "reach", "linkedin", "github", "call"],
    answer: "Fastest way to reach Pratik: pratikmaharjan323@gmail.com. He's also on GitHub (AMNTDEVIL) and LinkedIn (pratikmaharjan323) — links are in the Contact section." },
  { keys: ["where", "location", "based", "live", "lalitpur", "kathmandu", "nepal"],
    answer: "Pratik is based in Lalitpur, Nepal, and works with clients across Nepal (his Codesc Nepal office is in Kathmandu)." },
  { keys: ["speak", "language", "nepali", "hindi", "newari", "english"],
    answer: "Pratik speaks Nepali, English, Hindi and Newari." },
  { keys: ["wasm", "webassembly", "engine", "motion", "particle", "ffi"],
    answer: "The moving particles and cursor trail on this page run in a C engine compiled to WebAssembly — Pratik wrote it and calls it from JavaScript. He also uses C for hardware-locked logins via FFI." },
  { keys: ["price", "cost", "rate", "salary", "charge", "pay"],
    answer: "For rates and hiring details, please email pratikmaharjan323@gmail.com directly — he'll get back to you." },
  { keys: ["thanks", "thank", "great", "nice", "cool", "awesome"],
    answer: "Thanks! Glad you like it. Scroll on — Pip will keep showing you around. 🦜" }
];

window.PipChat = {
  send: async (msg) => {
    const q = " " + msg.toLowerCase().replace(/[^a-z0-9\s-]/g, " ") + " ";
    let best = null, bestScore = 0;
    for (const item of FAQ) {                       // longer keywords weigh more
      let score = 0;
      for (const k of item.keys) if (q.includes(k)) score += k.length;
      if (score > bestScore) { bestScore = score; best = item; }
    }
    return best ? best.answer
      : "I'm just a small FAQ bot, so I might have missed that. Try asking about Pratik's skills, projects, experience, education or contact — or email pratikmaharjan323@gmail.com directly.";
  }
};

function openChat(open) {
  chat.hidden = !open;
  chatOpen.setAttribute("aria-expanded", String(open));
  if (open) {
    if (!chatLog.children.length) addMsg("Hi! Ask me anything about Pratik's skills, projects, experience or contact.", "bot");
    chatInput.focus();
  }
}
chatOpen.addEventListener("click", () => openChat(chat.hidden));
chatClose.addEventListener("click", () => openChat(false));
chatForm.addEventListener("submit", async (e) => {
  e.preventDefault();
  const text = chatInput.value.trim();
  if (!text) return;
  chatInput.value = "";
  addMsg(text, "user");
  const wait = addMsg("Thinking…", "bot thinking");
  try {
    wait.textContent = await window.PipChat.send(text);
    wait.classList.remove("thinking");
  } catch (_) {
    wait.textContent = "Sorry, something glitched — please try again.";
    wait.classList.remove("thinking");
  }
  chatLog.scrollTop = chatLog.scrollHeight;
});

/* ---------- Canvas + pointer ---------- */
const cv = $("#fx"), ctx = cv.getContext("2d");
let W = 0, H = 0, dpr = 1, px = 0.5, py = 0.35, mx = -100, my = -100, scroll01 = 0;
const touch = matchMedia("(pointer: coarse)").matches;
addEventListener("pointermove", (e) => {
  mx = e.clientX; my = e.clientY; px = mx / W; py = my / H;
}, { passive: true });
addEventListener("scroll", () => {
  const m = document.documentElement.scrollHeight - innerHeight;
  scroll01 = m > 0 ? scrollY / m : 0;
}, { passive: true });

/* ---------- WASM bridge ---------- */
const status = $("#status");
const fxEls = [...document.querySelectorAll("[data-fx]")];
let M = null;

function sizeCanvas(resizeWasm) {
  W = innerWidth; H = innerHeight; dpr = Math.min(devicePixelRatio || 1, 2);
  cv.width = W * dpr; cv.height = H * dpr;
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  if (resizeWasm && M) M._portfoliofx_resize(W, H, dpr);
}

function initWasm(mod) {
  M = mod;
  sizeCanvas(false);
  const seed = (Math.random() * 2 ** 31) | 0;
  M._portfoliofx_init(W, H, dpr, seed);
  M._portfoliofx_set_is_touch(touch ? 1 : 0);
  M._domanim_init(fxEls.length, seed);
  M._unified_sim_init(W, H, seed);
  addEventListener("resize", () => sizeCanvas(true));
  status.textContent = `WASM engine running · ${M._portfoliofx_get_particle_count()} particles`;
  status.classList.add("ok");
}

/* Read a 16.16 int32 buffer straight out of WASM memory (re-read HEAP32 each
   frame: it is replaced whenever memory grows). */
const view = (ptr, n) => M.HEAP32.subarray(ptr >> 2, (ptr >> 2) + n);

function drawParticles(dark) {
  const n = M._portfoliofx_get_particle_count(), s = M._portfoliofx_get_particles_stride_floats();
  const d = view(M._portfoliofx_get_particles_ptr(), n * s);
  for (let i = 0; i < n; i++) {
    const o = i * s;                                   // x,y,vx,vy,r,hue,a,phase,phase_speed
    const x = d[o] / X, y = d[o + 1] / X, r = d[o + 4] / X, hue = d[o + 5] / X, a = d[o + 6] / X;
    ctx.fillStyle = dark ? `hsla(${hue},70%,70%,${a * 0.5})` : `hsla(${hue - 40},55%,38%,${a * 0.22})`;
    ctx.beginPath(); ctx.arc(x, y, r, 0, 6.2832); ctx.fill();
  }
}

function drawTrail(dark) {
  const n = M._unified_sim_get_cursor_count(), s = M._unified_sim_get_cursor_stride();
  const d = view(M._unified_sim_get_cursor_ptr(), n * s);
  for (let i = 0; i < n; i++) {
    const o = i * s, life = d[o + 4] / X;
    if (life <= 0) continue;
    const size = (d[o + 5] / X) * life, hue = d[o + 6] / X - 90;
    ctx.fillStyle = dark ? `hsla(${hue},75%,65%,${life * 0.5})` : `hsla(${hue},60%,42%,${life * 0.35})`;
    ctx.beginPath(); ctx.arc(d[o] / X, d[o + 1] / X, size, 0, 6.2832); ctx.fill();
  }
}

function applyDomAnim(t, dt) {
  M._domanim_step(fx(t), fx(dt), fx(scroll01), fx(px), fx(py));
  const s = M._domanim_get_stride_floats();
  const d = view(M._domanim_get_ptr(), fxEls.length * s);
  for (let i = 0; i < fxEls.length; i++) {              // tx,ty,rot,scale,...
    const o = i * s;
    const tx = (d[o] / X) * 0.18, ty = (d[o + 1] / X) * 0.18;   // scaled down: subtle
    const rot = (d[o + 2] / X) * 0.06, sc = 1 + ((d[o + 3] / X) - 1) * 0.5;
    fxEls[i].style.transform = `translate(${tx.toFixed(2)}px,${ty.toFixed(2)}px) rotate(${rot.toFixed(3)}deg) scale(${sc.toFixed(4)})`;
  }
}

/* ---------- Main loop ---------- */
let t0 = performance.now(), prev = t0;
function frameLoop(now) {
  requestAnimationFrame(frameLoop);
  spriteTick(now);
  if (!M || reduce) return;
  const t = (now - t0) / 1000, dt = Math.min((now - prev) / 1000, 0.05);
  prev = now;

  if (!touch) M._portfoliofx_set_pointer(mx < 0 ? W / 2 : mx, my < 0 ? H * 0.35 : my);
  M._portfoliofx_step(dt);
  M._unified_sim_step(fx(dt), fx(t), fx(mx), fx(my), W, H);

  root.style.setProperty("--mx", M._portfoliofx_get_spotlight_mx().toFixed(1) + "%");
  root.style.setProperty("--my", M._portfoliofx_get_spotlight_my().toFixed(1) + "%");

  ctx.clearRect(0, 0, W, H);
  const dark = isDark();
  drawParticles(dark);
  if (!touch) drawTrail(dark);
  applyDomAnim(t, dt);
}
requestAnimationFrame(frameLoop);

/* ---------- Load module (page still works if it fails) ---------- */
if (typeof createPortfolioModule === "function") {
  createPortfolioModule().then(initWasm).catch((e) => {
    console.error(e); status.textContent = "Engine failed to load. Showing the static page."; status.classList.add("err");
  });
} else {
  status.textContent = "Engine not built yet. Run build.sh to generate portfolio.js and portfolio.wasm.";
  status.classList.add("err");
}
})();

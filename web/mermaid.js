/* ============================================================================
   MERMAID BACKGROUND SWIMMER — web/mermaid.js
   Steers a pixel-art mermaid around the viewport naturally (wander + spring
   smoothing, S-glide undulation, rest moments, pose-by-velocity, depth).
   TUNABLES:
     SCALE_BASE / SCALE_SMALL  base sprite scale (0.45 desktop / 0.3 <760px)
     SPEED_MIN / SPEED_MAX     cruise speed range (px/s), re-rolled per leg
     TURN_RATE                 max turn rate (rad/s)
     UND_AMP / UND_HZ          perpendicular wave-glide amplitude (px) / frequency (Hz)
     BOB_AMP                   slow vertical bob (px)
     ARRIVE                    waypoint arrival radius (px)
     REST_MIN / REST_MAX       seconds between rest moments
     REST_DUR                  [min,max] rest length in seconds
     TILT_MAX                  max body tilt following motion (deg)
     MIN_POSE_MS               min ms between pose changes (anti-flicker)
     FLIP_MS                   slowdown before a facing flip (ms)
   ========================================================================== */
(() => {
  "use strict";
  const SCALE_BASE = 0.45, SCALE_SMALL = 0.3;
  const SPEED_MIN = 35, SPEED_MAX = 95;
  const TURN_RATE = 1.8;
  const UND_AMP = 16, UND_HZ = 0.25;
  const BOB_AMP = 10;
  const ARRIVE = 80;
  const REST_MIN = 15, REST_MAX = 30, REST_DUR = [2, 4];
  const TILT_MAX = 20;
  const MIN_POSE_MS = 600;
  const FLIP_MS = 450;

  const layer = document.getElementById("mermaidLayer");
  const body = document.getElementById("mermaidBody");
  if (!layer || !body) return;
  const poses = [...body.querySelectorAll(".mermaid-pose")];
  if (!poses.length) return;
  const rand = (a, b) => a + Math.random() * (b - a);
  const reduce = matchMedia("(prefers-reduced-motion: reduce)").matches;

  if (reduce) {                                   // static pose 3 in a corner, no motion
    layer.classList.add("mermaid-reduce");
    (poses[2] || poses[0]).classList.add("on");
    return;
  }

  let scale = innerWidth < 760 ? SCALE_SMALL : SCALE_BASE;
  let x = innerWidth * 0.75, y = innerHeight * 0.30;
  let vx = 0, vy = 0, heading = Math.PI;         // start swimming left
  let wp = { x: innerWidth * 0.25, y: innerHeight * 0.6 };
  let cruise = rand(SPEED_MIN, SPEED_MAX);
  let flip = 1, flipUntil = 0;
  let phase = 0, bobPhase = 0, t = 0;
  let tilt = 0;
  let resting = false, restEnd = 0;
  let nextRest = performance.now() + rand(REST_MIN, REST_MAX) * 1000;
  let restPoseAlt = false;
  let pose = 1, poseSince = 0, cand = 1, candSince = 0;
  let last = 0;

  function pickWaypoint(W, H) {
    const off = Math.random() < 0.35;            // sometimes slightly off-screen
    wp = {
      x: rand(off ? -0.15 : 0.08, off ? 1.15 : 0.92) * W,
      y: rand(off ? -0.15 : 0.15, off ? 1.15 : 0.85) * H
    };
    cruise = rand(SPEED_MIN, SPEED_MAX);         // vary speed per leg
  }

  function setPose(p, now) {
    if (p === pose) { cand = p; return; }
    if (p !== cand) { cand = p; candSince = now; return; }   // must be stable first
    if (now - poseSince < MIN_POSE_MS || now - candSince < 250) return;
    pose = p; poseSince = now;
    poses.forEach((img, i) => img.classList.toggle("on", i === pose - 1));
  }

  function step(now) {
    requestAnimationFrame(step);
    if (document.hidden) { last = now; return; }              // pause when tab hidden
    const dt = Math.min((now - last) / 1000, 0.05) || 0;
    last = now;
    if (!dt) return;
    t += dt;
    const W = innerWidth, H = innerHeight;

    if (!resting && now >= nextRest) {                        // rest moments
      resting = true; restEnd = now + rand(REST_DUR[0], REST_DUR[1]) * 1000;
      restPoseAlt = !restPoseAlt;
    }
    if (resting && now >= restEnd) {
      resting = false; nextRest = now + rand(REST_MIN, REST_MAX) * 1000;
      pickWaypoint(W, H);
    }
    if (x < -W * 0.4 || x > W * 1.4 || y < -H * 0.4 || y > H * 1.4) pickWaypoint(W, H);

    let dx = wp.x - x, dy = wp.y - y, dist = Math.hypot(dx, dy);
    if (dist < ARRIVE) { pickWaypoint(W, H); dx = wp.x - x; dy = wp.y - y; }
    const want = Math.atan2(dy, dx);
    let da = want - heading;
    while (da > Math.PI) da -= 2 * Math.PI;
    while (da < -Math.PI) da += 2 * Math.PI;
    heading += Math.max(-TURN_RATE * dt, Math.min(TURN_RATE * dt, da));

    let slow = 1;                                             // flip only after a short slowdown
    const desiredFlip = Math.cos(heading) < -0.05 ? -1 : 1;   // poses 2/3 face right
    if (desiredFlip !== flip) {
      slow = 0.35;
      if (!flipUntil) flipUntil = now + FLIP_MS;
      if (now >= flipUntil) { flip = desiredFlip; flipUntil = 0; }
    } else flipUntil = 0;

    const targetSpeed = resting ? rand(3, 8) : cruise * slow;
    const k = 1 - Math.exp(-2.5 * dt);                        // spring-ish velocity smoothing
    vx += (Math.cos(heading) * targetSpeed - vx) * k;
    vy += (Math.sin(heading) * targetSpeed - vy) * k;
    x += vx * dt; y += vy * dt;

    phase += UND_HZ * 2 * Math.PI * dt;                       // S-glide + bob (render only)
    bobPhase += 0.4 * dt;
    const perp = Math.sin(phase) * UND_AMP;
    const rx = x + Math.cos(heading + Math.PI / 2) * perp;
    const ry = y + Math.sin(heading + Math.PI / 2) * perp + Math.sin(bobPhase) * BOB_AMP;

    let wantTilt = Math.max(-TILT_MAX, Math.min(TILT_MAX, (vy / Math.max(40, Math.abs(vx))) * 18));
    if (pose === 4) wantTilt = 0;                             // diving sprite is already vertical
    tilt += (wantTilt - tilt) * (1 - Math.exp(-3 * dt));

    const depth = 1 + Math.sin(t * 0.12) * 0.1;               // slow +-10% depth breathing
    const s = scale * depth;
    const fs = (pose === 2 || pose === 3) ? flip : 1;
    body.style.transform =
      `translate3d(${rx.toFixed(1)}px,${ry.toFixed(1)}px,0) rotate(${tilt.toFixed(2)}deg) scale(${(s * fs).toFixed(4)},${s.toFixed(4)})`;
    body.style.opacity = (0.82 + (depth - 0.9) * 1.8).toFixed(2);   // slightly faint when "far"

    const sp = Math.hypot(vx, vy);                            // pose by velocity + hysteresis
    let p;
    if (resting) p = restPoseAlt ? 3 : 1;
    else if (sp < 22) p = Math.floor(t / 8) % 2 ? 3 : 1;      // slow drift, alternate 1/3
    else if (vy > 40 && Math.abs(vx) < 30) p = 4;             // steep dive
    else if (vy < -45) p = 5;                                 // swimming up / away
    else p = 2;                                               // horizontal cruise
    setPose(p, now);
  }

  addEventListener("resize", () => { scale = innerWidth < 760 ? SCALE_SMALL : SCALE_BASE; }, { passive: true });
  poses[0].classList.add("on");
  last = performance.now();
  requestAnimationFrame(step);
})();

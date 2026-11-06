#!/usr/bin/env bash
set -e

PORT=8080
DIR="$(cd "$(dirname "$0")/web" && pwd)"

# ── colours ──────────────────────────────────────────────────────────────────
G="\033[1;32m" Y="\033[1;33m" C="\033[1;36m" R="\033[0m"

clear
echo ""
echo -e "${C}  ┌─────────────────────────────────────────┐${R}"
echo -e "${C}  │   Pratik Maharjan — Portfolio Dev Server │${R}"
echo -e "${C}  └─────────────────────────────────────────┘${R}"
echo ""
echo -e "${Y}  Serving:${R}  $DIR"
echo -e "${Y}  URL:${R}      ${G}http://localhost:$PORT${R}"
echo ""
echo -e "  Press ${Y}Ctrl+C${R} to stop."
echo ""

# ── open browser after a short delay ─────────────────────────────────────────
(sleep 1 && \
  if command -v xdg-open &>/dev/null; then xdg-open "http://localhost:$PORT"
  elif command -v open &>/dev/null;     then open    "http://localhost:$PORT"
  fi) &

# ── pick a server, whatever is available ─────────────────────────────────────
if command -v python3 &>/dev/null; then
  python3 -m http.server "$PORT" --directory "$DIR"
elif command -v python &>/dev/null; then
  cd "$DIR" && python -m SimpleHTTPServer "$PORT"
elif command -v npx &>/dev/null; then
  npx --yes serve "$DIR" -l "$PORT"
else
  echo -e "\033[1;31m  No server found. Install python3 or node.\033[0m"
  exit 1
fi

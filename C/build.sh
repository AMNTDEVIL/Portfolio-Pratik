#!/usr/bin/env bash
# Run from ~/portfolio/C  (needs emsdk activated: source emsdk_env.sh)
set -euo pipefail

EXPORTS='["_malloc","_free",
"_portfoliofx_init","_portfoliofx_resize","_portfoliofx_set_pointer","_portfoliofx_set_is_touch","_portfoliofx_step",
"_portfoliofx_get_spotlight_mx","_portfoliofx_get_spotlight_my","_portfoliofx_get_particle_count",
"_portfoliofx_get_particles_stride_floats","_portfoliofx_get_particles_ptr",
"_domanim_init","_domanim_step","_domanim_get_count","_domanim_get_stride_floats","_domanim_get_ptr",
"_unified_sim_init","_unified_sim_step","_unified_sim_get_cursor_count","_unified_sim_get_cursor_stride","_unified_sim_get_cursor_ptr",
"_watersim_init","_watersim_step","_watersim_get_fish_buffer","_watersim_get_bubble_buffer"]'

emcc src/algorithms/*.c src/maths/*.c -Iinclude -O3 \
  -sMODULARIZE=1 -sEXPORT_NAME=createPortfolioModule \
  -sALLOW_MEMORY_GROWTH=1 -sENVIRONMENT=web \
  -sEXPORTED_FUNCTIONS="$(echo $EXPORTS | tr -d '\n ')" \
  -sEXPORTED_RUNTIME_METHODS='["HEAP32"]' \
  -o ../web/portfolio.js

echo "Built ../web/portfolio.js + web/portfolio.wasm"
# Serve with:  python3 -m http.server -d web 8000   (WASM needs http://, not file://)

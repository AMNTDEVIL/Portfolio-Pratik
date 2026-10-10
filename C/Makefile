# Compiler and flags
EMCC ?= emcc
CFLAGS = -I./include -O3 -ffast-math
WASM_FLAGS = -O3 -ffast-math -flto -msimd128 -mllvm -force-vector-width=4 -s WASM=1 \
    -s EXPORTED_FUNCTIONS='["_vector2D_add_x","_vector2D_add_y","_vector2D_sub_x","_vector2D_sub_y","_vector2D_scalar_x","_vector2D_scalar_y","_vector2D_lerp_x","_vector2D_lerp_y","_vector2D_dot_xy","_vector2D_magnitude_xy","_vector2D_rotation_x","_vector2D_rotation_y","_vector2D_smoothstep_x","_vector2D_smoothstep_y","_vector2D_ease_in_x","_vector2D_ease_in_y","_vector2D_ease_out_x","_vector2D_ease_out_y","_anim_clamp01","_anim_ease_in_out_cubic","_anim_smoothstep","_anim_exp_smooth","_anim_hash1","_anim_noise1","_anim_noise2","_anim_fbm2","_anim_wave","_anim_lerp","_anim_ease_out_elastic","_anim_ease_out_bounce","_anim_ease_out_back","_anim_ease_in_out_quart","_anim_benchmark_cubic_batch","_anim_benchmark_elastic_batch","_anim_spring","_anim_spring_velocity","_portfoliofx_init","_portfoliofx_resize","_portfoliofx_set_pointer","_portfoliofx_set_is_touch","_portfoliofx_step","_portfoliofx_get_spotlight_mx","_portfoliofx_get_spotlight_my","_portfoliofx_get_particle_count","_portfoliofx_get_particles_stride_floats","_portfoliofx_get_particles_ptr","_domanim_init","_domanim_step","_domanim_get_count","_domanim_get_stride_floats","_domanim_get_ptr","_unified_sim_init","_unified_sim_step","_unified_sim_get_cursor_count","_unified_sim_get_cursor_stride","_unified_sim_get_cursor_ptr","_unified_sim_get_shape_count","_unified_sim_get_shape_stride","_unified_sim_get_shape_ptr","_unified_sim_get_spring_count","_unified_sim_get_spring_stride","_unified_sim_get_spring_ptr","_unified_sim_trigger_spring","_skeleton_init","_skeleton_init_f","_skeleton_translate","_skeleton_rotate","_skeleton_get_x","_skeleton_get_y","_skeleton_get_rotation","_skeleton_get_x_f","_skeleton_get_y_f","_skeleton_move_forward","_skeleton_smooth","_skeleton_smooth_adaptive","_skeleton_move_direction","_animation_set_points","_animation_set_duration","_animation_step","_animation_get_x","_animation_get_y","_animation_reset","_animation_handle_create","_animation_handle_step","_animation_handle_get_x","_animation_handle_get_y","_animation_handle_reset","_animation_handle_destroy","_watersim_init","_watersim_step","_watersim_get_fish_buffer","_watersim_get_bubble_buffer","_watersim_get_wave_buffer"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    --no-entry

# Directories
SRC_DIR = src/maths
BUILD_DIR = build
INCLUDE_DIR = include

# Source files
SOURCES = src/maths/vector2D.c \
          src/maths/skeleton_maths.c \
          src/algorithms/anim.c \
          src/algorithms/portfolio_fx.c \
          src/algorithms/dom_anim.c \
          src/algorithms/unified_sim.c \
          src/algorithms/water_sim.c

# Output files
TARGET = vector2D
WASM_OUTPUT = $(BUILD_DIR)/$(TARGET).js
WASM_FILE = $(BUILD_DIR)/$(TARGET).wasm
INSTALL_DIR = ../js

# Default target
.PHONY: all
all: build

# Build target
.PHONY: build
build: $(BUILD_DIR)/.dir
	@echo "🔨 Building WebAssembly module..."
	$(EMCC) $(SOURCES) -o $(WASM_OUTPUT) $(WASM_FLAGS) -I$(INCLUDE_DIR)
	@echo "✅ Build complete!"
	@echo "📁 Output files:"
	@echo "   - $(WASM_OUTPUT)"
	@echo "   - $(WASM_FILE)"

# Create build directory (stamp file to avoid circular dependency with target name `build`)
$(BUILD_DIR)/.dir:
	@mkdir -p $(BUILD_DIR)
	@touch $(BUILD_DIR)/.dir

# Clean target
.PHONY: clean
clean:
	@echo "🧹 Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "✅ Clean complete!"

# Install to project directories
.PHONY: install
install: build
	@echo "📦 Installing WASM files to project..."
	@cp $(WASM_OUTPUT) $(WASM_FILE) $(INSTALL_DIR)/
	@echo "✅ Installed to $(INSTALL_DIR)/"
	@echo "   - $(INSTALL_DIR)/$(TARGET).js"
	@echo "   - $(INSTALL_DIR)/$(TARGET).wasm"

# Fresh install from scratch (purge + install)
.PHONY: fresh
fresh: clean install
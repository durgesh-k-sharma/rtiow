CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O3 -pthread
INCLUDES = -Isrc -isystem third_party

BUILD_DIR = build
BIN_RAYTRACER = $(BUILD_DIR)/raytracer
BIN_UNIT_TESTS = $(BUILD_DIR)/unit_tests
BIN_PIXEL_DIFF = $(BUILD_DIR)/pixel_diff

SRCS_RAYTRACER = src/main.cc src/image_writer.cc
SRCS_UNIT_TESTS = tests/test_main.cc tests/test_vec3.cc tests/test_sphere.cc tests/test_material.cc tests/test_aabb.cc
SRCS_PIXEL_DIFF = tools/pixel_diff.cc

all: $(BUILD_DIR) $(BIN_RAYTRACER) $(BIN_UNIT_TESTS) $(BIN_PIXEL_DIFF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_RAYTRACER): $(SRCS_RAYTRACER) src/*.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS_RAYTRACER) -o $(BIN_RAYTRACER)

$(BIN_UNIT_TESTS): $(SRCS_UNIT_TESTS) src/*.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS_UNIT_TESTS) -o $(BIN_UNIT_TESTS)

$(BIN_PIXEL_DIFF): $(SRCS_PIXEL_DIFF)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS_PIXEL_DIFF) -o $(BIN_PIXEL_DIFF)

test: $(BIN_UNIT_TESTS)
	./$(BIN_UNIT_TESTS)

smoke-test: all
	@mkdir -p golden_images output
	./$(BIN_RAYTRACER) --scene final --width 100 --samples 10 --depth 10 --threads 1 --png output/smoke_test.png
	@if [ -f golden_images/ci_smoke_test.png ]; then \
		./$(BIN_PIXEL_DIFF) output/smoke_test.png golden_images/ci_smoke_test.png --tolerance 5 --mean-tol 1.0; \
	else \
		echo "Golden image not yet generated. Saving output/smoke_test.png as golden_images/ci_smoke_test.png"; \
		cp output/smoke_test.png golden_images/ci_smoke_test.png; \
	fi

bench: all
	@echo "--- Benchmark: Without BVH (Linear Search) ---"
	./$(BIN_RAYTRACER) --scene final --width 200 --samples 20 --depth 20 --no-bvh --png output/bench_no_bvh.png
	@echo "--- Benchmark: With BVH Acceleration ---"
	./$(BIN_RAYTRACER) --scene final --width 200 --samples 20 --depth 20 --bvh --png output/bench_bvh.png

clean:
	rm -rf $(BUILD_DIR) output

.PHONY: all test smoke-test bench clean



#include "newyear_sparkling.h"  // Header file for this sketch

#include <Arduino.h>  // Core Arduino functionality
#include <FastLED.h>  // Main FastLED library for controlling LEDs

#include "fl/math_macros.h"  // Math helper functions and macros
#include "fl/time_alpha.h"   // Time-based alpha/transition effects
#include "fx/2d/blend.h"     // 2D blending effects between layers
#include "fx/2d/wave.h"      // 2D wave simulation

using namespace fl;  // Use the FastLED namespace for convenience

// Create a ripple effect at a random position within the central area of the display
void NewYearSparkling::triggerRipple() {
    // Define a margin percentage to keep ripples away from the edges
    float perc = .15f;

    // Calculate the boundaries for the ripple (15% from each edge)
    uint8_t min_x = perc * kMatrixWidth;         // Left boundary
    uint8_t max_x = (1 - perc) * kMatrixWidth;   // Right boundary
    uint8_t min_y = perc * kMatrixHeight;        // Top boundary
    uint8_t max_y = (1 - perc) * kMatrixHeight;  // Bottom boundary

    // Generate a random position within these boundaries
    int x = random(min_x, max_x);
    int y = random(min_y, max_y);

    // Set a wave peak at this position in both wave layers
    // The value 1.0 represents the maximum height of the wave
    waveFxLower.setf(x, y, 1);  // Create ripple in lower layer
    waveFxUpper.setf(x, y, 1);  // Create ripple in upper layer
}

// Create a fancy cross-shaped effect that expands from the center
void NewYearSparkling::applyFancyEffect(uint32_t now) {
    // Calculate the total animation duration based on the speed slider
    // Higher fancySpeed value = shorter duration (faster animation)
    uint32_t total = map(910, 0, 1000, 1000, 100);

    // Create a static TimeRamp to manage the animation timing
    // TimeRamp handles the transition from start to end over time
    static TimeRamp pointTransition = TimeRamp(total, 0, 0);

    // If the animation isn't currently active, exit early
    if (!pointTransition.isActive(now)) {
        // no need to draw
        return;
    }

    // Find the center of the display
    int mid_x = kMatrixWidth / 2;
    int mid_y = kMatrixHeight / 2;

    // Calculate the maximum distance from center (half the width)
    int amount = kMatrixWidth / 2;

    // Calculate the start and end coordinates for the cross
    int start_x = mid_x - amount;  // Leftmost point
    int end_x = mid_x + amount;    // Rightmost point
    int start_y = mid_y - amount;  // Topmost point
    int end_y = mid_y + amount;    // Bottommost point

    // Get the current animation progress (0-255)
    int curr_alpha = pointTransition.update8(now);

    // Map the animation progress to the four points of the expanding cross
    // As curr_alpha increases from 0 to 255, these points move from center to edges
    int left_x = map(curr_alpha, 0, 255, mid_x, start_x);  // Center to left
    int down_y = map(curr_alpha, 0, 255, mid_y, start_y);  // Center to top
    int right_x = map(curr_alpha, 0, 255, mid_x, end_x);   // Center to right
    int up_y = map(curr_alpha, 0, 255, mid_y, end_y);      // Center to bottom

    // Convert the 0-255 alpha to 0.0-1.0 range
    float curr_alpha_f = curr_alpha / 255.0f;

    // Calculate the wave height value - starts high and decreases as animation progresses
    // This makes the waves stronger at the beginning of the animation
    float valuef = (1.0f - curr_alpha_f) * 5 / 255.0f;

    // Calculate the width of the cross lines
    int span = 0.126 * kMatrixWidth;

    // Add wave energy along the four expanding lines of the cross
    // Each line is a horizontal or vertical span of pixels

    // Left-moving horizontal line
    for (int x = left_x - span; x < left_x + span; x++) {
        waveFxLower.addf(x, mid_y, valuef);  // Add to lower layer
        waveFxUpper.addf(x, mid_y, valuef);  // Add to upper layer
    }

    // Right-moving horizontal line
    for (int x = right_x - span; x < right_x + span; x++) {
        waveFxLower.addf(x, mid_y, valuef);
        waveFxUpper.addf(x, mid_y, valuef);
    }

    // Downward-moving vertical line
    for (int y = down_y - span; y < down_y + span; y++) {
        waveFxLower.addf(mid_x, y, valuef);
        waveFxUpper.addf(mid_x, y, valuef);
    }

    // Upward-moving vertical line
    for (int y = up_y - span; y < up_y + span; y++) {
        waveFxLower.addf(mid_x, y, valuef);
        waveFxUpper.addf(mid_x, y, valuef);
    }
}

void NewYearSparkling::processAutoTrigger(uint32_t now) {
    uint32_t trigger_delta = nextTrigger - now;
    if (trigger_delta > 10000) trigger_delta = 0;

    if (now >= nextTrigger) {
        triggerRipple();
        nextTrigger = now + random(350, 1800);
    }
}

// Color palettes define the gradient of colors used for the wave effects
// Each entry has the format: position (0-255), R, G, B

DEFINE_GRADIENT_PALETTE(electricBlueFirePal){
    0,   0,   0,   0,    // Black (lowest wave height)
    32,  0,   0,   70,   // Dark blue (low wave height)
    128, 20,  57,  255,  // Electric blue (medium wave height)
    255, 255, 255, 255   // White (maximum wave height)
};

DEFINE_GRADIENT_PALETTE(electricGreenFirePal){
    0,   0,   0,   0,    // Black (lowest wave height)
    8,   128, 64,  64,   // Green with red tint (very low wave height)
    16,  255, 222, 222,  // Pinkish red (low wave height)
    64,  255, 255, 255,  // White (medium wave height)
    255, 255, 255, 255   // White (maximum wave height)
};

// Create default configuration for the lower wave layer
WaveFx::Args NewYearSparkling::CreateArgsLower() {
    WaveFx::Args out;
    out.factor = SuperSample::SUPER_SAMPLE_2X;                                    // 2x supersampling for smoother waves
    out.half_duplex = true;                                                       // Only positive waves (no negative values)
    out.auto_updates = true;                                                      // Automatically update the simulation each frame
    out.speed = 0.18f;                                                            // Wave propagation speed
    out.dampening = 9.0f;                                                         // How quickly waves lose energy
    out.crgbMap = fl::make_shared<fl::WaveCrgbGradientMap>(electricBlueFirePal);  // Color palette for this wave
    return out;
}

// Create default configuration for the upper wave layer
WaveFx::Args NewYearSparkling::CreateArgsUpper() {
    WaveFx::Args out;
    out.factor = SuperSample::SUPER_SAMPLE_2X;                                 // 2x supersampling for smoother waves
    out.half_duplex = true;                                                    // Only positive waves (no negative values)
    out.auto_updates = true;                                                   // Automatically update the simulation each frame
    out.speed = 0.25f;                                                         // Wave propagation speed (faster than lower)
    out.dampening = 3.0f;                                                      // How quickly waves lose energy (less than lower)
    out.crgbMap = fl::make_shared<WaveCrgbGradientMap>(electricGreenFirePal);  // Color palette for this wave
    return out;
}

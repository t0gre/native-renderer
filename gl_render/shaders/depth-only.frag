#version 300 es
precision highp float;

// Add a dummy output so older GLs that still expect color outputs don't error.
out vec4 fragColor;

void main() {
    // masked, not used
    fragColor = vec4(0.0);
}
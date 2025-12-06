uniform float time;         // Animation timer
uniform sampler2D noiseTex; // Noise texture

// Calculates the pixel color based on noise and gradients to fake fire
void main()
{
    // Get the texture coordinates for the current pixel
    // 's' is horizontal (0 to 1), 't' is vertical (0 to 1)
    vec2 st = gl_TexCoord[0].st;

    // Makes fire look like it's rising
    vec2 scrollPos = st;
    scrollPos.y -= time * 1.5; 

    // Gives a flickering, moving pattern
    vec4 noise = texture2D(noiseTex, scrollPos);

    // Bright at the bottom (0.0), invisible at the top (1.0)
    float vertGrad = 1.0 - st.t;
    vertGrad = vertGrad * vertGrad;

    // Bright in the middle, invisible at the left/right edges
    float horizGrad = 1.0 - abs(st.s * 2.0 - 1.0);
    horizGrad = clamp(horizGrad * 1.5, 0.0, 1.0);

    // Create a mountain shape
    float finalGradient = vertGrad * horizGrad;

    // Calculate intensity
    float intensity = noise.r * finalGradient * 3.5;

    // Map the brightness values to actual fire colors
    vec4 color = vec4(0.0);

    // Color Thresholds
    if (intensity > 1.4)
        color = vec4(1.0, 1.0, 0.6, 1.0); // White/Yellow Core
    else if (intensity > 0.6)
        color = vec4(1.0, intensity * 0.6, 0.0, 0.9); // Orange Body
    else if (intensity > 0.2)
        color = vec4(0.8, 0.0, 0.0, 0.4); // Red Tips
    else
        discard; // Transparent background

    // Output the final color to the screen
    gl_FragColor = color;
}
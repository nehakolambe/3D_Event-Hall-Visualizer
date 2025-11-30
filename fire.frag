uniform float time;         // Animation timer
uniform sampler2D noiseTex; // Noise texture

void main()
{
    // texture coordinates
    vec2 st = gl_TexCoord[0].st;

    // noise downwards
    vec2 scrollPos = st;
    scrollPos.y -= time * 1.5; 

    // sample the noise(cloud) texture
    vec4 noise = texture2D(noiseTex, scrollPos);

    // Vertical Gradient (fades out at top)
    float vertGrad = 1.0 - st.t;
    vertGrad = vertGrad * vertGrad;

    // horizontal fade
    float horizGrad = 1.0 - abs(st.s * 2.0 - 1.0);
    
    // amplify horizontal fade
    horizGrad = clamp(horizGrad * 1.5, 0.0, 1.0);

    // combined gradient
    float finalGradient = vertGrad * horizGrad;

    // intensity based on noise and gradient
    float intensity = noise.r * finalGradient * 3.5;

    // fire colors based on intensity
    vec4 color;
    if (intensity > 1.4)
        color = vec4(1.0, 1.0, 0.6, 1.0); // White/Yellow Core
    else if (intensity > 0.6)
        color = vec4(1.0, intensity * 0.6, 0.0, 0.9); // Orange Body
    else if (intensity > 0.2)
        color = vec4(0.8, 0.0, 0.0, 0.4); // Red Tips
    else
        discard; // Transparent background

    gl_FragColor = color;
}
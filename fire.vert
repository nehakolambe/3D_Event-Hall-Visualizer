void main()
{
    // pass through texture coordinates
    gl_TexCoord[0] = gl_MultiTexCoord0;

    // standard transformation
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
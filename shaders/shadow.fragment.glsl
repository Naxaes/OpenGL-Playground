#version 330


void main()
{
    // Depth buffer is filled by deafult. Nothing to do here.
    gl_FragDepth = gl_FragCoord.z;
}

#ifdef GL_ES
//
precision mediump float;

#endif


uniform sampler2D uTexture;

varying vec2 vUvs;
varying vec4 vColor;
uniform float uScreenHeight;

void main(void)
{

    vec4 original = texture2D(uTexture, vUvs) * vColor;

    original *= (mod(vUvs.y, (1.0 / (uScreenHeight * 2.0)) * 2.0) * 1.0 / (1.0 / (uScreenHeight * 2.0)));

    gl_FragColor = clamp(original * 0.45, 0.0, 1.0);
}

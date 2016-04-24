#version 120

uniform sampler2DRect tex0;

uniform vec2 imageViewPort;
uniform vec2 screenViewPort;

uniform float doGray;

void main(){
    vec2 coord = imageViewPort - gl_FragCoord.xy / screenViewPort * imageViewPort;
    vec4 color = texture2DRect(tex0, coord);
    
    float gray = color.r + color.g + color.b;
    gray = gray * 0.33333;
    
    if(doGray > 0) {
        gl_FragColor = vec4(gray, gray, gray, color.a);
    }
    else {
        gl_FragColor = color;
    }
}
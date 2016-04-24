#version 120

uniform sampler2DRect tex0;

uniform vec2 imageViewPort;
uniform vec2 screenViewPort;

uniform float clenchCount;

uniform float focusIndex;

uniform vec2 facePosition;

void main(){
	//this is the fragment shader
	//this is where the pixel level drawing happens
	//gl_FragCoord gives us the x and y of the current pixel its drawing
	
	//we grab the x and y and store them in an int
	float xVal = gl_FragCoord.x;
	float yVal = gl_FragCoord.y;
	
	//we use the mod function to only draw pixels if they are every 2 in x or every 4 in y
    //gl_FragColor = gl_Color;  
    
    vec2 coord = imageViewPort - gl_FragCoord.xy / screenViewPort * imageViewPort;
    vec4 color = texture2DRect(tex0, coord);
    float gray = color.r + color.g + color.b;
    gray = gray * 0.33333;
    
    float dist = distance(coord, facePosition) / 150;
    vec4 modColor;
    modColor = color * 2.0f;

    if(/*clenchCount > 0*/ dist < 1) {
        modColor = mix(modColor, vec4(gray, gray, gray, color.a), dist);
    }
    else {
        modColor = vec4(gray, gray, gray, color.a);
    }
    gl_FragColor = mix(color, modColor, focusIndex);
    
    if(clenchCount > 0) {
        gl_FragColor = vec4(gray, gray, gray, color.a);
    }

}
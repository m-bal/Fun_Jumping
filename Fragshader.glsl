#version 120
#define MIN_DIST 0.0001
#define MAX_DIST 20
#define MAX_STEPS 100

uniform mat4 iMat;
uniform vec2 iResolution;
uniform vec3 iDebug;
uniform float time;

float SDF_Sphere(in vec3 pos){
	
	vec4 sphere = vec4(0,0,-2, .25);
	float sDist = length(pos-sphere.xyz)-sphere.w;

	return sDist;
}
/*
This is will be my ground plane 
*/
float SDF_Plane(in vec3 pos){
	float planeDist = pos.y+ 0.25;
	
	return planeDist;
}

float RAY_Marcher(in vec3 co, in vec3 cd){

	float d = 0.0;
	vec3 pos;
	for(int i = 0; i < MAX_STEPS; i++){
		pos = co+d*cd;
		float distsphere = SDF_Sphere(co+d*cd);
		float distplane = SDF_Plane(pos);
		float dist = min(distplane, distsphere); 
		if(dist < MIN_DIST)break;
		d += dist;
		if(d > MAX_DIST) break;
	}
	
	return d;
}



void main(){
	
	vec2 uv = ((2*gl_FragCoord.xy)-iResolution.xy)/iResolution.xy;
	
	//camera origin works the same way as opengl coordinates 
	vec3 cam_o = vec3(0,0,2);
	//this is the point we are shooting to --V
	vec3 cam_d = normalize(vec3(uv.x, uv.y, -2));
	
	
	vec3 col = vec3(0);
	float p = RAY_Marcher(cam_o, cam_d);
	
	
	if( p < MAX_DIST){
		vec3 sun = vec3(0, 1, 0);
		
		col = vec3(1);
	}
	
		
	gl_FragColor = vec4(col, 1);
}

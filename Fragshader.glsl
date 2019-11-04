#version 120
#define MIN_DIST 0.01
#define MAX_STEPS 100

uniform mat4 iMat;
uniform vec2 iResolution;
uniform vec3 iDebug;
uniform float time;

float SDF_Sphere(in vec3 pos){
	vec4 s= vec4(0, 1, 6, 1);
	float sDist = length(pos-s.xyz)-s.w;
	float planeDist = pos.y;
	
	float d = min(sDist, planeDist);
	
	return d;
}

float RAY_Marcher(in vec3 co, in vec3 cd){

	float d = 0.0;
	for(int i = 0; i < MAX_STEPS; i++){
		vec3 p = co+d*cd;
		float distsphere = SDF_Sphere(p);
		d += distsphere;
		if(distsphere < MIN_DIST || d > MAX_STEPS) break;
	}
	
	
	return d;
}



void main(){
	vec2 uv = ((gl_FragCoord.xy)-.5*iResolution.xy)/iResolution.y;
	
	vec3 cam_o = vec3(0,1,0);
	vec3 cam_d = normalize(vec3(uv.x, uv.y, 1));
	
	
	vec3 col = vec3(0);	
	float p = RAY_Marcher(cam_o, cam_d);		
	p /= 6.;
	col = vec3(p);

	
	gl_FragColor = vec4(col, 1);
}

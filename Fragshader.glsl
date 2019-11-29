#version 120
#define MIN_DIST 0.001
#define MAX_DIST 20
#define MAX_STEPS 100

uniform mat4 iMat;
uniform vec2 iResolution;
uniform vec3 iDebug;
uniform float time;

float smoothmin(float f1, float f2, float k)
{
	float h = max( k-abs(f1-f2), 0.0 )/k;
    return min( f1, f2 ) - h*h*k*(1.0/4.0);
}

float SDF_Sphere(in vec3 pos){
	vec4 s1 = vec4(sin(time),0,0,0.25);
	vec4 s2 = vec4(sin(-time),0,0,0.25);
	float dists1 = length(pos-s1.xyz)-s1.w;
	float dists2 = length(pos-s2.xyz)-s2.w;
	return smoothmin(dists1, dists2, 1);

}
/*
This is will be my ground plane 
*/
float SDF_Plane(in vec3 pos){
	//float planeDist = pos.y + 0.25;
	float planeDist = pos.y+(cos(pos.x)*.25);
	return planeDist;
}

float map(in vec3 pos){
	return min(SDF_Sphere(pos), SDF_Plane(pos));
}

float RAY_Marcher(in vec3 co, in vec3 cd){

	float d = 0.0;
	
	for(int i = 0; i < MAX_STEPS; i++){
		vec3 pos = co+d*cd;
		float dist = map(pos); 
		if(dist < MIN_DIST)break;
		d += dist;
		if(d > MAX_DIST) break;
	}
	//if(d > MAX_DIST) d = -1.0;
	
	return d;
}

float softshadow( in vec3 ro, in vec3 rd, float mint, float maxt, float k )
{
    float res = 1.0;
    for( float t=mint; t<maxt; )
    {
        float h = map(ro + rd*t);
        if( h<0.0001 )
            return 0.0;
        res = min( res, k*h/t );
        t += h;
    }
    return res;
}
vec3 calcNormals(in vec3 p){
	const float eps = 0.0001;
	vec2 h = vec2(eps, 0);
	return normalize(vec3(map(p+h.xyy)-map(p-h.xyy),
					map(p+h.yxy)-map(p-h.yxy),
					map(p+h.yyx)-map(p-h.yyx)));
}
void main(){
	
	vec2 uv = ((2*gl_FragCoord.xy)-iResolution.xy)/iResolution.xy;
	
	//camera origin works the same way as opengl coordinates 
	vec3 cam_o = vec3(0,0,2);
	//this is the point we are shooting to --V
	vec3 cam_d = normalize(vec3(uv, -2));
	
	
	vec3 col = 0.5 + 0.5*cos(time+uv.xyy);

	float p = RAY_Marcher(cam_o, cam_d);
	if (p < MAX_DIST) {
		vec3 pos = cam_o + p*cam_d;
		vec3 nor = calcNormals(pos);
		vec3 sun = vec3(cos(time), 1, 1);
		vec3 sun_dir = normalize(sun);
		float diff = clamp(dot(sun_dir, nor), 0, 1);
		float sshadow = softshadow(pos+nor*0.001, sun_dir, 0.1, 3, 8);
		//col = 1-cos(sin(time)+uv.xyx+vec3(0,2,4))*diff*sshadow;
		vec3 check = sin(time)*cos(time)*uv.xyx;
		col = vec3(.92, .88, .68)*diff*sshadow;
		//col += check;
		
	}
	
		
	gl_FragColor = vec4(col, 1);
}

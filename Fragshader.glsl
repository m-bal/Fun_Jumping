#version 120
#define MIN_DIST 0.0001
#define MAX_DIST 20
#define MAX_STEPS 100

uniform mat4 iMat;
uniform vec2 iResolution;
uniform vec3 iDebug;
uniform float time;


//function credit goes to Dave Smith creator of the game Dreams
float smoothmin(float f1, float f2, float k)
{
    float h = max( k-abs(f1-f2), 0.0 )/k;
    return min( f1, f2 ) - h*h*h*k*(1.0/6);
}

float SDF_Sphere(in vec3 pos){
	vec4 s1 = vec4(0,(sin(time))-.5,0,0.50);
	vec4 s2 = vec4(0,(sin(time))+.5,0,0.35);
	vec3 mpos = vec3(mod(abs(pos.x), 3.0)-1.5, pos.y, mod(pos.z+1.5, 2)-1.5);
	float dists1 = length(mpos-s1.xyz)-s1.w;
	float dists2 = length(mpos-s2.xyz)-s2.w;
	return smoothmin(dists1, dists2, 1);

}
/*
This is will be my ground plane 
*/
float SDF_Plane(in vec3 pos){
	float planeDist = pos.y + 0.25;
	//float planeDist = pos.y;
	return planeDist;
}

float map(in vec3 pos){
	return smoothmin(SDF_Sphere(pos), SDF_Plane(pos), 1);
}

float RAY_Marcher(in vec3 co, in vec3 cd){

	float d = 0.0;
	
	for(int i = 0; i < MAX_STEPS; i++){
		vec3 pos = co+d*cd;
		float dist = map(pos); 
		if(abs(dist) < (MIN_DIST*d))break;
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
        if( h<0.001 )
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
	vec3 cam_o = vec3(0,3,-3+time);
	//this is the point we are shooting to --V
	vec3 cam_d = normalize(vec3(uv, 1));
	
	
	vec3 col = .7 + 0.7*tan(uv.yyy*uv.xyx)+sin(time);

	float p = RAY_Marcher(cam_o, cam_d);
	if (p < MAX_DIST) {
		vec3 pos = cam_o + p*cam_d;
		vec3 nor = calcNormals(pos);
		vec3 sun = vec3(2, 4, 0);
		vec3 sun_dir = normalize(sun);
		float diff = clamp(dot(sun_dir, nor), 0, 1);
		float sshadow = softshadow(pos+nor*0.001, sun_dir, 0.1, 3, 8);
		//col = 1-cos(sin(time)+uv.xyx+vec3(0,2,4))*diff*sshadow;
		//checkered ground
		float check = smoothstep(.2,-.2, sin(pos.z)*sin(pos.x)+sin(pos.z)*sin(pos.x));
		col = vec3(1, 1, 1)*diff*sshadow;
		col += vec3(.1,.1,.1)*check;
		
	}
	
		
	gl_FragColor = vec4(col, 1);
}

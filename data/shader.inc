//Object A (tunnel)
float oa(vec3 q)
{
 return cos(q.x)+cos(q.y*1.5)+cos(q.z)+cos(q.y*20.)*.05;
}

//Object B (ribbon)
float ob(vec3 q)
{
 return length(max(abs(q-vec3(cos(q.z*1.5)*.3,-.5+cos(q.z)*.2,.0))-vec3(.125,.02,time+3.),vec3(.0)));
}

//Scene
float o(vec3 q)
{
 return min(oa(q),ob(q));
}

//Get Normal
vec3 gn(vec3 q)
{
 vec3 f=vec3(.01,0,0);
 return normalize(vec3(o(q+f.xyy),o(q+f.yxy),o(q+f.yyx)));
}

vec3 RotateY(vec3 _vDir, float _fAngle)
{
  return vec3(_vDir.x * cos(_fAngle) + _vDir.z * sin(_fAngle), _vDir.y, _vDir.z * cos(_fAngle) - _vDir.x * sin(_fAngle));
}

float softshadow( in vec3 ro, in vec3 rd, float mint, float maxt )
{
    float res = 1.0;
    for( float t=mint; t < maxt; )
    {
        float h = o(ro + rd*t);
        if( h<0.001 )
            return 0.0;
        res = min( res, 2*h/t );
        t += h;
    }
    return res;
}

//MainLoop
void main(void)
{
 vec4 c=vec4(1.0);
 vec2 v    = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;
 vec3 dir = normalize(RotateY(RotateY(vec3(v.y * fov, v.x * fov * ratio, 1.0), -pitch).yxz, heading));
 vec3 org = position;

 vec3 q=org,pp;
 float d=.0;

 //First raymarching
 for(int i=0;i<64;i++)
 {
  d=o(q);
  q+=d*dir;
 }
 pp=q;
 float f=length(q-org)*0.02;

 //Second raymarching (reflection)
 dir=reflect(dir,gn(q));
 q+=dir;
 for(int i=0;i<64;i++)
 {
 d=o(q);
 q+=d*dir;
 }
 c=max(dot(gn(q),vec3(.1,.1,.0)),.0)+vec4(.3,cos(time*.5)*.5+.5,sin(time*.5)*.5+.5,1.)*min(length(q-org)*.04,1.);
 q=pp;
 dir= normalize(vec3(0.0, 0.0, 10.0) - pp);
 

 //Ribbon Color
 if(oa(pp)>ob(pp))c=mix(c,vec4(cos(time*.3)*.5+.5,cos(time*.2)*.5+.5,sin(time*.3)*.5+.5,1.),.3);

 //Final Color
 vec4 fcolor = ((c+vec4(f))+(1.-min(pp.y+1.9,1.))*vec4(1.,.8,.7,1.))*min(time*.5,1.);
 gl_FragColor=vec4(fcolor.xyz,1.0);
}

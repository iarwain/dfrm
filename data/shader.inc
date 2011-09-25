vec3 rotateY(vec3 _vDir, float _fAngle)
{
  return vec3(_vDir.x * cos(_fAngle) + _vDir.z * sin(_fAngle), _vDir.y, _vDir.z * cos(_fAngle) - _vDir.x * sin(_fAngle));
}
vec3 rotateX(vec3 _vDir, float _fAngle)
{
  return vec3(_vDir.y * cos(_fAngle) + _vDir.z * sin(_fAngle), _vDir.x, _vDir.z * cos(_fAngle) - _vDir.y * sin(_fAngle));
}
float planeX(in vec3 p, in float x)
{
    return distance(p,vec3(x,p.y,p.z));
}
float planeY(in vec3 p, in float y)
{
    return distance(p,vec3(p.x,y,p.z));
}
float planeZ(in vec3 p, in float z)
{
    return distance(p,vec3(p.x,p.y,z));
}
float sphere( in vec3 p, in float r )
{
    return length(p) - r;
}
float box( in vec3 p, in vec3 abc )
{
    vec3 di=max(abs(p)-abc,0.0);
    return dot(di,di);
}
float ob(vec3 q)
{
 return length(max(abs(q-vec3(cos(q.z*1.5)*.3,-.5+cos(q.z)*.2,.0))-vec3(.125,.02,time+3.),vec3(.0)));
}

float oc(vec3 q)
{
 return length(max(abs(q-vec3(0.,.5+cos(q.x)*.2,cos(q.x*1.5)*.3))-vec3(time+3.,.02,.125),vec3(.0)));
}

vec3 vEarth = vec3(1.5, 0.5, 0.0);

float map(in vec3 vPos, out int iID)
{
  vec3 vMod = vec3(0.0, 0.0, repeat * 0.0011 * floor(1000.0 * sin(3.14*0.1*time)));
  vec3 q = vPos;
  
  float d1 = oc(q);
  float d4 = ob(q);
  float ff = 0.6 * abs(sin(time*0.5)) * distort;
  float c = cos(ff*vPos.y);
  float s = sin(ff*vPos.y);
  mat2  m = mat2(c,-s,s,c);
  q = vec3(m*vPos.xy,vPos.z);
  vec3 vToS = vPos - vEarth;
  vec2 vUV = vec2(1.0 - mod(atan(vToS.x, vToS.z) + time * 0.1, 6.28) / 6.28, 1.0 - acos(vToS.y / length(vToS)) / 3.14);
  vec3 vT = texture2D(tex0, vUV).rgb;
  float dT = 0.01 * (vT.x + vT.y + vT.z) * bump;
  
  float d2 = sphere(q - vEarth, 0.8) - dT;
  q = mod(vPos, vMod) - 0.5 * vMod;
  float d3 = sphere(q - vec3(0.0, 1.2, 0.0), 0.2);

  float d = min(d4, min(d3, min(d1, d2)));

  if(d == d4)
  {
    iID = 4;
  }
  else if(d == d3)
  {
    iID = 3;
  }
  else if(d == d2)
  {
    iID = 2;
  }
  else
  {
    iID = 1;
  }

  return d;
}

vec3 calcNormal( in vec3 pos )
{
    vec3 e = vec3(0.0002, 0.0, 0.0);
    vec3 n;
    int id;
    n.x = map(pos + e.xyy,id) - map(pos - e.xyy,id);
    n.y = map(pos + e.yxy,id) - map(pos - e.yxy,id);
    n.z = map(pos + e.yyx,id) - map(pos - e.yyx,id);
    return normalize(n);
}

vec3 getColor(vec3 pos, int id )
{
    vec3 color;

         if(id==2)
          {
            vec3 vToS = pos - vEarth;
            vec2 vUV = vec2(1.0 - mod(atan(vToS.x, vToS.z) + time * 0.1, 6.28) / 6.28, 1.0 - acos(vToS.y / length(vToS)) / 3.14);

            color = texture2D(tex0, vUV).rgb;
          }
    else if(id==4) color = vec3(0.15,0.5,0.15);
    else if(id==3) color = vec3(0.64,0.15,0.1);
    else if(id==1) color = vec3(0.5,0.15,0.5);
    else           color = vec3(0.76,0.75,0.5);
    return color;
}

float SoftShadow( in vec3 ro, in vec3 rd, float mint, float maxt)
{
    float res = 1.0;
    for( float t=mint; t < maxt; )
    {
      int i;
        float h = map(ro + rd*t, i);
        if( h<0.001 )
            return 0.0;
        res = min( res, 64.0*h/t);
        t += h;
    }
    return res;
}

void main(void)
{
    vec2 v    = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;
    vec3 Rd = normalize(rotateY(rotateY(vec3(v.y * fov, v.x * fov * ratio, 1.0), -pitch).yxz, heading));
    vec3 Ro = position;
    vec3 color = vec3(0.0,0.15,0.1);
    vec3 vLight = vec3(1.5 * sin(3.14 * 0.5 * time), 2.0, 1.5 * cos(3.14 * 0.5 * time));
    
    float t = 0.0, d = 0.0;
    for(t = 0.0; t < 10.0; t += d)
    {
        vec3 pos = Ro+t*Rd;
        int id;
        d = map(pos,id);
        const float eps = 0.001;
        if(d < eps)
        {
             vec3 l = vLight - pos;
             float dl = length(l);
             l = normalize(l);
             color = getColor(pos,  id );
             vec3 n = calcNormal( pos );
             color = color*(0.1+0.8*dot(n, l)) / (0.4 * dl);
             color *= SoftShadow(vLight, -l, 0.01, dl - 0.1);
             color = color * exp2( -0.4*t );

             break;
        }
    }

    gl_FragColor = vec4(color,1.0);
}

// Pseudo random number base generator (credits go to iq/rgba)
float rnd(vec2 x)
{
    int n = int(x.x * 40.0 + x.y * 6400.0);
    n = (n << 13) ^ n;
    return 1.0 - float( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;
}

// Generate cubic interpolated random values
float smoothrnd(vec2 x)
{
    x = mod(x,1000.0);
    vec2 a = fract(x);
    x -= a;
    vec2 u = a*a*(3.0-2.0*a);
    return mix(
    mix(rnd(x+vec2(0.0)),rnd(x+vec2(1.0,0.0)), u.x),
    mix(rnd(x+vec2(0.0,1.0)),rnd(x+vec2(1.0)), u.x), u.y);
}
// Convert the cipher range from [-1,1] to [0,1]
float norm(float x)
{
    return x * 0.5 + 0.5;
}

vec3 rotateY(in vec3 _vDir, in float _fAngle)
{
  return vec3(_vDir.x * cos(_fAngle) + _vDir.z * sin(_fAngle), _vDir.y, _vDir.z * cos(_fAngle) - _vDir.x * sin(_fAngle));
}
vec3 rotateX(in vec3 _vDir, in float _fAngle)
{
  return vec3(_vDir.y * cos(_fAngle) + _vDir.z * sin(_fAngle), _vDir.x, _vDir.z * cos(_fAngle) - _vDir.y * sin(_fAngle));
}
float sphere(in vec3 _vPos, in float _fRadius)
{
    return length(_vPos) - _fRadius;
}
float ribbon1(vec3 _vPos)
{
 return length(max(abs(_vPos-vec3(cos(_vPos.z*1.5)*.3,-.5+cos(_vPos.z)*.2,.0))-vec3(.125,.02,time+3.),vec3(.0)));
}

float ribbon2(vec3 _vPos)
{
 return length(max(abs(_vPos-vec3(0.,.5+cos(_vPos.x)*.2,cos(_vPos.x*1.5)*.3))-vec3(time+3.,.02,.125),vec3(.0)));
}

vec3 vEarth = vec3(1.5, 0.5, 0.0), vLight;

vec3 getEarthColor(in vec3 _vPos)
{
  vec3 vToE = _vPos - vEarth;
  vec2 vUV  = vec2(1.0 - mod(atan(vToE.x, vToE.z) + time * 0.1, 6.28) / 6.28, 1.0 - acos(vToE.y / length(vToE)) / 3.14);

  return texture2D(tex0, vUV).rgb;
}

float map(in vec3 _vPos, out int _iID)
{
  vec3 vMod   = vec3(0.0, 0.0, repeat * 0.0011 * floor(1000.0 * sin(3.14 * 0.1 * time)));
  vec3 vTest  = _vPos;

  // Ribbons
  float d1    = ribbon2(vTest);
  float d4    = ribbon1(vTest);

  // Distorted earth
  float fDist = 0.6 * abs(sin(time * 0.5)) * distort;
  float fCos  = cos(fDist * _vPos.y);
  float fSin  = sin(fDist * _vPos.y);
  mat2  mRot  = mat2(fCos, -fSin, fSin, fCos);
  vec3  vCol  = getEarthColor(_vPos);
  vTest       = vec3(mRot * _vPos.xy, _vPos.z);
  float fBump = 0.01 * (vCol.x + vCol.y + vCol.z) * bump;
  float d2    = sphere(vTest - vEarth, 0.8) - fBump;

  // Applies repeat on last sphere
  vTest = mod(_vPos, vMod) - 0.5 * vMod;
  float d3 = sphere(vTest - vec3(0.0, 1.2, 0.0), 0.2);

  // Gets full scene distance
  float d = min(d4, min(d3, min(d1, d2)));

  // Updates ID
  if(d == d4)
  {
    _iID = 4;
  }
  else if(d == d3)
  {
    _iID = 3;
  }
  else if(d == d2)
  {
    _iID = 2;
  }
  else
  {
    _iID = 1;
  }

  // Done!
  return d;
}

vec3 getNormal(in vec3 _vPos)
{
  int   iID;
  vec3  vEps = vec3(0.0001, 0.0, 0.0);

  return normalize(vec3(map(_vPos + vEps.xyy, iID) - map(_vPos - vEps.xyy, iID), map(_vPos + vEps.yxy, iID) - map(_vPos - vEps.yxy, iID), map(_vPos + vEps.yyx, iID) - map(_vPos - vEps.yyx, iID)));
}

vec3 getColor(in vec3 _vPos, in int _iID)
{
  vec3 vColor;

  if(_iID==1)
  {
    vColor = vec3(0.5,0.15,0.5);
  }
  else if(_iID==2)
  {
    vColor = getEarthColor(_vPos);
  }
  else if(_iID==4)
  {
    vColor = vec3(0.15, 0.5, 0.15);
  }
  else if(_iID==3)
  {
    vColor = vec3(0.64, 0.15, 0.1);
  }
  else
  {
    vColor = vec3(0.0);
  }

  return vColor;
}

float getShadow(in vec3 _vPos, in vec3 _vDir, in float _fMin, in float _fMax)
{
  float fResult = 1.0, fDist, fStep;

  for(fDist = _fMin; fDist < _fMax; fDist += fStep)
  {
    int iID;
    fStep = map(_vPos + _vDir * fDist, iID);

    if(fStep < 0.001)
    {
      fResult = 0.0;

      break;
    }

    fResult = min(fResult, 64.0 * fStep / fDist);
  }

  return fResult;
}

vec3 getReflection(in vec3 _vPos, in vec3 _vDir)
{
  float fDist, fStep;
  vec3  vResult = vec3(0.0);

  for(fDist = 0.01; fDist < 10.0; fDist += fStep)
  {
    int iID;
    vec3 vTest    = _vPos + _vDir * fDist;
    fStep         = map(vTest, iID);

    if(fStep < 0.001)
    {
      vec3 vToL   = vLight - vTest;
      float fToL  = length(vToL);
      vToL        = normalize(vToL);
      vResult     = getColor(vTest, iID) * max(0.0, (0.8 * dot(getNormal(vTest), vToL)) / (0.4 * fToL));

      break;
    }
  }

  return vResult;
}

vec3 getSky(in vec3 _vDir, in vec3 _vToL)
{
  float fPhi    = atan(_vDir.x, _vDir.z);
  float fTheta  = acos(_vDir.y / length(_vDir));

  return vec3(smoothstep(0.0, 0.5, norm(0.5 * smoothrnd(300.0 * vec2(fPhi, fTheta))) + norm(0.75 * smoothrnd(500.0 * vec2(fPhi, fTheta))) - 1.25) * clamp(1.0 - _vToL.y, 0.0, 1.0));
}
              
void main()
{
  // Gets camera position & direction
  vec2 v    = -1.0 + 2.0 * gl_FragCoord.xy / resolution.xy;
  vec3 vDir = normalize(rotateY(rotateY(vec3(v.y * fov, v.x * fov * ratio, 1.0), -pitch).yxz, heading));
  vec3 vPos = position;

  // Inits color
  vec3 vColor = vec3(0.02, 0.02, 0.02);

  // Moves light
  vLight = vec3(1.5 * sin(3.14 * 0.5 * time), 2.0, 1.5 * cos(3.14 * 0.5 * time));

  float fDist, fStep;
  vec3  vTest;
  for(fDist = 0.0; fDist < 10.0; fDist += fStep)
  {
    int iID;

    // Gets test position
    vTest = vPos + fDist * vDir;

    // Maps it
    fStep = map(vTest, iID);

    // Found?
    if(fStep < 0.001)
    {
      vec3 vToL   = vLight - vTest;
      float fToL  = length(vToL);
      vToL        = normalize(vToL);
      vec3 vNorm  = getNormal(vTest);

      // Gets base color
      vColor = getColor(vTest, iID);

      // First ribbon?
      if(iID == 1)
      {
        // Adds reflection
        vColor += getReflection(vTest, reflect(vDir, vNorm));
      }

      // Adds lighting
      vColor = vColor * (0.2 + 0.8 * dot(vNorm, vToL)) / (0.4 * fToL);

      // Adds soft shadowing
      vColor *= getShadow(vLight, -vToL, 0.01, fToL - 0.2);

      break;
    }
  }

  if(fDist >= 10.0)
  {
    vColor += getSky(vDir, normalize(vLight - vTest));
  }
  
  // Updates fragement color
  gl_FragColor = vec4(vColor, 1.0);
}

#version 120
#extension GL_EXT_gpu_shader4 : enable



uniform mat4 rotMat4;
uniform vec3 translateVec3;

uniform sampler3D blockPool;
uniform sampler1D tranf;
uniform sampler1D CDFtranf;
uniform sampler2D preInt2D;

uniform float sizeX;
uniform float sizeY;
uniform float sizeZ;

uniform float isPreIntegration;

uniform float aspectRatio;

uniform float stepsize;

uniform vec3 cameraO;
uniform vec3 cameraDir;
uniform vec3 cameraUp;

vec3 eyeO;


vec2 intersectBox(vec3 entryPoint, vec3 rayDir, vec3 boxmin, vec3 boxmax)
{
    vec3 invR = vec3(1.0) / rayDir;
    vec3 tbot = invR * (boxmin - entryPoint);
    vec3 ttop = invR * (boxmax - entryPoint);
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    float largest_tmin = max(max(tmin.x, tmin.y), tmin.z);
    float smallest_tmax = min(min(tmax.x, tmax.y), tmax.z);

    return vec2(largest_tmin, smallest_tmax);
}


vec4 Rendering(vec3 front, vec3 back, float upperThreshold, float threshold, float offset)
{
    vec3 dir;
    dir = back - front;
    float len = length(dir);
    vec3 norm_dir = normalize(dir);

    vec3 rayIncrement = stepsize*norm_dir;

    vec3 pos = front;
    vec4 value=vec4(0, 0, 0, 0);
    float valuef = 0;

    vec4 result = vec4(0, 0, 0, 0);
    float raylength = 0;

    float scale = 1.0/(upperThreshold - threshold);
    float res = 0;

    vec4 lastCDF = vec4(0);
    vec4 currentCDF = vec4(0);
    float lastValuef = 0;

    while (result.a < 1.0 && raylength < len)
    {
        float factor = 0.5;

        vec3 pos = front+raylength*norm_dir;
        vec4 texValue = vec4(0);

        valuef = texture3D(blockPool, pos).a;

        if (isPreIntegration == 1.0)
        {
            //1D pre-integration
            currentCDF = texture1D(CDFtranf, valuef);
            float dv = abs(lastValuef - valuef) * 255.0;
            float da = abs(lastCDF.a - currentCDF.a);
            if (dv == 0.0 || da == 0.0)
            texValue = texture1D(tranf, valuef);
            else
            {
                texValue.a = da/dv;
                texValue.rgb = abs(lastCDF.rgb - currentCDF.rgb)/da;
            }
        }
        // 2D pre-integration
        else if (isPreIntegration == 2.0)
        {
            texValue = texture2D(preInt2D, vec2(valuef, lastValuef));
        }
        lastCDF = currentCDF;
        lastValuef = valuef;

        texValue.a = texValue.a *stepsize *sizeX;

        result.rgb += (1.0-result.a)*texValue.a*texValue.rgb;
        result.a += (1.0-result.a)*texValue.a;
        raylength +=  stepsize;
    }
    return result;

}
void main()
{
    float maxSize = max(sizeZ, max(sizeX, sizeY));
    float nsizeX = sizeX/maxSize;
    float nsizeY = sizeY/maxSize;
    float nsizeZ = sizeZ/maxSize;
    vec3 boxMin = vec3(-nsizeX, -nsizeY, -nsizeZ);
    vec3 boxMax = vec3(nsizeX, nsizeY, nsizeZ);

    mat3 rotateMat = mat3(vec3(rotMat4[0]), vec3(rotMat4[1]), vec3(rotMat4[2]));
    vec4 O;
    vec3 eyeD;
    vec3 du, dv, imagePlane;

    vec2 uv = (gl_TexCoord[0].xy/gl_TexCoord[0].w);
    uv[0] *= aspectRatio;

    O = rotMat4*(vec4(-translateVec3[0], -translateVec3[1], 4-translateVec3[2], 1));
    eyeO = O.xyz/O.w;
    eyeD = rotateMat*normalize(vec3(uv, -2.0));

    vec2 range = intersectBox(eyeO, eyeD, boxMin, boxMax);
    // range.x is the nearest point
    // range.y is the farthest point

    vec3 start = vec3(0);
    vec3 end = vec3(0);

    if (range.x<0)
    range.x = 0;

    if (range.x>range.y)
    gl_FragColor = vec4(0, 0, 0, 1);
    else
    {
        vec3 scale = vec3(1.0/(nsizeX*2.0), 1.0/(nsizeY*2.0), 1.0/(nsizeZ*2.0));
        vec3 offset = vec3(nsizeX, nsizeY, nsizeZ);
        start = ((eyeO + eyeD*range.x)+offset)*scale;
        end = ((eyeO + eyeD*range.y)+offset)*scale;
        gl_FragColor = Rendering(start, end, 0, 0, 0);
    }
}

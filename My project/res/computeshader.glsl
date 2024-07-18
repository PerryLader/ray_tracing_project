#version 420
#extension GL_ARB_shader_storage_buffer_object : enable
#extension GL_ARB_compute_shader : enable

//#extension GL_NV_shader_atomic_float : enable
#define COMPUTE_SHADER
layout(local_size_x = 8,local_size_y = 128,local_size_z = 1) in;



struct Triangle {
    vec3 v0;
    vec3 v1;
    vec3 v2;
    vec3 norm;
};
struct Ray {
    vec3 dir;    
};

layout(std430, binding = 0) buffer Triangles {
    Triangle triangles[];
};

layout(std430, binding = 1) buffer Rays {
    Ray rays[];
};

layout(std430, binding = 2) buffer Results {
    float distances[];
};

uniform vec3 rayOrigin;
uniform int numRays;
uniform float maxDistance;
uniform int numTriangles;
shared float sharedData[8];

float rayIntersectsTriangle(vec3 orig,vec3 dir, vec3 v0, vec3 v1, vec3 v2,vec3 norm)
{
    const vec3 e0 = v1 - v0;
    const vec3 e1 = v0 - v2;
    vec3 barycentricCoord;
    float temp=( 1.0 / dot( norm,dir ) ) ;
    const vec3 e2 =temp * ( v0 - orig );
    const vec3 i  = cross( dir, e2 );

    barycentricCoord.y = dot( i, e1 );
    barycentricCoord.z = dot( i, e0 );
    barycentricCoord.x = 1.0 - (barycentricCoord.z + barycentricCoord.y);
    float new_t   = dot( norm, e2 );
    
    if ((new_t < maxDistance) && new_t > 0.01&&  all(greaterThanEqual(barycentricCoord, vec3(0.0))))
      {
        return new_t; 
      } 
    return maxDistance;   
}

void main() {
   // uint triangleIndex = gl_GlobalInvocationID.x;
    uint rayIndex = gl_GlobalInvocationID.y;
       
    Triangle tri;
    float new_Distance;
    float curClosesDistance=maxDistance;
    uint len=numTriangles/8;
    uint blockNum=gl_LocalInvocationID.x;
    if( (rayIndex<numRays))
    {
        uint firstInBlock=blockNum*len;

        for(uint i=firstInBlock;i<firstInBlock+len+1;i++)
        {
            tri = triangles[i];
            new_Distance=rayIntersectsTriangle(rayOrigin,normalize(rays[rayIndex].dir), tri.v0, tri.v1, tri.v2,tri.norm);
            if(curClosesDistance>new_Distance)
            {
                curClosesDistance=new_Distance;
            }
        }
        sharedData[blockNum]=curClosesDistance;
            memoryBarrierShared ();
        if(blockNum==0)
        {
            float minVal=maxDistance;
            for(int i=0;i<8;i++)
            {
                tri = triangles[i];
                new_Distance=sharedData[i];
                if(minVal>new_Distance)
                {
                    minVal=new_Distance;
                }
            }
            distances[rayIndex]=minVal;
        }
    }
}
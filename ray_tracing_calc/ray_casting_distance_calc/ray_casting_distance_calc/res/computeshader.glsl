#version 420
#extension GL_ARB_shader_storage_buffer_object : enable
#extension GL_ARB_compute_shader : enable
#define COMPUTE_SHADER

layout(local_size_x = 8,local_size_y = 1,local_size_z = 1) in;



struct Triangle {
    vec4 v0;
    vec4 v1;
    vec4 v2;
    vec4 norm;
};
struct Ray {
    vec4 dir;    
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

uniform vec4 rayOrigin;
uniform int numRays;
uniform float maxDistance;
uniform int numTriangles;
shared float sharedData[8];





float PointInOrOn( vec3 P1, vec3 P2, vec3 A, vec3 B )
{
    vec3 CP1 = cross(B - A, P1 - A);
    vec3 CP2 = cross(B - A, P2 - A);
    return step(0.0, dot(CP1, CP2));
}

bool PointInTriangle( vec3 px, vec3 p0, vec3 p1, vec3 p2 )
{
    return bool( PointInOrOn(px, p0, p1, p2) *
        PointInOrOn(px, p1, p2, p0) *
        PointInOrOn(px, p2, p0, p1))
       ;
}

// Check intersection point
vec3 IntersectPlane(vec3 orig,vec3 dir, vec3 p1, vec3 p2, vec3 p3,vec3 norm) {
    // Direction
    vec3 D = dir;

    vec3 X = orig + D * dot(p1 - orig, norm) / dot(D, norm);
    
    // Return coordinate of intersect point
    return X;
}




float rayIntersectsTriangle(vec3 orig,vec3 dir, vec3 v0, vec3 v1, vec3 v2,vec3 norm)
{
    vec3 x = IntersectPlane( orig, dir, v0, v1, v2, norm);
    if(PointInTriangle(x, v0, v1, v2))
    {
      vec3 diff=x-orig;
      float l=sqrt(dot(diff,diff));
      if(l<maxDistance&&l>0.5)
      {
        return l;
      }
    }
     return maxDistance;
}

vec3 vec4tovec3(vec4 v)
{
    return vec3(v.x,v.y,v.z);
}

void main() {
   
    uint rayIndex = gl_WorkGroupID.y;
       
    Triangle tri;
    float new_Distance;
    float curClosesDistance=maxDistance;
    uint len=numTriangles/8;
    uint blockNum=gl_LocalInvocationID.x;
    if( (rayIndex<numRays)&&blockNum<8)
    {
        uint firstInBlock=blockNum*len;

        for(uint i=firstInBlock;i<firstInBlock+len;i++)
        {
            tri = triangles[i];
            new_Distance=rayIntersectsTriangle(
            vec4tovec3(rayOrigin),
            vec4tovec3(normalize(rays[rayIndex].dir)),
            vec4tovec3(tri.v0),
            vec4tovec3(tri.v1), 
            vec4tovec3(tri.v2),
            vec4tovec3(tri.norm)
            );
        
            if(curClosesDistance>new_Distance)
            {
                curClosesDistance=new_Distance;
            }
        }

        sharedData[blockNum]=curClosesDistance;
        //sharedData[blockNum]=normalize(rays[rayIndex].dir).y;
        
        memoryBarrierShared ();
        barrier();

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
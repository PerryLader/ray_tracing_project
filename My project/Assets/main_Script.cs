using UnityEngine;


struct Triangle
{
    public Vector4 v0;
    public Vector4 v1;
    public Vector4 v2;
    public Vector4 norm;
}
struct Ray
{
    public Vector4 dir;
};


public class main_script : MonoBehaviour
{
    public ComputeShader computeShader;
    public Vector3 rayOrigin;
    public int numRays;
    public float maxDistance;
    public int numTriangles;
    Triangle[] triangles;
    Ray[] rays;
    float[] distances;
    private ComputeBuffer trianglesBuffer;
    private ComputeBuffer raysBuffer;
    private ComputeBuffer distancesBuffer;

    void Start()
    {
        // Initialize data
         triangles = new Triangle[numTriangles];
         rays = new Ray[numRays];
         distances = new float[numRays];

        // Fill data (example initialization, replace with actual data)

            



        //for (int i = 0; i < numTriangles; i++)
        //{
        //    triangles[i] = new Triangle
        //    {
        //        v0 = Random.insideUnitSphere,
        //        v1 = Random.insideUnitSphere,
        //        v2 = Random.insideUnitSphere,
        //        norm = Random.insideUnitSphere
        //    };
        //}

        //for (int i = 0; i < numRays; i++)
        //{
        //    rays[i] = new Ray
        //    {
        //        dir = Random.onUnitSphere
        //    };
        //}

        // Create buffers
        trianglesBuffer = new ComputeBuffer(numTriangles, sizeof(float) * 12); // 3 * 4 * 4
        raysBuffer = new ComputeBuffer(numRays, sizeof(float) * 3);
        distancesBuffer = new ComputeBuffer(numRays, sizeof(float));

        // Set data to buffers
        trianglesBuffer.SetData(triangles);
        raysBuffer.SetData(rays);
        distancesBuffer.SetData(distances);

        // Set buffers and uniforms to the compute shader
        computeShader.SetBuffer(0, "triangles", trianglesBuffer);
        computeShader.SetBuffer(0, "rays", raysBuffer);
        computeShader.SetBuffer(0, "distances", distancesBuffer);
        computeShader.SetVector("rayOrigin", rayOrigin);
        computeShader.SetInt("numRays", numRays);
        computeShader.SetFloat("maxDistance", maxDistance);
        computeShader.SetInt("numTriangles", numTriangles);

        // Dispatch the compute shader
        int threadGroupsX = Mathf.CeilToInt(8.0f);
        int threadGroupsY = Mathf.CeilToInt(numRays / 128.0f);
        computeShader.Dispatch(0, threadGroupsX, threadGroupsY, 1);

        // Retrieve the result
        distancesBuffer.GetData(distances);

        // Cleanup
        trianglesBuffer.Release();
        raysBuffer.Release();
        distancesBuffer.Release();
    }
}

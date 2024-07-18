using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System;
using UnityEngine;

public class DrawLines : MonoBehaviour
{
    public Vector3[] linePoints1;
    public Vector3[] linePoints2;
    public int numberOfLines = 50;
    public float range = 10f;
    float[][] dataArray;







    public Vector3 rayOrigin;
    public Vector3 rayDirection;
    public float maxDistance = 1000;
    Vector3[] positions;

    void computeRayHitdistance()
    {
        Process process = new Process();

        // Configure the process start information
        process.StartInfo.FileName = "Assets/calc/ray_casting_distance_calc.exe";
        process.StartInfo.Arguments = ""; // Add any command-line arguments if needed
        process.StartInfo.RedirectStandardOutput = true;
        process.StartInfo.RedirectStandardError = true;
        process.StartInfo.UseShellExecute = false;
        process.StartInfo.CreateNoWindow = true;

        try
        {
            // Start the process
            process.Start();

            // Read the output streams
            string output = process.StandardOutput.ReadToEnd();
            string error = process.StandardError.ReadToEnd();

            // Wait for the process to exit
            process.WaitForExit();

            // Get the exit code of the process
            int exitCode = process.ExitCode;
            UnityEngine.Debug.Log($"The application exited with code: {exitCode}");

            // Write output to a text file
            string outputFilePath = "calc_App_output.txt";
            File.WriteAllText(outputFilePath, output);

            // Optionally log errors to another file or handle them as needed
            if (!string.IsNullOrEmpty(error))
            {
                string errorFilePath = "calc_App_error.txt";
                File.WriteAllText(errorFilePath, error);
            }
        }
        catch (Exception ex)
        {
            UnityEngine.Debug.Log($"An error occurred: {ex.Message}");
        }
    }
    void setRays()
    {
        int azimuthSteps = 360 / 10;
        int elevationSteps = 180 / 10;
        int arraySize = azimuthSteps * elevationSteps;
        positions = new Vector3[arraySize];
        int index = 0;
        for (int az = 0; az < azimuthSteps; az++)
        {
            for (int el = 0; el < elevationSteps; el++)
            {

                float azimuth = Mathf.Deg2Rad * (az * 10);
                float elevation = Mathf.Deg2Rad * (el * 10);


                float x = Mathf.Sin(elevation) * Mathf.Cos(azimuth);
                float y = Mathf.Abs(Mathf.Cos(elevation));
                float z = Mathf.Sin(elevation) * Mathf.Sin(azimuth);
                positions[index] = new Vector3(x, y, z);
                index++;
            }
        }
    }
    void addColiders()
    {
        GameObject[] objectsToAddColliders = GameObject.FindGameObjectsWithTag("building");

        foreach (GameObject obj in objectsToAddColliders)
        {

            if (obj.GetComponent<MeshCollider>() == null)
            {
                MeshCollider collider = obj.AddComponent<MeshCollider>();

            }
        }
    }
    void getRaysHit()
    {
        string filePath = "res/data.csv";
        string[] lines = File.ReadAllLines(filePath);
        List<float[]> dataList = new List<float[]>();
        foreach (string line in lines)
        {
            string[] stringValues = line.Split(',');
            float[] doubleValues = Array.ConvertAll(stringValues, float.Parse);
            dataList.Add(doubleValues);
        }
        dataArray = dataList.ToArray();

        return;
    }
    void Start()
    {
        Thread childThread = new Thread(new ThreadStart(computeRayHitdistance));
        childThread.Start();
        setRays();
        addColiders();
        childThread.Join();
        // Execution resumes here after the child thread finishes
        UnityEngine.Debug.Log("Parent thread continues after child thread finishes.");
        string outputFilePath = "main_output.txt";
        using (StreamWriter writer = new StreamWriter(outputFilePath))
        {
            getRaysHit();
            foreach (var row in dataArray)
            {
                string line = string.Join(", ", row);
                writer.WriteLine(line);
            }
        }

    }
    void Update()
    {
        CheckRaycastDistance();
        CreateLine(linePoints1, Color.red);
        CreateLine(linePoints2, Color.blue);
    }
    void CheckRaycastDistance()
    {
        foreach (Vector3 dir in positions)
        {
            Ray ray = new Ray(rayOrigin, dir);
            RaycastHit hit;

            if (Physics.Raycast(ray, out hit, maxDistance))
            {

                // If the ray hits an object, log the distance

                //UnityEngine.Debug.Log("Hit object: " + hit.collider.name + " at distance: " + hit.distance);
                // UnityEngine.Debug.DrawRay(rayOrigin, dir * hit.distance, Color.red);
            }
            else
            {
                // If the ray does not hit any object, log that information
                // UnityEngine.Debug.Log("No object hit within the maximum distance.");
                //UnityEngine.Debug.DrawRay(rayOrigin, dir * maxDistance, Color.blue);

            }
        }

    }


















    void Start()
    {
        // Initialize linePoints1 with random values
        linePoints1 = new Vector3[numberOfLines * 2];
        for (int i = 0; i < numberOfLines * 2; i++)
        {
            linePoints1[i] = new Vector3(
                Random.Range(-range, range),
                Random.Range(-range, range),
                Random.Range(-range, range)
            );
        }

        // Initialize linePoints2 with random values
        linePoints2 = new Vector3[numberOfLines * 2];
        for (int i = 0; i < numberOfLines * 2; i++)
        {
            linePoints2[i] = new Vector3(
                Random.Range(-range, range),
                Random.Range(-range, range),
                Random.Range(-range, range)
            );
        }

        CreateLine(linePoints1, Color.red);
        CreateLine(linePoints2, Color.blue);
    }

    void CreateLine(Vector3[] positions, Color color)
    {
        GameObject lineObject = new GameObject("Line");
        LineRenderer lineRenderer = lineObject.AddComponent<LineRenderer>();

        lineRenderer.startWidth = 0.1f;
        lineRenderer.endWidth = 0.1f;
        lineRenderer.positionCount = positions.Length;
        lineRenderer.material = new Material(Shader.Find("Sprites/Default"));
        lineRenderer.startColor = color;
        lineRenderer.endColor = color;

        lineRenderer.SetPositions(positions);
    }
}

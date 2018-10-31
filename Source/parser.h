#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <math.h>
#include "vector3.hpp"

enum TransformationType
{
	TRANSLATION,
	ROTATION,
	SCALING
};

enum MeshType
{
	SOLID,
	WIREFRAME
};

namespace parser
{
    //Notice that all the structures are as simple as possible
    //so that you are not enforced to adopt any style or design.
    struct Vec3f
    {
        float x, y, z;
    };
    
    struct Vertex
    {
    	Vector3 position;
    	Vector3 normal;
    };

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct Camera
    {
        Vec3f position;
        Vector3 gaze;
        Vector3 up;
        Vec4f near_plane;
        float near_distance;
        float far_distance;
        int image_width, image_height;
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
        bool status;
    };

    struct Material
    {
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        float phong_exponent;
    };

    struct Transformation
    {
        TransformationType type;
        int id;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;
        std::vector<Transformation> transformations;
        MeshType type;
    };

    struct Scene
    {
        //Data
        Vec3i background_color;
        Camera camera;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vertex> vertex_data;
        std::vector<Vec3f> translations;
        std::vector<Vec3f> scalings;
        std::vector<Vec4f> rotations;
        std::vector<Mesh> meshes;

        //Functions
        void loadFromXml(const std::string& filepath);
    };
}

#endif

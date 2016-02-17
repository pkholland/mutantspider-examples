
#include "mutantspider.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags


template<typename T>
T& operator<<(T& str, const aiColor4D& c)
{
    return str << "<" << c.r << ", " << c.b << ", " << c.b << ", " << c.a << ">";
}

template<typename T>
T& operator<<(T& str, const aiString& s)
{
    return str << s.C_Str();
}

class W3DInstance : public MS_AppInstance
{
public:
    explicit W3DInstance(MS_Instance instance)
        : MS_AppInstance(instance)
    {}

    virtual bool Init(uint32_t argc, const char* argn[], const char* argv[])
    {
        std::vector<std::string> persistent_dirs;
        mutantspider::init_fs(this, persistent_dirs);
        return true;
    }
    
    virtual void AsyncStartupComplete()
    {
        Assimp::Importer importer;
        // And have it read the given file with some example postprocessing
        // Usually - if speed is not the most important aspect for you - you'll 
        // propably to request more postprocessing than we do in this example.
        const aiScene* scene = importer.ReadFile( "/resources/spider.obj",
            aiProcess_CalcTangentSpace       | 
            aiProcess_Triangulate            |
            aiProcess_JoinIdenticalVertices  |
            aiProcess_SortByPType);

        // If the import failed, report it
        if( !scene)
        {
            ms_log(importer.GetErrorString());
            return;
        }
        
        ms_log("hasAnimations: " << scene->HasAnimations());
        ms_log("hasCameras: " << scene->HasCameras());
        ms_log("hasLights: " << scene->HasLights());
        ms_log("hasMaterials: " << scene->HasMaterials() << ", num: " << scene->mNumMaterials);
        #if 0
        for (int i = 0; i < scene->mNumMaterials; i++)   {
            ms_log("  material " << i << " has " << scene->mMaterials[i]->mNumProperties << " properties, " << scene->mMaterials[i]->mNumAllocated << " allocated");
            aiString name;
            if (scene->mMaterials[i]->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS)
                ms_log("    material name: " << name);
            else
                ms_log("    material name missing");
            
            int shading;
            if (scene->mMaterials[i]->Get(AI_MATKEY_SHADING_MODEL, shading) == aiReturn_SUCCESS) {
                switch(shading) {
                    case aiShadingMode_Phong:
                        ms_log("    shading model: phong");
                        break;
                    case aiShadingMode_Blinn:
                        ms_log("    shading model: blinn");
                        break;
                    case aiShadingMode_NoShading:
                        ms_log("    shading model: constant");
                        break;
                    case aiShadingMode_Gouraud:
                        ms_log("    shading model: gouraud");
                        break;
                    default:
                        ms_log("    shading model: " << shading);
                        break;
                }
            } else
                ms_log("    shading model missing");
            
            aiColor4D c;
            if(scene->mMaterials[i]->Get(AI_MATKEY_COLOR_AMBIENT,c) == aiReturn_SUCCESS)
                ms_log("    ambient color: " << c);
            else
                ms_log("    ambient color missing");
            
            if(scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE,c) == aiReturn_SUCCESS)
                ms_log("    diffuse color: " << c);
            else
                ms_log("    diffuse color missing");

            if(scene->mMaterials[i]->Get(AI_MATKEY_COLOR_SPECULAR,c) == aiReturn_SUCCESS)
                ms_log("    specular color: " << c);
            else
                ms_log("    specular color missing");
            
            if(scene->mMaterials[i]->Get(AI_MATKEY_COLOR_EMISSIVE,c) == aiReturn_SUCCESS)
                ms_log("    emissive color: " << c);
            else
                ms_log("    emissive color missing");
            
            float f;
            if(scene->mMaterials[i]->Get(AI_MATKEY_SHININESS,f) == aiReturn_SUCCESS)
                ms_log("    shininess: " << f);
            else
                ms_log("    shininess missing");

            if(scene->mMaterials[i]->Get(AI_MATKEY_OPACITY,f) == aiReturn_SUCCESS)
                ms_log("    opacity: " << f);
            else
                ms_log("    opacity missing");
            
            if(scene->mMaterials[i]->Get(AI_MATKEY_REFRACTI,f) == aiReturn_SUCCESS)
                ms_log("    refraction: " << f);
            else
                ms_log("    refraction missing");
            
            aiString relpath;
            if (scene->mMaterials[i]->Get(_AI_MATKEY_TEXTURE_BASE, aiTextureType_DIFFUSE, 0, relpath) == aiReturn_SUCCESS)
                ms_log("    texture file: " << relpath);
            else
                ms_log("    texture file missing");

            #if 0
            for (int p = 0; p < scene->mMaterials[i]->mNumProperties; p++)
                ms_log("    " << scene->mMaterials[i]->mProperties[p]->mKey.C_Str());
            #endif
        }
        #endif
        ms_log("hasMeshes: " << scene->HasMeshes() << ", num: " << scene->mNumMeshes);
        
        for (int i = 0; i < scene->mNumMeshes; i++)   {
            ms_log("  mesh " << i);
            ms_log("    GetNumColorChannels:      " << scene->mMeshes[i]->GetNumColorChannels());
            ms_log("    GetNumUVChannels:         " << scene->mMeshes[i]->GetNumUVChannels());
            ms_log("    HasBones:                 " << scene->mMeshes[i]->HasBones());
            ms_log("    numFaces:                 " << scene->mMeshes[i]->mNumFaces);
            ms_log("    numNormals:               " << (scene->mMeshes[i]->mNormals ? scene->mMeshes[i]->mNumVertices : 0) );
            ms_log("    HasPositions:             " << scene->mMeshes[i]->HasPositions());
            ms_log("    HasTangentsAndBitangents: " << scene->mMeshes[i]->HasTangentsAndBitangents());
            //ms_log("    HasTextureCoords:         " << scene->mMeshes[i]->HasTextureCoords());
            //ms_log("    HasVertexColors:          " << scene->mMeshes[i]->HasVertexColors());
            aiString name;
            scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->Get(AI_MATKEY_NAME, name);
            ms_log("    mMaterialIndex:           " << scene->mMeshes[i]->mMaterialIndex << " (" << name << ")");
            ms_log("    mName:                    " << scene->mMeshes[i]->mName);
            ms_log("    mColors:                  " << scene->mMeshes[i]->mColors);
            ms_log("    mPrimitiveTypes:          " << scene->mMeshes[i]->mPrimitiveTypes);
            
            int maxFaces = scene->mMeshes[i]->mNumFaces;
            if (maxFaces > 4)
                maxFaces = 4;
            for (int f = 0; f < maxFaces; f++)
                ms_log("      face " << f << " has " << scene->mMeshes[i]->mFaces[f].mNumIndices << " indices, starting with: " << scene->mMeshes[i]->mFaces[f].mIndices[0] << ", " << scene->mMeshes[i]->mFaces[f].mIndices[1] << ", " << scene->mMeshes[i]->mFaces[f].mIndices[2]);
        }
        
        
        ms_log("hasTextures: " << scene->HasTextures());
        
        // Now we can access the file's contents. 
        // DoTheSceneProcessing( scene);
        // We're done. Everything will be cleaned up by the importer destructor
    }

};

class W3DModule : public MS_Module
{
public:
    virtual MS_AppInstancePtr CreateInstance(MS_Instance instance)
    {
        mutantspider::gAppInstance = new W3DInstance(instance);
        return mutantspider::gAppInstance;
    }
};

namespace pp {

MS_Module* CreateModule() { return new W3DModule(); }

}

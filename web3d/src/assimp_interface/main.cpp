/*
 * main.cpp
 *
 * js -> c interface for assimp
 */

#include "main.h"
#include "mutantspider.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>


// json_exporter.cpp
extern Assimp::Exporter::ExportFormatEntry Assimp2Json_desc;

// helper to format failure messages
template<typename Func>
static void _reject(int id, Func func)
{
  std::ostringstream format;
  func(format);
  ai_reject_promise(id, format.str().c_str());
}

#define reject_promise(id, _body) _reject(id, [&](std::ostream& formatter) {formatter << _body;})

// we put all of the external file data inside of "/.models"
static std::string gModels("/.models");

std::string mkpth(const char* path)
{
  if (strstr(path, "/resources/") == path)
    return path;
  return gModels + (path[0] == '/' ? "" : "/") + path;
}

extern "C" {

void MS_Init(const char* init_args)
{
  mutantspider::init_fs();
  
  // the set of directories that we want to be "persistent"
  // that is, file io done in these directories (or subdirectories of them)
  // will persist across page loads in the browser.
  std::vector<std::string> persistent_dirs;
  mutantspider::mount_fs(persistent_dirs);
}

void MS_AsyncStartupComplete()
{
  // file system, and in particular the _async_ persistent part, is ready to use.
  if (mkdir(gModels.c_str(),0777) != 0)
    ms_log("mkdir(\"/.models\",0777) failed on startup!");
}

// "rm -rf dir_name"
void rm_rf(const std::string& dir_name)
{
  DIR	*dir;
  bool unlinked;
  do {
    unlinked = false;
    if ((dir = opendir(dir_name.c_str())) != 0) {
      struct dirent *ent;
      while ((ent = readdir(dir)) != 0) {
        if (strcmp(ent->d_name,".") && strcmp(ent->d_name,"..")) {
          std::string path = dir_name + "/" + ent->d_name;
          struct stat st;
          if (stat(path.c_str(),&st) == 0) {
            if (S_ISDIR(st.st_mode)) {
              unlinked = true;
              rm_rf(path);
            } else {
              unlinked = true;
              int ret = unlink(path.c_str());
            }
          }
        }
      }
      closedir(dir);
    }
  } while (unlinked);
  int ret = rmdir(dir_name.c_str());
}

void AI_ClearFileSystem()
{
  rm_rf(gModels);
  if (mkdir(gModels.c_str(),0777) != 0)
    ms_log("mkdir(\"/.models\",0777) failed on clear!");
}

void AI_AddFile(int id, void* fileBuffer, size_t fileBuffer_size, ms_transfered_buffer* fileBuffer_free,
                            const char * path)
{
  auto fd = open(mkpth(path).c_str(),O_RDWR | O_CREAT, 0666);
  if (fd == -1)
    reject_promise(id, "open(\"" << path << "\",O_RDWR | O_CREAT, 0666) failed with errno: " << errno);
  else {
    auto bytes_written = write(fd, fileBuffer, fileBuffer_size);
    if (bytes_written != fileBuffer_size)
      reject_promise(id, "write to file \"" << path << "\" only wrote " << bytes_written << " of " << fileBuffer_size << " bytes");
    else
      ai_resolve_fs_promise(id);
    close(fd);
  }
  ms_free_transfered_buffer(fileBuffer_free, fileBuffer);
}

void AI_AddDirectory(int id, const char * path)
{
  if (mkdir(mkpth(path).c_str(),0777) != 0)
    reject_promise(id, "mkdir(\"" << path << "\",0777) failed with errno: " << errno);
  else
    ai_resolve_fs_promise(id);
}

void AI_ImportAsset(int id, const char* file_name)
{
  Assimp::Importer importer;
  // And have it read the given file with some example postprocessing
  // Usually - if speed is not the most important aspect for you - you'll
  // propably to request more postprocessing than we do in this example.
  const aiScene* scene = importer.ReadFile( mkpth(file_name).c_str(),
           // aiProcess_CalcTangentSpace        |
            aiProcess_Triangulate             |
            aiProcess_GenNormals              |
           // aiProcess_MakeLeftHanded          |
           // aiProcess_JoinIdenticalVertices   |
           // aiProcess_OptimizeMeshes          |
           // aiProcess_OptimizeGraph           |
           // aiProcess_SortByPType             |
            0);

  // If the import failed, report it
  if( !scene)
  {
    ai_reject_promise(id, importer.GetErrorString());
    return;
  }
  
	Assimp::Exporter exp;
	exp.RegisterExporter(Assimp2Json_desc);
  
  const aiExportDataBlob* const blob = exp.ExportToBlob(scene,"assimp.json");
  if(!blob) {
    ai_reject_promise(id, exp.GetErrorString());
    return;
  }

  const std::string json_str(static_cast<char*>(blob->data), blob->size);
  ai_resolve_asset_promise(id, json_str.c_str());

}

}






/////////////////////////////////////////////////////////////////////

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


  #if 0
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
  
  #endif


#pragma once
//
// Простой загрузчик .obj/.fbx -> Mesh3D (первый меш)

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <vector>

#include "../Managers/Items/components_3d.h"

namespace EDD::Tools {

inline bool Create_Mesh3D_FromFile(const char* path, EDD::Managers::Mesh3D& out_mesh) {
  Assimp::Importer L_importer;
  const aiScene* L_scene = L_importer.ReadFile(
      path,
      aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices |
          aiProcess_CalcTangentSpace);
  if (!L_scene || !L_scene->mRootNode || L_scene->mNumMeshes == 0) return false;

  aiMesh* L_mesh = L_scene->mMeshes[0];

  std::vector<EDD::Managers::VertexPNUTB> L_vertices;
  L_vertices.reserve(L_mesh->mNumVertices);
  for (unsigned i = 0; i < L_mesh->mNumVertices; ++i) {
    EDD::Managers::VertexPNUTB L_v{};
    L_v.pos_[0] = L_mesh->mVertices[i].x;
    L_v.pos_[1] = L_mesh->mVertices[i].y;
    L_v.pos_[2] = L_mesh->mVertices[i].z;

    if (L_mesh->HasNormals()) {
      L_v.normal_[0] = L_mesh->mNormals[i].x;
      L_v.normal_[1] = L_mesh->mNormals[i].y;
      L_v.normal_[2] = L_mesh->mNormals[i].z;
    }
    if (L_mesh->HasTextureCoords(0)) {
      L_v.uv_[0] = L_mesh->mTextureCoords[0][i].x;
      L_v.uv_[1] = L_mesh->mTextureCoords[0][i].y;
    } else {
      L_v.uv_[0] = L_v.uv_[1] = 0.0f;
    }
    L_vertices.push_back(L_v);
  }

  std::vector<uint32_t> L_indices;
  L_indices.reserve(L_mesh->mNumFaces * 3);
  for (unsigned f = 0; f < L_mesh->mNumFaces; ++f) {
    const aiFace& L_face = L_mesh->mFaces[f];
    if (L_face.mNumIndices == 3) {
      L_indices.push_back(L_face.mIndices[0]);
      L_indices.push_back(L_face.mIndices[1]);
      L_indices.push_back(L_face.mIndices[2]);
    }
  }

  auto L_layout = EDD::Managers::VertexPNUTB::Get_Layout();
  const bgfx::Memory* L_vmem = bgfx::copy(
      L_vertices.data(),
      uint32_t(L_vertices.size() * sizeof(EDD::Managers::VertexPNUTB)));
  const bgfx::Memory* L_imem = bgfx::copy(L_indices.data(),
                                          uint32_t(L_indices.size() * sizeof(uint32_t)));

  out_mesh.vbh_ = bgfx::createVertexBuffer(L_vmem, L_layout);
  out_mesh.ibh_ = bgfx::createIndexBuffer(L_imem);
  out_mesh.index_count_ = (uint32_t)L_indices.size();
  out_mesh.is_loaded_ = bgfx::isValid(out_mesh.vbh_) && bgfx::isValid(out_mesh.ibh_);
  return out_mesh.is_loaded_;
}

}  // namespace EDD::Tools

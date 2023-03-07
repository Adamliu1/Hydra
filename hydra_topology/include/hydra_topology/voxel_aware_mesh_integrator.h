/* -----------------------------------------------------------------------------
 * Copyright 2022 Massachusetts Institute of Technology.
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Research was sponsored by the United States Air Force Research Laboratory and
 * the United States Air Force Artificial Intelligence Accelerator and was
 * accomplished under Cooperative Agreement Number FA8750-19-2-1000. The views
 * and conclusions contained in this document are those of the authors and should
 * not be interpreted as representing the official policies, either expressed or
 * implied, of the United States Air Force or the U.S. Government. The U.S.
 * Government is authorized to reproduce and distribute reprints for Government
 * purposes notwithstanding any copyright notation herein.
 * -------------------------------------------------------------------------- */
#pragma once
#include <voxblox/mesh/mesh_integrator.h>

#include "hydra_topology/gvd_voxel.h"
#include "hydra_topology/voxblox_types.h"

namespace hydra {
namespace topology {

class VoxelAwareMeshIntegrator : public voxblox::MeshIntegrator<TsdfVoxel> {
 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  VoxelAwareMeshIntegrator(const voxblox::MeshIntegratorConfig& config,
                           Layer<TsdfVoxel>* sdf_layer,
                           const Layer<VertexVoxel>::Ptr& vertex_layer,
                           MeshLayer* mesh_layer);

  virtual ~VoxelAwareMeshIntegrator() = default;

  virtual void generateMesh(bool only_mesh_updated_blocks,
                            bool clear_updated_flag) override;

  virtual void extractMeshInsideBlock(const Block<TsdfVoxel>& block,
                                      const VoxelIndex& index,
                                      const voxblox::Point& coords,
                                      VertexIndex* next_mesh_index,
                                      Mesh* mesh) override;

  virtual void extractMeshOnBorder(const Block<TsdfVoxel>& block,
                                   const VoxelIndex& index,
                                   const voxblox::Point& coords,
                                   VertexIndex* next_mesh_index,
                                   Mesh* mesh) override;

  void processInterior(const BlockIndexList& blocks, ThreadSafeIndex* index_getter);

  void processExterior(const BlockIndexList& blocks, ThreadSafeIndex* index_getter);

  void updateBlockInterior(const BlockIndex& block_index);

  void updateBlockExterior(const BlockIndex& block_index);

  void launchThreads(const BlockIndexList& blocks, bool interior_pass);

  BlockIndex getNeighborBlockIndex(const BlockIndex& block_idx,
                                   VoxelIndex& corner_index);

 protected:
  Layer<VertexVoxel>::Ptr vertex_layer_;

  Eigen::Matrix<FloatingPoint, 3, 8> cube_coord_offsets_;
};

}  // namespace topology
}  // namespace hydra

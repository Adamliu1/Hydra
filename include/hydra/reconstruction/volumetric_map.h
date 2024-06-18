// The contents of this file are originally from Panoptic-Mapping,
// under the following license:
//
// BSD 3-Clause License
// Copyright (c) 2021, ETHZ ASL
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// See https://github.com/ethz-asl/panoptic_mapping for original code and paper
//
// Modifications (including work done by Lukas Schmid for Khronos) fall under the same
// license as Hydra and are subject to the following copyright and disclaimer:
//
// Copyright 2022 Massachusetts Institute of Technology.
// All Rights Reserved
//
// Research was sponsored by the United States Air Force Research Laboratory and
// the United States Air Force Artificial Intelligence Accelerator and was
// accomplished under Cooperative Agreement Number FA8750-19-2-1000. The views
// and conclusions contained in this document are those of the authors and should
// not be interpreted as representing the official policies, either expressed or
// implied, of the United States Air Force or the U.S. Government. The U.S.
// Government is authorized to reproduce and distribute reprints for Government
// purposes notwithstanding any copyright notation herein.
#pragma once

#include "hydra/reconstruction/voxel_types.h"

namespace hydra {

/**
 * @brief Merge all elements of a layer into another layer, overwriting data in the
 * other layer if it exists already. This assumes that layers have identical grid
 * layouts and block types.
 * @tparam Block Type of the block in both layers.
 * @param layer_in Input layer to merge.
 * @param layer_out Output layer to merge into.
 */
template <typename Block>
void mergeLayer(const spatial_hash::BlockLayer<Block>& layer_in,
                spatial_hash::BlockLayer<Block>& layer_out) {
  for (const auto& block_in : layer_in) {
    layer_out.allocateBlock(block_in.index) = block_in;
  }
}
template <typename Block>
void mergeLayer(const spatial_hash::VoxelLayer<Block>& layer_in,
                spatial_hash::VoxelLayer<Block>& layer_out) {
  for (const auto& block_in : layer_in) {
    layer_out.allocateBlock(block_in.index) = block_in;
  }
}

class VolumetricMap {
 public:
  struct Config {
    /// Voxel size.
    float voxel_size = 0.1f;
    /// Number of voxels per block side.
    int voxels_per_side = 16;  // TODO(nathan) fix int
    /// TSDF truncation distance.
    float truncation_distance = 0.3f;
  } const config;

  explicit VolumetricMap(const Config& config, bool with_semantics = false);

  virtual ~VolumetricMap() = default;

  TsdfLayer& getTsdfLayer() { return tsdf_layer_; }
  const TsdfLayer& getTsdfLayer() const { return tsdf_layer_; }

  MeshLayer& getMeshLayer() { return mesh_layer_; }
  const MeshLayer& getMeshLayer() const { return mesh_layer_; }

  SemanticLayer* getSemanticLayer() { return semantic_layer_.get(); }
  const SemanticLayer* getSemanticLayer() const { return semantic_layer_.get(); }

  bool hasSemantics() const { return semantic_layer_ != nullptr; }

  virtual void removeBlock(const BlockIndex& block);

  template <typename BlockIndices>
  void removeBlocks(const BlockIndices& blocks) {
    for (const auto& idx : blocks) {
      removeBlock(idx);
    }
  }

  virtual std::string printStats() const;

  void save(const std::string& filepath) const;

  static std::unique_ptr<VolumetricMap> fromTsdf(const TsdfLayer& tsdf,
                                                 double truncation_distance_m,
                                                 bool with_semantics = false);

  static std::unique_ptr<VolumetricMap> load(const std::string& filepath);

  virtual std::unique_ptr<VolumetricMap> clone() const;

  virtual void updateFrom(const VolumetricMap& other);

 protected:
  TsdfLayer tsdf_layer_;
  MeshLayer mesh_layer_;
  SemanticLayer::Ptr semantic_layer_;

 public:
  const float block_size;
  const float voxel_size_inv;
};

void declare_config(VolumetricMap::Config& config);

}  // namespace hydra

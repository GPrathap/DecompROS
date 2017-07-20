#include "mesh_visual.h"

namespace planning_rviz_plugins {
  MeshVisual::MeshVisual(Ogre::SceneManager *scene_manager,
                         Ogre::SceneNode *parent_node) {
    scene_manager_ = scene_manager;
    frame_node_ = parent_node->createChildSceneNode();
    obj_.reset(new rviz::MeshShape(scene_manager_, frame_node_));
    wall_.reset(new rviz::MeshShape(scene_manager_, frame_node_));
  }

  MeshVisual::~MeshVisual() { scene_manager_->destroySceneNode(frame_node_); }

  void MeshVisual::setMessage(const BoundVec3f &bds, const std::vector<bool>& passes) {
    obj_->clear();
    wall_->clear();

    if (bds.empty())
      return;
    bool has_wall = false;
    for(const auto& it: passes) {
      if(!it) {
        has_wall = true;
        break;
      }
    }

    obj_->beginTriangles();
    if(has_wall)
      wall_->beginTriangles();
    int free_cnt = 0;
    int wall_cnt = 0;
    int id = 0;
    for (const auto &vs: bds) {
      if (vs.size() > 2) {
	Vec3f p0 = vs[0];
	Vec3f p1 = vs[1];
	Vec3f p2 = vs[2];
	Vec3f n = (p2-p0).cross(p1-p0);
	n = n.normalized();
	if(std::isnan(n(0)))
	  n = Vec3f(0, 0, -1);

	if(passes.empty() || passes[id]) {
	  int ref_cnt = free_cnt;
	  Ogre::Vector3 normal(n(0), n(1), n(2));
	  for (unsigned int i = 0; i < vs.size(); i++) {
	    obj_->addVertex(Ogre::Vector3(vs[i](0), vs[i](1), vs[i](2)), normal);
	    if (i > 1 && i < vs.size())
	      obj_->addTriangle(ref_cnt, free_cnt - 1, free_cnt);
	    free_cnt++;
	  }
	}
	else {
	  int ref_cnt = wall_cnt;
	  Ogre::Vector3 normal(n(0), n(1), n(2));
	  for (unsigned int i = 0; i < vs.size(); i++) {
	    wall_->addVertex(Ogre::Vector3(vs[i](0), vs[i](1), vs[i](2)), normal);
	    if (i > 1 && i < vs.size())
	      wall_->addTriangle(ref_cnt, wall_cnt - 1, wall_cnt);
	    wall_cnt++;
	  }

	}
      }
      id ++;
    }
    if(has_wall)
      wall_->endTriangles();
    obj_->endTriangles();
  }

  // Position and orientation are passed through to the SceneNode.
  void MeshVisual::setFramePosition(const Ogre::Vector3 &position) {
    frame_node_->setPosition(position);
  }

  void MeshVisual::setFrameOrientation(const Ogre::Quaternion &orientation) {
    frame_node_->setOrientation(orientation);
  }

  void MeshVisual::setColor(float r, float g, float b, float a) {
    obj_->setColor(r, g, b, a);
    wall_->setColor(1, 0, 0, 0.8);
  }
}

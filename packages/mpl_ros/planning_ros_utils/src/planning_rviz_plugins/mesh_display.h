#include <boost/circular_buffer.hpp>

#include <planning_ros_msgs/Mesh.h>
#include <poly_utils/geometry_utils.h>
#include <rviz/message_filter_display.h>

#include <rviz/properties/color_property.h>
#include <rviz/properties/float_property.h>
#include <rviz/properties/int_property.h>
#include <rviz/properties/enum_property.h>
#include <rviz/visualization_manager.h>
#include <rviz/frame_manager.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreSceneManager.h>

#include <rviz/load_resource.h>

#include "mesh_visual.h"
#include "bound_visual.h"
#include "vector_visual.h"

namespace planning_rviz_plugins {
class MeshDisplay
    : public rviz::MessageFilterDisplay<planning_ros_msgs::Mesh> {
  Q_OBJECT
public:
  MeshDisplay();
  virtual ~MeshDisplay();

protected:
  virtual void onInitialize();

  virtual void reset();

private Q_SLOTS:
  void updateMeshColorAndAlpha();
  void updateBoundColorAndAlpha();
  void updateVsColorAndAlpha();
  void updateState();
  void updateScale();
  void updateVsScale();

private:
  void processMessage(const planning_ros_msgs::Mesh::ConstPtr &msg);
  void visualizeMessage(int state);
  void visualizeMesh();
  void visualizeBound();
  void visualizeVs();

  boost::circular_buffer<boost::shared_ptr<MeshVisual>> visuals_mesh_;
  boost::circular_buffer<boost::shared_ptr<BoundVisual>> visuals_bound_;
  boost::circular_buffer<boost::shared_ptr<VectorVisual>> visuals_vector_;

  rviz::ColorProperty *mesh_color_property_;
  rviz::ColorProperty *bound_color_property_;
  rviz::FloatProperty *alpha_property_;
  rviz::FloatProperty *scale_property_;
  rviz::FloatProperty *vs_scale_property_;
  rviz::ColorProperty *vs_color_property_;
  rviz::EnumProperty *state_property_;

  Ogre::Vector3 position_;
  Ogre::Quaternion orientation_;

  BoundVec3f vertices_;
  vec_E<std::pair<Vec3f, Vec3f>> vs_;
};

} // end namespace planning_rviz_plugins

#ifndef MESH_SHAPE_H
#define MESH_SHAPE_H

#include <rviz_rendering/objects/shape.hpp>

namespace Ogre
{
class ManualObject;
}

// TODO(JafarAbdi): This's taken from https://github.com/ros2/rviz for MoveIt 2 beta release -- remove when it's ported
namespace rviz_plugins
{
/** \brief This class allows constructing Ogre shapes manually, from triangle lists.

    For example:
    Assuming we have a set of mesh triangles represented like this:
    \verbatim
    struct Triangle
    {
      unsigned v1, v2, v3; // index for the 3 vertices that make up a triangle
    };
    std::vector<Triangle> triangles;
    std::vector<Ogre::Vector3> vertices;
    std::vector<Ogre::Vector3> normals; // normal at every vertex
    \endverbatim

    we can use this class to render the mesh as follows:
    \verbatim
    rviz::MeshShape *shape = new MeshShape(scene_manager);
    mesh->estimateVertexCount(vertices.size());
    mesh->beginTriangles();
    for (std::size_t i = 0 ; i < vertices.size() ; ++i)
      mesh->addVertex(vertices[i], normals[i]);
    for (std::size_t i = 0 ; i < triangles.size() ; ++i)
      mesh->addTriangle(triangles[i].v1, triangles[i].v2, triangles[i].v3);
    mesh->endTriangles();
    \endverbatim
 */
class MeshShapeMod : public rviz_rendering::Shape
{
public:
  /**
   * \brief Constructor
   *
   * @param scene_manager The scene manager this object is associated with
   * @param parent_node A scene node to use as the parent of this object.  If nullptr, uses the root scene node.
   */
  MeshShapeMod(Ogre::SceneManager* scene_manager, Ogre::SceneNode* parent_node = nullptr);
  ~MeshShapeMod() override;

  /* \brief Estimate the number of vertices ahead of time. */
  void estimateVertexCount(size_t vcount);

  /** \brief Start adding triangles to the mesh */
  void beginTriangles();

  /** \brief Add a vertex to the mesh (no normal defined). If using
      this function only (not using addTriangle()) it is assumed that
      triangles are added by specifying the 3 vertices in order (3
      consecutive calls to this function). This means there must be
      3*n calls to this function to add n triangles. If addTriangle()
      is used, indexing in the defined vertices is done. */
  void addVertex(const Ogre::Vector3& position);

  /** \brief Add a vertex to the mesh with a normal defined. If using
      this function only (not using addTriangle()) it is assumed that
      triangles are added by specifying the 3 vertices in order (3
      consecutive calls to this function). This means there must be
      3*n calls to this function to add n triangles.If addTriangle()
      is used, indexing in the defined vertices is done.  */
  void addVertex(const Ogre::Vector3& position, const Ogre::Vector3& normal);

  /** \brief Add a vertex to the mesh with normal and color defined. If using
      this function only (not using addTriangle()) it is assumed that
      triangles are added by specifying the 3 vertices in order (3
      consecutive calls to this function). This means there must be
      3*n calls to this function to add n triangles.If addTriangle()
      is used, indexing in the defined vertices is done. */
  void addVertex(const Ogre::Vector3& position, const Ogre::Vector3& normal, const Ogre::ColourValue& color);

  /** \brief Add normal for a vertex */
  void addNormal(const Ogre::Vector3& normal);

  /** \brief Add color for a vertex */
  void addColor(const Ogre::ColourValue& color);

  /** \brief Add a triangle by indexing in the defined vertices. */
  void addTriangle(unsigned int p1, unsigned int p2, unsigned int p3);

  /** \brief Notify that the set of triangles to add is complete. No more triangles can be added, beginTriangles() can
   * no longer be called unless clear() was called. */
  void endTriangles();

  /** \brief Clear the mesh */
  void clear();

  /** \brief Get the manual object created for the mesh */
  Ogre::ManualObject* getManualObject()
  {
    return manual_object_;
  }

private:
  // true in between calls to beginTriangles() and endTriangles()
  bool started_;
  Ogre::ManualObject* manual_object_;
};

}  // namespace rviz_rendering


#endif
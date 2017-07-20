#include "bag_reader.hpp"
#include "txt_reader.hpp"
#include <decomp_ros_utils/data_ros_utils.h>
#include <ros/ros.h>
#include <decomp_util/iterative_decomp.h>
#include <sensor_msgs/point_cloud_conversion.h>
#include <nav_msgs/Path.h>

std_msgs::Header header_;

int main(int argc, char ** argv){
  ros::init(argc, argv, "test");
  ros::NodeHandle nh("~");

  ros::Publisher map_pub = nh.advertise<sensor_msgs::PointCloud2>("cloud", 1, true);
  ros::Publisher marker_pub = nh.advertise<visualization_msgs::MarkerArray>("markers", 1, true);
  ros::Publisher path_pub = nh.advertise<nav_msgs::Path>("path", 1, true);
  ros::Publisher es_pub = nh.advertise<decomp_ros_msgs::Ellipsoids>("ellipsoids", 1, true);
  ros::Publisher poly_pub = nh.advertise<decomp_ros_msgs::Polyhedra>("polyhedra", 1, true);

  header_.frame_id = std::string("map");
  std::string file_name, topic_name, marker_name, path_file;

  nh.param("path_file", path_file, std::string("path.txt"));
  nh.param("bag_file", file_name, std::string("voxel_map"));
  nh.param("bag_topic", topic_name, std::string("voxel_map"));
  nh.param("bag_marker", marker_name, std::string("voxel_map"));
  //Read the point cloud from bag
  sensor_msgs::PointCloud2 map = read_point_cloud2(file_name, topic_name);
  map.header = header_;
  map_pub.publish(map);

  //Convert into vector of Eigen
  sensor_msgs::PointCloud cloud;
  sensor_msgs::convertPointCloud2ToPointCloud(map, cloud);
  vec_Vec3f obs = DecompROS::cloud_to_vec(cloud);

  visualization_msgs::MarkerArray markers = read_marker_array(file_name, marker_name);
  for(auto & it: markers.markers)
    it.header = header_;
  marker_pub.publish(markers);

  //Read path from txt
  vec_Vec3f path;
  if(!read_path(path_file, path))
    ROS_ERROR("Fail to read a path!");

  //Downsample the path as many line segments with lenght equal to 1.0
  path = path_downsample(path, 1.0);

  nav_msgs::Path path_msg = DecompROS::eigen_to_path(path);
  path_msg.header = header_;
  path_pub.publish(path_msg);

  //Using iterative decomposition
  IterativeDecomp decomp_util(true);
  decomp_util.set_obstacles(obs);
  decomp_util.decomp_iter(path, 10, true); //Set max iteration number of 10, do fix the path

  //Publish visualization msgs
  decomp_ros_msgs::Ellipsoids es_msg = DecompROS::ellipsoids_to_ros(decomp_util.get_ellipsoids());
  es_msg.header = header_;
  es_pub.publish(es_msg);

  decomp_ros_msgs::Polyhedra poly_msg = DecompROS::polyhedra_to_ros(decomp_util.get_polyhedra());
  poly_msg.header = header_;
  poly_pub.publish(poly_msg);


  /*
  vec_LinearConstraint3f cs = decomp_util.get_constraints();
  for(int i = 0; i < cs.size(); i++) {
    MatD3f A = cs[i].first;
    VecDf b = cs[i].second;

    printf("i: %d\n", i);
    std::cout << "start: " << (A*path[i]-b).transpose() << std::endl;
    std::cout << "end: " << (A*path[i+1]-b).transpose() << std::endl;
  }
  */


  ros::spin();

  return 0;
}

/**
 * @file env_vision.h
 * @brief environment for planning in image space
 */

#ifndef ENV_VISION_H
#define ENV_VISION_H
#include <planner/env_base.h>
#include <primitive/primitive.h>
#include <collision_checking/vision_queue_util.h>

namespace MPL {

/**
 * @brief Image environment
 */
class env_vision : public env_base
{
  protected:
    std::unique_ptr<VisionQueueUtil> map_util_;
  public:

    ///Simple constructor
    env_vision() {
      map_util_.reset(new VisionQueueUtil());
    }

    ///Get depth point cloud
    vec_Vec3f cloud() {
      return map_util_->cloud();
    }

    ///Get image with projected trajectory
    cv::Mat image(const Trajectory& traj) {
      return map_util_->img(0, traj);
    }

    ///Add new image in buffer
    void add_image(const cv::Mat& img, const Aff3f& TF, const CameraInfo& info) {
      map_util_->addImage(img, TF, info);
    }

    ///Set goal state
    void set_goal(const Waypoint& goal) {
      ps_.clear();
      primitives_.clear();
      goal_node_ = goal;
      goal_outside_ = false;

      if (map_util_->isOutside(goal.pos)) {
        printf(ANSI_COLOR_GREEN "goal out side! " ANSI_COLOR_RESET "\n");
        goal_outside_ = true;
      }
    }

    ///Check if a point is in free space
    bool is_free(const Vec3f& pt) const {
      return map_util_->isFree(pt);
    }

    ///Check if a primitive is in free space
    bool is_free(const Primitive& pr) const {
      return map_util_->isFree(pr);
    }

    /**
     * @brief Get successor
     *
     * When goal is outside, extra step is needed for finding optimal trajectory
     * Here we use Heuristic function and multiply with 2
     */
    void get_succ( const Waypoint& curr, 
        std::vector<Waypoint>& succ,
        std::vector<Key>& succ_idx,
        std::vector<double>& succ_cost,
        std::vector<int>& action_idx ) const
    {
      ps_.push_back(curr.pos);
      succ.clear();
      succ_idx.clear();
      succ_cost.clear();
      action_idx.clear();

      if (map_util_->isOutside(curr.pos)) {
        if(goal_outside_) {
          succ.push_back(goal_node_);
          succ_idx.push_back(state_to_idx(goal_node_));
          succ_cost.push_back(2*get_heur(curr));
          action_idx.push_back(-1);
        }
        return;
      }

      if(!map_util_->isFree(curr.pos))
        return;


      for(int i = 0; i < (int)U_.size(); i++) {
        Primitive p(curr, U_[i], dt_);
        Waypoint tn = p.evaluate(dt_);
        if(p.valid_vel(v_max_)) {
          if(!is_free(p))
            continue;
          tn.use_pos = true;
          tn.use_vel = true;
          tn.use_acc = false;

          succ.push_back(tn);
          succ_idx.push_back(state_to_idx(tn));
          succ_cost.push_back(p.J(wi_) + w_*dt_);
          action_idx.push_back(i);
          //primitives_.push_back(p);
        }

      }

    }


};

}

#endif

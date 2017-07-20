#include <planner/mp_base_util.h>

using namespace MPL;

MPBaseUtil::MPBaseUtil() {
  planner_verbose_ = false;
}

void MPBaseUtil::setEpsilon(decimal_t eps) {
  epsilon_ = eps;
  if(planner_verbose_)
    printf("[MPBaseUtil] set epsilon: %f\n", epsilon_);
}

void MPBaseUtil::setMaxNum(int num) {
  max_num_ = num;
  if(planner_verbose_)
    printf("[MPBaseUtil] set max num: %d\n", max_num_);
}

void MPBaseUtil::setDt(decimal_t dt) {
  ENV_->set_dt(dt);
  if(planner_verbose_)
    printf("[MPBaseUtil] set dt: %f\n", dt);
}

void MPBaseUtil::setW(decimal_t w) {
  ENV_->set_w(w);
  if(planner_verbose_)
    printf("[MPBaseUtil] set w: %f\n", w);
}

void MPBaseUtil::setMode(int n, bool use_3d, const Waypoint& p) {
  ENV_->set_discretization(n, use_3d);
  if(planner_verbose_)
    printf("[MPBaseUtil] set n: %d, use_3d: %d\n", n, use_3d);
  if(p.use_pos && p.use_vel && p.use_acc) {
    ENV_->set_wi(2);
    if(planner_verbose_)
      printf("[MPBaseUtil] set effort in jrk\n");
  }
  else if(p.use_pos && p.use_vel && !p.use_acc) {
    ENV_->set_wi(1);
    if(planner_verbose_)
      printf("[MPBaseUtil] set effort in acc\n");
  }
  else if(p.use_pos && !p.use_vel && !p.use_acc) {
    ENV_->set_wi(0);
    if(planner_verbose_)
      printf("[MPBaseUtil] set effort in vel\n");
  }
}

void MPBaseUtil::setVmax(decimal_t v_max) {
  ENV_->set_v_max(v_max);
  if(planner_verbose_)
    printf("[MPBaseUtil] set v_max: %f\n", v_max);
}

void MPBaseUtil::setAmax(decimal_t a_max) {
  ENV_->set_a_max(a_max);
  if(planner_verbose_)
    printf("[MPBaseUtil] set a_max: %f\n", a_max);
}

void MPBaseUtil::setUmax(decimal_t u_max) {
  ENV_->set_u_max(u_max);
  if(planner_verbose_)
    printf("[MPBaseUtil] set u_max: %f\n", u_max);
}

void MPBaseUtil::setTol(decimal_t tol_dis, decimal_t tol_vel) {
  ENV_->set_tol_dis(tol_dis);
  ENV_->set_tol_vel(tol_vel);
  if(planner_verbose_) {
    printf("[MPBaseUtil] set tol_dis: %f\n", tol_dis);
    printf("[MPBaseUtil] set tol_vel: %f\n", tol_vel);
  }
}

std::vector<Waypoint> MPBaseUtil::getPath() {
  return path_;
}

std::vector<Primitive> MPBaseUtil::getPrimitives() { 
  return ENV_->primitives(); 
}

vec_Vec3f MPBaseUtil::getPs() { 
  return ENV_->ps(); 
}

Trajectory MPBaseUtil::getTraj() {
  std::vector<Primitive> ps;

  for(int i = 0; i < (int)path_.size()-1; i++){
    Waypoint nw1 = path_[i];
    Waypoint nw2 = path_[i+1];

    //Primitive p(nw1, nw2, ENV_->get_dt());
    Primitive p(nw1, nw2, dts_[i]);
    ps.push_back(p);
  }

  return Trajectory(ps);
}

std::vector<decimal_t> MPBaseUtil::getDts() {
  return dts_;
}


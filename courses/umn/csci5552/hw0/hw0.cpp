#include <homework/hw0/hw0.h>

std::vector<Eigen::VectorXd> DrawEllipse(Eigen::Vector2d center,
                                         Eigen::Vector2d major_axis, Eigen::Vector2d minor_axis,
                                         size_t n_pts) {
  std::vector<Eigen::VectorXd> points(n_pts);
  double major_len = major_axis.norm(), minor_len = minor_axis.norm(),
    major_angle_cos = major_axis[0] / major_len,
    major_angle_sin = major_axis[1] / major_len;

  for (size_t i = 0; i < n_pts; ++i) {
    // n_pts-1 so that the first and last points are the same (completing the
    // circle)
    double theta = 6.28318530718 * i / (n_pts - 1);

    // HW0: Modify this to account for the center, major axis, and minor axis
    double x0 = major_len * std::cos(theta), y0 = minor_len * std::sin(theta);
    points[i] = Eigen::Vector2d(x0 * major_angle_cos - y0 * major_angle_sin + center[0],
                                x0 * major_angle_sin + y0 * major_angle_cos + center[1]);
  }

  return points;
}

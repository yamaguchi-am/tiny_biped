struct Command {
  // Each parameter must be in the range of [-1.0, 1.0].
  float forward;
  float turn;
};

enum Phase {
  STAND_STILL,         // start position
  RIGHT_START_OR_END,  // pivot=right, between STAND_STILL and RIGHT_?
  RIGHT_1,             // pivot=right, left leg behind
  RIGHT_2,             // pivot=right, left leg ahead
  LEFT_1,              // pivot=left, right leg behind
  LEFT_2,              // pivot=left, right leg ahead
  LEFT_START_OR_END,   // pivot=left, between STAND_STILL and LEFT_?
};

enum Pivot { P_CENTER, P_LEFT, P_RIGHT };

// Parameters to express poses of the robot during its walk motion.
struct PoseParam {
  Pivot pivot;  // Pivot foot.
  int toes;     // Toe out (+) or toe in (-).
  int twist;    // Twist the body to the left (+) or right (-).
};

class WalkController {
 public:
  WalkController();
  // Advances the time by |elapsed| seconds.
  void Update(float elapsed, const Command& command);
  void GetPose(float* output_angles) const;
  void Init();

 private:
  // Updates the internal state machine to decide next pose.
  void AdvanceState(const Command& command);
  // Sets up next transition to the new pose.
  void StartNextMove(const Command& command);

  // Current state on the state transition diagram.
  Phase phase_;
  bool first_;               // must walk at least 1 step after start.
  Command current_command_;  // memorize command until finishing 1st step

  // Transition between two poses.
  PoseParam current_pose_;
  PoseParam next_pose_;
  float duration_;

  float elapsed_;
};

#include "walk_controller.h"

// ID of servos. Index of kServoPinMap.
const int kIdRightYaw = 0;
const int kIdRightRoll = 1;
const int kIdLeftYaw = 2;
const int kIdLeftRoll = 3;

// The angle of leg yaw axes for walking forward, in microseconds.
const int kTwistAngleMsec = 300;
// The angle of leg yaw axes for turns, in microseconds.
const int kTurnAngleMsec = 150;
const float kTwistDurationSec = 0.4;
const float kRollDurationSec = 0.2;

WalkController::WalkController() { Init(); }

void WalkController::Init() {
  current_pose_.pivot = P_CENTER;
  current_pose_.twist = 0;
  current_pose_.toes = 0;
  next_pose_.pivot = P_CENTER;
  next_pose_.twist = 0;
  next_pose_.toes = 0;
  duration_ = kRollDurationSec;
  phase_ = STAND_STILL;
  first_ = true;
}

void WalkController::AdvanceState(const Command& command) {
  if (command.forward || command.turn) {
    current_command_ = command;
  } else if (command.forward == 0 && command.turn == 0 && first_) {
    command = current_command_;
  }
  if (command.forward == 0 && command.turn == 0) {
    switch (phase_) {
      case STAND_STILL:
      case LEFT_START_OR_END:
      case RIGHT_START_OR_END:
        phase_ = STAND_STILL;
        break;
      case RIGHT_1:
      case RIGHT_2:
        phase_ = RIGHT_START_OR_END;
        break;
      case LEFT_1:
      case LEFT_2:
        phase_ = LEFT_START_OR_END;
        break;
    }
  } else {
    switch (phase_) {
      case STAND_STILL:
        phase_ = RIGHT_START_OR_END;
        first_ = true;
        break;
      case RIGHT_START_OR_END:
        phase_ = RIGHT_2;
        break;
      case RIGHT_1:
        phase_ = RIGHT_2;
        break;
      case RIGHT_2:
        phase_ = LEFT_1;
        first_ = false;
        break;
      case LEFT_1:
        phase_ = LEFT_2;
        break;
      case LEFT_2:
        phase_ = RIGHT_1;
        first_ = false;
        break;
      case LEFT_START_OR_END:
        phase_ = LEFT_2;
        break;
    }
  }
}

void WalkController::StartNextMove(const Command& command) {
  current_pose_ = next_pose_;
  switch (phase_) {
    case STAND_STILL:
      next_pose_.pivot = P_CENTER;
      next_pose_.twist = 0;
      next_pose_.toes = 0;
      break;
    case RIGHT_START_OR_END:
      next_pose_.pivot = P_RIGHT;
      next_pose_.twist = 0;
      next_pose_.toes = 0;
      break;
    case LEFT_START_OR_END:
      next_pose_.pivot = P_LEFT;
      next_pose_.twist = 0;
      next_pose_.toes = 0;
      break;
    case RIGHT_1:
      next_pose_.pivot = P_RIGHT;
      next_pose_.twist = kTwistAngleMsec * command.forward;
      next_pose_.toes = kTurnAngleMsec * command.turn;
      break;
    case RIGHT_2:
      next_pose_.pivot = P_RIGHT;
      next_pose_.twist = -kTwistAngleMsec * command.forward;
      next_pose_.toes = -kTurnAngleMsec * command.turn;
      break;
    case LEFT_1:
      next_pose_.pivot = P_LEFT;
      next_pose_.twist = -kTwistAngleMsec * command.forward;
      next_pose_.toes = -kTurnAngleMsec * command.turn;
      break;
    case LEFT_2:
      next_pose_.pivot = P_LEFT;
      next_pose_.twist = kTwistAngleMsec * command.forward;
      next_pose_.toes = kTurnAngleMsec * command.turn;
      break;
  }
  if (current_pose_.pivot != next_pose_.pivot) {
    duration_ = kRollDurationSec;
  } else {
    duration_ = kTwistDurationSec;
  }
}

void GeneratePose(struct PoseParam param, float* output) {
  // Roll angle of the pivot foot.
  const int kRollPivot = 280;
  const int kRollPivotDeltaForTurn = 310;
  // Roll anale of the free foot. Made larger than above so that
  // the outer edge of the foot will push the ground to tilt the robot.
  const int kRollFree = 700;

  int pivot_roll =
      (param.toes != 0) ? kRollPivotDeltaForTurn : kRollPivotDeltaForTurn;
  if (param.pivot == P_RIGHT) {
    output[kIdRightRoll] = -pivot_roll;
    output[kIdLeftRoll] = -kRollFree;
  } else if (param.pivot == P_LEFT) {
    output[kIdRightRoll] = kRollFree;
    output[kIdLeftRoll] = pivot_roll;
  } else {
    output[kIdRightRoll] = 0;
    output[kIdLeftRoll] = 0;
  }
  // Yaw axes
  // (+): foot turns clockwise
  // (-): foot turns counterclockwise
  output[kIdLeftYaw] = output[kIdRightYaw] = param.twist;
  output[kIdLeftYaw] -= param.toes;
  output[kIdRightYaw] += param.toes;
}

void WalkController::Update(float elapsed, const Command& command) {
  elapsed_ += elapsed;
  if (elapsed_ > duration_) {
    elapsed_ -= duration_;
    AdvanceState(command);
    StartNextMove(command);
  }
}

void WalkController::GetPose(float* output_angles) const {
  float ratio = elapsed_ / duration_;
  float start_angles[4];
  float end_angles[4];
  GeneratePose(current_pose_, start_angles);
  GeneratePose(next_pose_, end_angles);
  for (int i = 0; i < 4; i++) {
    output_angles[i] = (1 - ratio) * start_angles[i] + ratio * end_angles[i];
  }
}

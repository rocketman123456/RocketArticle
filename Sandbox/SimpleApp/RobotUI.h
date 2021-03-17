#pragma once
#include "Event/Event.h"
#include "Logic/StateMachine.h"
#include "Render/DrawBasic/UI.h"
#include "RobotControl.h"

class RobotUI : implements Rocket::UIContext
{
public:
    void Draw() final;
private:
    void Calculation();
    void CalculateRotation(double x, double y, Rocket::EventVarVec& var);
    void CalculateMovement(int32_t stage, Rocket::EventVarVec& var);
    float CalculateProgress(float start, float end, float current);

    void DrawRobotSetting();
    void DrawRobotState();
    
    Rocket::Ref<Rocket::StateNode> node_curr;

    // UI Control Variable
    int32_t rotate_mode = 0;
    int32_t motor_id = 0;

    bool init = false;
    bool walk = false;
    bool rotation = false;

    bool get_motor_data = false;
    bool set_motor_data = false;
    bool reset_motor = false;

    bool showRobotState = true;

    double W = 0;
    double L = 0;
    double h = 0;
    double angle_x_prev = 0;
    double angle_y_prev = 0;
    double angle_x_curr = 0;
    double angle_y_curr = 0;
    double stride = 0;
    double up_height = 0;
    int32_t direction = 0;

    float motor_data_prev[10] = {0};
    uint8_t valve_data_prev[10] = {0};
    float motor_data_curr[10] = {0};
    uint8_t valve_data_curr[10] = {0};
    float motor_data_target[10] = {0};
    uint8_t valve_data_target[10] = {0};
};
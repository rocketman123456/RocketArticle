#pragma once
#include "Event/Event.h"
#include "Logic/StateMachine.h"
#include "Render/DrawBasic/UI.h"

class RobotUI : implements Rocket::UIContext
{
public:
    void Draw() final;
    bool OnResponseEvent(Rocket::EventPtr& e);
private:
    void Calculation();
    void CalculateRotation(double x, double y);
    void CalculateMovement(int32_t stage);
    void CalculateMovementRecover(int32_t stage);
    float CalculateProgress(float start, float end, float current);

    void SetEventData(Rocket::EventVarVec& var);

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

    double W = 800;
    double L = 1000;
    double h = 120;
    double angle_x_prev = 0;
    double angle_y_prev = 0;
    double angle_x_curr = 0;
    double angle_y_curr = 0;
    double stride = 100;
    double up_height = 80;
    int32_t direction = 1;

    const int32_t max_motor_data_store = 100 * 10;
    Rocket::Vec<float> motor_data[10];

    float motor_data_start[10] = {0};
    float motor_data_prev[10] = {0};
    float motor_data_curr[10] = {0};
    float motor_data_target[10] = {0};

    uint8_t valve_data_target[10] = {0};
};
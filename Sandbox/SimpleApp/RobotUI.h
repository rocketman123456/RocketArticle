#pragma once
#include "Render/DrawBasic/UI.h"
#include "RobotControl.h"

class RobotUI : implements Rocket::UIContext
{
public:
    void Draw() final;
private:
    void CalculateRobot();
    
    // UI Control Variable
    ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    double angle_x;
    double angle_y;
    int32_t direction;
    bool showRobotState = true;

    double W = 0;
    double L = 0;
    double h = 0;
    int motor_id = 0;
    float motor_data[10] = {0};
    uint8_t valve_data[10] = {0};
};
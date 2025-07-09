#include "User_Task.h"
#include "Drv_RcIn.h"
#include "LX_FC_Fun.h"
#include "Drv_BSP.h"
#include "Command.h"
#include "LX_FC_State.h"

#define FREE_CONTROL 1

#ifndef BAK

#if !FREE_CONTROL

static u8 mission_step = 0;

#endif

int hasBattery = 0;

void UserTask_OneKeyCmd(void)
{
    //////////////////////////////////////////////////////////////////////
    // 一键起飞/降落例程
    //////////////////////////////////////////////////////////////////////
    // 用静态变量记录一键起飞/降落指令已经执行。
    static u8 has_takeoff = 0;

    // 判断有遥控信号才执行
    if (rc_in.no_signal == 0 && getFX_Mode() == 3 && getUnlockState() == Unlock) {
#if FREE_CONTROL
        if (CommandPacket.state == Doing && IsIdle() == 1) {
                if (Delay2s() != 1) {
                    return;
                }
                CommandPacket.state = Done;
        }
        if (CommandPacket.state == Waiting) {
                switch (CommandPacket.type) {
                    case TakeOff:
                        if (has_takeoff == 0) {
                            has_takeoff = 1;
                            OneKey_Takeoff(CommandPacket.arg2);
                        } else {
                            PackageClear();
                            ErrorCallBack(DoubleTakeOff);
                            return;
                        }
                        break;
                    case Move:
                        if (has_takeoff == 1) {
                            Horizontal_Move(CommandPacket.arg2, 100, CommandPacket.arg1);
                        } else {
                            PackageClear();
                            ErrorCallBack(NotInFlight);
                            return;
                        }
                        break;
                    case Land:
                        if (has_takeoff == 1) {
                            has_takeoff = 0;
                            OneKey_Land();
                            break;
                        } else if (has_takeoff == 0) {
                            PackageClear();
                            ErrorCallBack(PreFlightlanding);
                            return;
                        }
                }
                CommandPacket.state = Doing;
        } else if (CommandPacket.state == Done) {
                CompleteCallBack();
                PackageClear();
        }
#else
        if (has_takeoff == 0) {
            switch (mission_step) {
                case 0:
                    mission_step += LX_Change_Mode(3);
                    break;
                case 1:
                    mission_step += Delay20ms();
                    break;
                case 2:
                    mission_step += FC_Unlock();
                    break;
                case 3:
                    mission_step += Delay20ms();
                    break;
                case 4:
                    OneKey_Takeoff(100);
                    has_takeoff = 1;
                    break;
                default:
                    break;
            }
        }
#endif
    }
    ////////////////////////////////////////////////////////////////////////
}

#endif // BAK
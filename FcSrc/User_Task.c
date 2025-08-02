#include "User_Task.h"
#include "ANO_LX.h"
#include "Drv_RcIn.h"
#include "Drv_Uart.h"
#include "LX_FC_Fun.h"
#include "Drv_BSP.h"
#include "Command.h"
#include "LX_FC_State.h"
#include <stdint.h>

#ifndef BAK
#include "command.h"
#include "debug.h"
#include <stdio.h>

#define FREE_CONTROL 1

#if !FREE_CONTROL

static u8 mission_step = 0;

#endif
static u8 has_takeoff = 0;
static u8 has_init    = 0;
u8 emergency_hover = 0;

void UserTask_OneKeyCmd(void)
{
#ifdef FREE_CONTROL
    //////////////////////////////////////////////////////////////////////
    // 一键起飞/降落例程
    //////////////////////////////////////////////////////////////////////
    // 用静态变量记录一键起飞/降落指令已经执行。
    

    // 判断有遥控信号才执行
    if (rc_in.no_signal == 1) {
        return;
    }
    if (has_takeoff == 1 && has_init == 1 && emergency_hover == 1) {
        Hover();
        emergency_hover = 0;
        return;
    }
    if (has_init == 0) {
        if (getUnlockState() == Unlock && rc_in.rc_ch.st_data.ch_[ch_10_aux6] > 1800) {
            has_init = 1;
            DrvUart3SendBuf((uint8_t *)"I", 1);
        }
    }
    if (getUnlockState() == Unlock && has_init == 1) {
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
#ifdef S_DEBUG
                        char buffer[256];
                        sprintf(buffer, "Take Off Height : %d", CommandPacket.arg2);
                        DebugTransmit(buffer);
#endif
                        OneKey_Takeoff(CommandPacket.arg2);
                    } else {
                        PackageClear();
                        ErrorCallBack(DoubleTakeOff);
                        return;
                    }
                    break;
                case Move:
                    if (has_takeoff == 1) {
#ifdef S_DEBUG
                        char buffer[256];
                        sprintf(buffer, "Move Command: Distance: %d, Direction: %d", CommandPacket.arg2, CommandPacket.arg1);
                        DebugTransmit(buffer);
#endif
                        Horizontal_Move(CommandPacket.arg2, 25, CommandPacket.arg1);
                    } else {
                        PackageClear();
                        ErrorCallBack(NotInFlight);
                        return;
                    }
                    //54 01 00 00 00 6e 45
                    //54 00 00 00 00 64 45
                    //54 01 00 00 00 c8 45
                    break;
                case Land:
                    if (has_takeoff == 1) {
                        has_takeoff = 0;
#ifdef S_DEBUG
                        DebugTransmit("Landing Command Received");
#endif
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
    } else if (getUnlockState() == Lock && rc_in.rc_ch.st_data.ch_[ch_10_aux6] < 1800 && has_init == 1) {
        has_takeoff = 0;
        has_init    = 0;
        PackageClear();
    }
#endif
}

////////////////////////////////////////////////////////////////////////

#endif // BAK
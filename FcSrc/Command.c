#include "command.h"
#include "Drv_Uart.h"
#include <stdint.h>
#include "Drv_BSP.h"
#include "LX_FC_State.h"

extern u8 U3RxDataTmp[100];
extern u8 U3RxInCnt;
extern u8 U3Cnt;

_CommandPacket CommandPacket = {
    .arg1  = 0,
    .arg2  = 0,
    .state = Empty,
    .type  = TakeOff};

static int delay2s_counter = 0;
int Delay2s(void)
{
    if (delay2s_counter < 100) {
        delay2s_counter++;
        return 0; // Not yet 10 seconds
    } else {
        delay2s_counter = 0;
        return 1; // 10 seconds elapsed
    }
}

static int delay20ms_counter = 0;
int Delay20ms(void) {
    if (delay20ms_counter < 1) {
        delay20ms_counter++;
        return 0; // Not yet 1 second
    } else {
        delay20ms_counter = 0;
        return 1; // 1 second elapsed
    }
}

int IsIdle(void) {
    return ((fc_sta.cmd_fun.CID == 0x10) && (fc_sta.cmd_fun.CMD_0 == 0x00) && (fc_sta.cmd_fun.CMD_1 == 0x04));
}


void PackageClear(void)
{
    CommandPacket.arg1  = 0;
    CommandPacket.arg2  = 0;
    CommandPacket.state = Empty;
    CommandPacket.type  = TakeOff;
}

static void successReceiveCallBack(void)
{
    DrvUart3SendBuf((uint8_t *)"S", 1);
}

void PackageGet(void)
{

    if (CommandPacket.state != Empty) {
        DrvUart3SendBuf((uint8_t *)"B", 1);
        return;
    }
    for (u8 i = 0; i < U3Cnt; i++) {
        if (U3RxDataTmp[i] == 0x54) {
            if (i + 5 < U3Cnt && U3RxDataTmp[i + 5] == 0x45) {
                if (CommandPacket.state == Doing || CommandPacket.state == Waiting) {
                    DrvUart3SendBuf((uint8_t *)"B", 1);
                    return; // Already have a command packet
                }
                CommandPacket.type  = (CommandType)U3RxDataTmp[i + 1];
                CommandPacket.arg1  = (U3RxDataTmp[i + 2] << 8) | U3RxDataTmp[i + 3];
                CommandPacket.arg2  = U3RxDataTmp[i + 4];
                CommandPacket.state = Waiting;
                U3Cnt = 0;
                successReceiveCallBack();
            }
        }
    }
}

void ErrorCallBack(CommandError error)
{
    switch (error) {
        case DoubleTakeOff:
            DrvUart3SendBuf((uint8_t *)"T", 1);
            break;
        case DoubleLand:
            DrvUart3SendBuf((uint8_t *)"L", 1);
            break;
        case PreFlightlanding:
            DrvUart3SendBuf((uint8_t *)"D", 1);
            break;
        case NotInFlight:
            DrvUart3SendBuf((uint8_t *)"N", 1);
            break;
        case Other:
            DrvUart3SendBuf((uint8_t *)"O", 1);
            break;
        default:
            break;
    }
}

void CompleteCallBack(void)
{
    DrvUart3SendBuf((uint8_t *)"C", 1);
}
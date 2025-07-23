
#include "Drv_BSP.h"
#include "Command.h"
#include "string.h"


void DrvUart3SendBuf(unsigned char *DataToSend, u8 data_num);
void DebugTransmit(const char *str)
{
#ifdef S_DEBUG
    // 发送调试信息
    DrvUart3SendBuf((uint8_t *)str, strlen(str));
#endif
}
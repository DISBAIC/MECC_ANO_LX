

#ifndef _COMMAND_H_
#define _COMMAND_H_

typedef unsigned char u8;
typedef unsigned short u16;

typedef enum {
    TakeOff = 0,
    Move    = 1,
    Land    = 2
} CommandType;

typedef enum {
    Waiting = 1,
    Doing   = 2,
    Done    = 3,
    Empty  = 0
} TaskState;

typedef enum {
    Forward = 0,
    Right = 90,
    Backward = 180,
    Left = 270
} Direction;

typedef enum {
    DoubleTakeOff = 0,
    DoubleLand = 1,
    PreFlightlanding = 2,
    NotInFlight = 3,
    Other = 4
} CommandError ;

// pack type
// [0x54 CommandType Arg1[0:15] Arg2[0:7] 0x45] size == 6

typedef struct {
    CommandType type;
    u16 arg1;
    u8 arg2;
    TaskState state; // 0: not completed, 1: completed
} _CommandPacket;

void Uart3_IRIQ(void);

int IsIdle(void);

void PackageGet(void);

void PackageClear(void);

void CompleteCallBack(void);

void ErrorCallBack(CommandError error);

int Delay2s(void);

int Delay20ms(void);

extern _CommandPacket CommandPacket;

#endif
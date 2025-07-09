#include "User_Task.h"
#include "Drv_RcIn.h"
#include "LX_FC_Fun.h"
#include "Drv_BSP.h"
#include "LX_FC_State.h"

#ifdef BAK

void UserTask_OneKeyCmd(void)
{
    //////////////////////////////////////////////////////////////////////
    //一键起飞/降落例程
    //////////////////////////////////////////////////////////////////////
    //用静态变量记录一键起飞/降落指令已经执行。
    static u8 one_key_takeoff_f = 1, one_key_land_f = 1, one_key_mission_f = 0;
    static u8 mission_step;
    //判断有遥控信号才执行
    if (rc_in.no_signal == 0 && getUnlockState() == Unlock)
    {
        //判断第6通道拨杆位置 1300<CH_6<1700
        if (rc_in.rc_ch.st_data.ch_[ch_6_aux2] > 1300 && rc_in.rc_ch.st_data.ch_[ch_6_aux2] < 1700  && getFX_Mode() == 3)
        {
            //还没有执行
            if (one_key_takeoff_f == 0)
            {
                //标记已经执行
                one_key_takeoff_f =
                    //执行一键起飞
                    OneKey_Takeoff(100); //参数单位：厘米； 0：默认上位机设置的高度。
            }
        }
        else
        {
            //复位标记，以便再次执行
            one_key_takeoff_f = 0;
        }
        //
        //判断第6通道拨杆位置 800<CH_6<1200
        if (rc_in.rc_ch.st_data.ch_[ch_6_aux2] > 800 && rc_in.rc_ch.st_data.ch_[ch_6_aux2] < 1200  && getFX_Mode() == 3)
        {
            //还没有执行
            if (one_key_land_f == 0)
            {
                //标记已经执行
                one_key_land_f =
                    //执行一键降落
                    OneKey_Land();
            }
        }
        else
        {
            //复位标记，以便再次执行
            one_key_land_f = 0;
        }
	//判断第6通道拨杆位置 1700<CH_6<2200 
		if(rc_in.rc_ch.st_data.ch_[ch_6_aux2]>1700 && rc_in.rc_ch.st_data.ch_[ch_6_aux2]<2200 && getFX_Mode() == 3)
		{
			//还没有执行
			if(one_key_mission_f ==0)
			{
				//标记已经执行
				one_key_mission_f = 1;
				//开始流程
				mission_step = 1;
			}
		}
		else
		{
			//复位标记，以便再次执行
			one_key_mission_f = 0;		
		}
		//
		if(one_key_mission_f==1)
		{
			static u16 time_dly_cnt_ms = 0;
			//
			switch(mission_step)
			{
				case 0:
				{
					//切换程控模式
					mission_step += 1;
				}
				break;
				case 1:
				{
					//解锁
					mission_step += 1;
				}
				break;
				case 2:
				{
					//等2秒
					if(time_dly_cnt_ms < 2000)
					{
						time_dly_cnt_ms += 20;//ms
					}
					else
					{
						time_dly_cnt_ms = 0;
						mission_step += 1;
					}
				}
				break;
				case 3:
				{
					//起飞
					mission_step += OneKey_Takeoff(100);//参数单位：厘米； 0：默认上位机设置的高度。
				}
				break;
				case 4:
				{
					//等20秒
					if(time_dly_cnt_ms < 20000)
					{
						time_dly_cnt_ms+=20;//ms
					}
					else
					{
						time_dly_cnt_ms = 0;
						mission_step += 1;
					}
				}
				break;
				case 5:
				{
					//起飞
					OneKey_Land();
				}
				break;
				default:
				break;
			}
		}
		else
		{
			mission_step = 0;
		}
	}
    ////////////////////////////////////////////////////////////////////////
}
#endif
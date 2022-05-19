#include "key_core.h"

#include "app_log.h"
#include <string.h>

#if defined(PKG_USING_KEY)||defined(RT_DEBUG_KEY)
/*******************************************************************
 *                          Variable Declaration
 *******************************************************************/
static struct key* head_key = NULL;

keySTATE all_keys_state = {0};
bool Multiple_keys_trigger_flag = false;
/*******************************************************************
 *                         Function Declaration
 *******************************************************************/
static char *StrnCopy(char *dst, const char *src, uint32_t n);
static void add_key(key_t* btn);
static void key_cycle_process(key_t *btn);

/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//创建按键
void key_create(
    const char *name,key_t *btn,
    uint8_t(*read_btn_level)(void),
    uint8_t btn_trigger_level)
{
    if( btn == NULL)
		return ;
        //APP_LOG_INFO(RT_DEBUG_THREAD,("struct key is NULL!"));
	
	//数据清零
    memset(btn, 0, sizeof(struct key));		//Clear structure information

    StrnCopy(btn->name, name, KEY_NAME_MAX);		//key name

    btn->key_state         = NONE_TRIGGER;			//key status
    btn->key_last_state    = NONE_TRIGGER;			//key last status
    btn->key_trigger_event = NONE_TRIGGER;			//key trigger event
    btn->read_key_level    = read_btn_level;		//key trigger level reading function
    btn->key_trigger_level = btn_trigger_level;		//key trigger level
    btn->key_last_level    = btn->read_key_level();	//key current level
    btn->debounce_time	   = 0;

    //APP_LOG_INFO(RT_DEBUG_THREAD,("key create success!"));
    add_key(btn);	//Added to the singly linked list when key created

#if defined(RT_USING_KEY_PRINT)
    //print_key_info(btn);
#endif
}

//创建按键
void key_create2(
    const char *name,key_t *btn,
    uint8_t(*read_btn_level)(void),
    uint8_t btn_trigger_level, 
	uint8_t Key_index_number)
{
    if( btn == NULL)
		return ;
        //APP_LOG_INFO(RT_DEBUG_THREAD,("struct key is NULL!"));
	
	//数据清零
    memset(btn, 0, sizeof(struct key));		//Clear structure information

    StrnCopy(btn->name, name, KEY_NAME_MAX);		//key name
	btn->Key_index_number = Key_index_number;
	
    btn->key_state         = NONE_TRIGGER;			//key status
    btn->key_last_state    = NONE_TRIGGER;			//key last status
    btn->key_trigger_event = NONE_TRIGGER;			//key trigger event
    btn->read_key_level    = read_btn_level;		//key trigger level reading function
    btn->key_trigger_level = btn_trigger_level;		//key trigger level
    btn->key_last_level    = btn->read_key_level();	//key current level
    btn->debounce_time	   = 0;

    //APP_LOG_INFO(RT_DEBUG_THREAD,("key create success!"));
    add_key(btn);	//Added to the singly linked list when key created

#if defined(RT_USING_KEY_PRINT)
    //print_key_info(btn);
#endif
}


//把按键事件与回调函数链接起来
void key_attach(key_t *btn,Key_Event btn_event,key_callback btn_callback)
{
    if( btn == NULL)
		return ;
        //APP_LOG_INFO(RT_DEBUG_THREAD,("struct key is NULL!"));

    if(KEY_ALL_TRIGGER == btn_event)//多个按键同时触发
    {
        for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
            btn->callback_function[i] = btn_callback; //A callback function triggered by a key event ,Used to handle key events
    }
    else//一般情况
        btn->callback_function[btn_event] = btn_callback;
}



//将按键从链表中删除
void key_delete(key_t *btn)
{
    struct key** curr;
    for(curr = &head_key; *curr;)//遍历链表
    {
        struct key* entry = *curr;
        if (entry == btn)
            *curr = entry->next;
        else
            curr = &entry->next;
    }
}
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//按键处理过程
void key_process(void)
{
    struct key* pass_key;
    for(pass_key = head_key; pass_key != NULL; pass_key = pass_key->next)
    {
        key_cycle_process(pass_key);
    }
}

/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
uint8_t get_key_state(key_t *btn)
{
    return (uint8_t)(btn->key_state);
}


uint8_t get_key_event(key_t *btn)
{
	return (uint8_t)(btn->key_trigger_event);
}

uint8_t check_pressed_key_num(uint8_t data)
{
	uint8_t count = 0;
	while(data)
	{
		data = data & (data - 1);
		count++;
	}
	return count;
}

Key_Event What_is_the_key_event(uint8_t data)
{
    Key_Event E_num = KEY_ALL_TRIGGER;
    switch(data)
    {
		case UP_DOWN:{
			E_num = KEYS_UP_DOWN;
		}break;
		
		case LEFT_RIGHT:{
            E_num = KEYS_LEFT_RIGHT;
		}break;
			
		case OK_UP:{
            E_num = KEYS_OK_UP;
		}break;
			
		case OK_DOWN:{
            E_num = KEYS_OK_DOWN;
		}break;
			
		case OK_LEFT:{
            E_num = KEYS_OK_LEFT;
		}break;
			
		case OK_RIGHT:{
            E_num = KEYS_OK_RIGHT;
		}break;
        
		case LEFT_UP:{
            E_num = KEYS_LEFT_UP;
		}break;
			
		case LEFT_DOWN:{
            E_num = KEYS_LEFT_DOWN;
		}break;
			
		case RIGHT_UP:{
            E_num = KEYS_RIGHT_UP;
		}break;
			
		case RIGHT_DOWN:{
            E_num = KEYS_RIGHT_DOWN;
		}break;

		default:{
		}break;
    }
    
    return E_num;
}
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
#if defined(RT_USING_KEY_PRINT)
void key_process_callback(void *btn)
{
	uint8_t btn_event = get_key_event(btn);

	switch(btn_event)
	{
		case KEY_DOWM:
		{
			APP_LOG_INFO("Add processing logic for your press trigger");
			break;
		}

		case KEY_UP:
		{
			APP_LOG_INFO("Add processing logic for your trigger release");
			break;
		}

		case KEY_DOUBLE:
		{
			APP_LOG_INFO("Add processing logic for your double-click trigger");
			break;
		}

		case KEY_LONG:
		{
			APP_LOG_INFO("Add processing logic for your long press trigger");
			break;
		}

		case KEY_LONG_FREE:
		{
			APP_LOG_INFO("Add processing logic for your long press trigger free");
			break;
		}
	}
}


void search_key(void)
{
	struct key* pass_key;

	for(pass_key = head_key; pass_key != NULL; pass_key = pass_key->next)
	{
		APP_LOG_INFO("have_key:%s\r\n", pass_key->name);
	}
	
	APP_LOG_INFO("ok\r\n");
}


void get_key_eventlnfo(key_t *btn)
{
	for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
	{
		if(btn->callback_function[i] != 0)
		{
			APP_LOG_INFO("Key_Event:%d",i);
		}
	}
}


void print_key_info(key_t* btn)
{
	APP_LOG_INFO(	"key struct information:\n\
					btn->Name:%s \n\
					btn->key_State:%d \n\
					btn->key_trigger_Event:%d \n\
					btn->key_trigger_Level:%d \n\
					btn->key_Last_Level:%d \n\
					",
					btn->name,
					btn->key_state,
					btn->key_trigger_event,
					btn->key_trigger_level,
					btn->key_last_level		);

	search_key();
}
#endif
/**************************** The following is the internal call function ********************/
static char *StrnCopy(char *dst, const char *src, uint32_t n)
{
    if (n != 0)
    {
        char *d = dst;
        const char *s = src;
        do
        {
            if ((*d++ = *s++) == 0)
            {
                while (--n != 0)
                    *d++ = 0;
                break;
            }
        } while (--n != 0);
    }
    return (dst);
}


static void add_key(key_t* btn)
{
//    struct key *pass_key = head_key;

//    while(pass_key)//用于检查当前的链表
//    {
//        pass_key = pass_key->next;
//    }

    btn->next	= head_key;
    head_key	= btn;
}


static void key_cycle_process(key_t *btn)
{
    uint8_t current_level = (uint8_t)btn->read_key_level();  

	/* 经过按键消抖之后，检测该按键的电平是否发生改变，区分出按键状态：按下，松开，双击 */
    if( (current_level != btn->key_last_level) && (++(btn->debounce_time) >= KEY_DEBOUNCE_TIME) )   
    {
        btn->key_last_level    = current_level;//如果该按键的当前状态与上一次状态不一致，则保存当前电平
        btn->debounce_time     = 0;//清除该按键消抖时间计数器

        if( (btn->key_state == NONE_TRIGGER) || (btn->key_state == KEY_DOUBLE) )
        {
            btn->key_state = KEY_DOWM;//在确定该按键已经被触发后，如果上一次状态为无触发或者双击，则此次状态更新为按下
        }
        else if(btn->key_state == KEY_DOWM)//在确定该按键已经被触发后，如果上一次状态为按下，则此次状态更新为松开
        {
            btn->key_state = KEY_UP;
            //APP_LOG_INFO(RT_DEBUG_THREAD,("key release"));
        }
		
		/* ----------------------------------------------- */
		//可以放在if之外，但是会增加访问次数，消耗资源
		/* ----------------------------------------------- */
		if(!all_keys_state.state)
		{
			Multiple_keys_trigger_flag = false;
		}
		/* ----------------------------------------------- */
    }

    switch(btn->key_state)//btn->key_state:按键待检测状态
    {
		//如果按键状态更新为按下
		case KEY_DOWM:
		{
			//如果按键当前的电平与触发电平(固定的)一致，则证明该按键被触发
			if(btn->key_last_level == btn->key_trigger_level)
			{
				btn->key_trigger_event = KEY_DOWM;//按键事件更新为被按下
				/* ----------------------------------------------- */
				setbit(all_keys_state.state, btn->Key_index_number);
				
				if(Multiple_keys_trigger_flag)
				{
					break;//不再处理长按事件，只触发一次组合按键事件
				}
				/* ----------------------------------------------- */
				//如果按键被按下的时间大于或等于长按事件的阈值，则为长按
				if(++(btn->long_time) >= KEY_LONG_TIME)
				{
				#if defined( LONG_FREE_TRIGGER )
					btn->key_trigger_event = KEY_LONG;
				#else
					if(++(btn->key_cycle) >= KEY_LONG_CYCLE)//Continuous triggering of long press cycles
					{
						btn->key_cycle = 0;
						btn->key_trigger_event = KEY_LONG;
						TRIGGER_CB(KEY_LONG);    //long triggering
					}
				#endif
                    
                    
                    //需要单按键长按事件[定时连续触发]的时，改动此处即可,如果两种情况都用到，则将此处改成函数
                #if 0
					//当长按的时间超过计数(u8),计数溢出处理
					if(btn->long_time == 0xFF)    //Update time overflow
					{
						btn->long_time = KEY_LONG_TIME;//50
					}
					//APP_LOG_INFO("Long press"));
                #else
					//当长按的时间超过计数(u8),计数溢出处理
					if(btn->long_time > COUNT_OVERFLOW)//Update time overflow
					{
						btn->long_time = COUNT_OVERFLOW;
					}
                #endif
                    
                    /*  组合按键检测：如果检测到该按键的长按时间小于长按的
                        定时按下事件的时间，那么就检测是不是组合按键 */
                    if( check_pressed_key_num(all_keys_state.state) > 1 )//检测被按下的按键个数
                    {
                        Multiple_keys_trigger_flag = true;
                        
                        btn->long_time = 0;
                        btn->key_cycle = 0;//单个按键的长按清除
                        
                        //执行组合按键的触发事件
                        btn->key_trigger_event = What_is_the_key_event(all_keys_state.state);
                        TRIGGER_CB(btn->key_trigger_event);
                    }
                    
				#if KEY_LONG_DIFFERENT_TIME_FUN
					//长按到达时间要松开才触发 或 长按到达时间就[密集连续触发]
					if( (btn->long_time >= KEY_LONG_TIME_2S) && (btn->key_trigger_event == KEY_LONG) )//100*20=2s
					{
						if(btn->long_time >= KEY_LONG_TIME_3S)//150*20=3s
						{
							btn->key_trigger_event = KEY_LONG_3s;
							
							//假设长按时可连续触发KEY_LONG_3s
							//TRIGGER_CB(KEY_LONG_3s);
						}
						else
						{
							btn->key_trigger_event = KEY_LONG_2s;
							
							//假设长按时可连续触发KEY_LONG_2s
							//TRIGGER_CB(KEY_LONG_2s);
						}
					}
				#else
					//长按到达时间只触发一次
					if( (btn->long_time >= KEY_LONG_TIME_2S) && (btn->key_trigger_event == KEY_LONG) )//100*20=2s
					{
						switch(btn->long_time)
						{
							case KEY_LONG_TIME_2S:{

								btn->key_trigger_event = KEY_LONG_2s;								
								TRIGGER_CB(KEY_LONG_2s);
							}break;
							
							case KEY_LONG_TIME_3S:{

								btn->key_trigger_event = KEY_LONG_3s;								
								TRIGGER_CB(KEY_LONG_3s);
							}break;

							default:
								break;
						}
					}
				#endif
				}
			}

			break;
		}
		
		//如果按键状态更新为松开
		case KEY_UP:
		{
			/* ----------------------------------------------- */
			clrbit(all_keys_state.state, btn->Key_index_number);
			
			if(Multiple_keys_trigger_flag)
			{
				//清除按键状态
				btn->key_state = NONE_TRIGGER;
				btn->key_last_state = NONE_TRIGGER;
				
				btn->long_time = 0;	//Detection long press failed, clear 0
				btn->timer_count = 0;//当前按键时间为单击，为检测双击做准备
					
				break;
			}
			/* ----------------------------------------------- */
			//如果按键的之前的触发事件是被按下，判断是双击，松开
			if(btn->key_trigger_event == KEY_DOWM) //Trigger click
			{
				//如果距离上一次按下的时间不大于双击的时间阈值，并且按键的上一次状态为双击
				if((btn->timer_count <= KEY_DOUBLE_TIME)&&(btn->key_last_state == KEY_DOUBLE)) // double click
				{
					btn->key_trigger_event = KEY_DOUBLE;//那么就更新按键的触发事件为双击
					TRIGGER_CB(KEY_DOUBLE);
					//APP_LOG_INFO(RT_DEBUG_THREAD,("double click"));
					
					//清除按键状态
					btn->key_state = NONE_TRIGGER;
					btn->key_last_state = NONE_TRIGGER;
				}
				else//其他情况下，按键松开，则清除相关计数器
				{
					btn->long_time = 0;	//Detection long press failed, clear 0
					btn->timer_count = 0;//当前按键时间为单击，为检测双击做准备
					
				#ifndef SINGLE_AND_DOUBLE_TRIGGER
					TRIGGER_CB(KEY_DOWM);              //click
				#endif
					//按键检测状态更新为检测双击
					btn->key_state = KEY_DOUBLE;
					btn->key_last_state = KEY_DOUBLE;
				}
			}
		#if KEY_LONG_DIFFERENT_TIME_FUN
			//如果按键的之前的触发事件是长按2s，检测到松开
			else if(btn->key_trigger_event == KEY_LONG_2s)
			{
				TRIGGER_CB(KEY_LONG_2s);	//Long press free

				//清除长按时间计数器，及按键检测状态记录
				btn->long_time = 0;
				btn->key_state = NONE_TRIGGER;
				//更新按键上一次的检测记录为长按
				btn->key_last_state = KEY_LONG;
			}
			//如果按键的之前的触发事件是长按3s，检测到松开
			else if(btn->key_trigger_event == KEY_LONG_3s)
			{
				TRIGGER_CB(KEY_LONG_3s);	//Long press free

				//清除长按时间计数器，及按键检测状态记录
				btn->long_time = 0;
				btn->key_state = NONE_TRIGGER;
				//更新按键上一次的检测记录为长按
				btn->key_last_state = KEY_LONG;
			}
		#endif
			//如果按键的之前的触发事件是长按，检测到松开
			else if(btn->key_trigger_event == KEY_LONG)
			{
			#if defined( LONG_FREE_TRIGGER )
				TRIGGER_CB(KEY_LONG);	//Long press
			#else
				TRIGGER_CB(KEY_LONG_FREE);	//Long press free
			#endif
				
				//清除长按时间计数器，及按键检测状态记录
				btn->long_time = 0;
				btn->key_state = NONE_TRIGGER;
				//更新按键上一次的检测记录为长按
				btn->key_last_state = KEY_LONG;
			}
			break;
		}

		case KEY_DOUBLE:
		{
			btn->timer_count++;		//Update time
			if(btn->timer_count >= KEY_DOUBLE_TIME)//不是双击
			{
				btn->key_state = NONE_TRIGGER;
				btn->key_last_state = NONE_TRIGGER;
			}
			
		#if defined(SINGLE_AND_DOUBLE_TRIGGER)
			if( (btn->timer_count>=KEY_DOUBLE_TIME) && (btn->key_last_state != KEY_DOWM) )
			{
				btn->timer_count = 0;
				TRIGGER_CB(KEY_DOWM);
				
				btn->key_state = NONE_TRIGGER;
				btn->key_last_state = KEY_DOWM;
			}
		#endif

			break;
		}

		default :
		break;
    }
}

#endif

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
//��������
void key_create(
    const char *name,key_t *btn,
    uint8_t(*read_btn_level)(void),
    uint8_t btn_trigger_level)
{
    if( btn == NULL)
		return ;
        //APP_LOG_INFO(RT_DEBUG_THREAD,("struct key is NULL!"));
	
	//��������
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

//��������
void key_create2(
    const char *name,key_t *btn,
    uint8_t(*read_btn_level)(void),
    uint8_t btn_trigger_level, 
	uint8_t Key_index_number)
{
    if( btn == NULL)
		return ;
        //APP_LOG_INFO(RT_DEBUG_THREAD,("struct key is NULL!"));
	
	//��������
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


//�Ѱ����¼���ص�������������
void key_attach(key_t *btn,Key_Event btn_event,key_callback btn_callback)
{
    if( btn == NULL)
		return ;
        //APP_LOG_INFO(RT_DEBUG_THREAD,("struct key is NULL!"));

    if(KEY_ALL_TRIGGER == btn_event)//�������ͬʱ����
    {
        for(uint8_t i = 0 ; i < number_of_event-1 ; i++)
            btn->callback_function[i] = btn_callback; //A callback function triggered by a key event ,Used to handle key events
    }
    else//һ�����
        btn->callback_function[btn_event] = btn_callback;
}



//��������������ɾ��
void key_delete(key_t *btn)
{
    struct key** curr;
    for(curr = &head_key; *curr;)//��������
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
//�����������
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

//    while(pass_key)//���ڼ�鵱ǰ������
//    {
//        pass_key = pass_key->next;
//    }

    btn->next	= head_key;
    head_key	= btn;
}


static void key_cycle_process(key_t *btn)
{
    uint8_t current_level = (uint8_t)btn->read_key_level();  

	/* ������������֮�󣬼��ð����ĵ�ƽ�Ƿ����ı䣬���ֳ�����״̬�����£��ɿ���˫�� */
    if( (current_level != btn->key_last_level) && (++(btn->debounce_time) >= KEY_DEBOUNCE_TIME) )   
    {
        btn->key_last_level    = current_level;//����ð����ĵ�ǰ״̬����һ��״̬��һ�£��򱣴浱ǰ��ƽ
        btn->debounce_time     = 0;//����ð�������ʱ�������

        if( (btn->key_state == NONE_TRIGGER) || (btn->key_state == KEY_DOUBLE) )
        {
            btn->key_state = KEY_DOWM;//��ȷ���ð����Ѿ��������������һ��״̬Ϊ�޴�������˫������˴�״̬����Ϊ����
        }
        else if(btn->key_state == KEY_DOWM)//��ȷ���ð����Ѿ��������������һ��״̬Ϊ���£���˴�״̬����Ϊ�ɿ�
        {
            btn->key_state = KEY_UP;
            //APP_LOG_INFO(RT_DEBUG_THREAD,("key release"));
        }
		
		/* ----------------------------------------------- */
		//���Է���if֮�⣬���ǻ����ӷ��ʴ�����������Դ
		/* ----------------------------------------------- */
		if(!all_keys_state.state)
		{
			Multiple_keys_trigger_flag = false;
		}
		/* ----------------------------------------------- */
    }

    switch(btn->key_state)//btn->key_state:���������״̬
    {
		//�������״̬����Ϊ����
		case KEY_DOWM:
		{
			//���������ǰ�ĵ�ƽ�봥����ƽ(�̶���)һ�£���֤���ð���������
			if(btn->key_last_level == btn->key_trigger_level)
			{
				btn->key_trigger_event = KEY_DOWM;//�����¼�����Ϊ������
				/* ----------------------------------------------- */
				setbit(all_keys_state.state, btn->Key_index_number);
				
				if(Multiple_keys_trigger_flag)
				{
					break;//���ٴ������¼���ֻ����һ����ϰ����¼�
				}
				/* ----------------------------------------------- */
				//������������µ�ʱ����ڻ���ڳ����¼�����ֵ����Ϊ����
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
                    
                    
                    //��Ҫ�����������¼�[��ʱ��������]��ʱ���Ķ��˴�����,�������������õ����򽫴˴��ĳɺ���
                #if 0
					//��������ʱ�䳬������(u8),�����������
					if(btn->long_time == 0xFF)    //Update time overflow
					{
						btn->long_time = KEY_LONG_TIME;//50
					}
					//APP_LOG_INFO("Long press"));
                #else
					//��������ʱ�䳬������(u8),�����������
					if(btn->long_time > COUNT_OVERFLOW)//Update time overflow
					{
						btn->long_time = COUNT_OVERFLOW;
					}
                #endif
                    
                    /*  ��ϰ�����⣺�����⵽�ð����ĳ���ʱ��С�ڳ�����
                        ��ʱ�����¼���ʱ�䣬��ô�ͼ���ǲ�����ϰ��� */
                    if( check_pressed_key_num(all_keys_state.state) > 1 )//��ⱻ���µİ�������
                    {
                        Multiple_keys_trigger_flag = true;
                        
                        btn->long_time = 0;
                        btn->key_cycle = 0;//���������ĳ������
                        
                        //ִ����ϰ����Ĵ����¼�
                        btn->key_trigger_event = What_is_the_key_event(all_keys_state.state);
                        TRIGGER_CB(btn->key_trigger_event);
                    }
                    
				#if KEY_LONG_DIFFERENT_TIME_FUN
					//��������ʱ��Ҫ�ɿ��Ŵ��� �� ��������ʱ���[�ܼ���������]
					if( (btn->long_time >= KEY_LONG_TIME_2S) && (btn->key_trigger_event == KEY_LONG) )//100*20=2s
					{
						if(btn->long_time >= KEY_LONG_TIME_3S)//150*20=3s
						{
							btn->key_trigger_event = KEY_LONG_3s;
							
							//���賤��ʱ����������KEY_LONG_3s
							//TRIGGER_CB(KEY_LONG_3s);
						}
						else
						{
							btn->key_trigger_event = KEY_LONG_2s;
							
							//���賤��ʱ����������KEY_LONG_2s
							//TRIGGER_CB(KEY_LONG_2s);
						}
					}
				#else
					//��������ʱ��ֻ����һ��
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
		
		//�������״̬����Ϊ�ɿ�
		case KEY_UP:
		{
			/* ----------------------------------------------- */
			clrbit(all_keys_state.state, btn->Key_index_number);
			
			if(Multiple_keys_trigger_flag)
			{
				//�������״̬
				btn->key_state = NONE_TRIGGER;
				btn->key_last_state = NONE_TRIGGER;
				
				btn->long_time = 0;	//Detection long press failed, clear 0
				btn->timer_count = 0;//��ǰ����ʱ��Ϊ������Ϊ���˫����׼��
					
				break;
			}
			/* ----------------------------------------------- */
			//���������֮ǰ�Ĵ����¼��Ǳ����£��ж���˫�����ɿ�
			if(btn->key_trigger_event == KEY_DOWM) //Trigger click
			{
				//���������һ�ΰ��µ�ʱ�䲻����˫����ʱ����ֵ�����Ұ�������һ��״̬Ϊ˫��
				if((btn->timer_count <= KEY_DOUBLE_TIME)&&(btn->key_last_state == KEY_DOUBLE)) // double click
				{
					btn->key_trigger_event = KEY_DOUBLE;//��ô�͸��°����Ĵ����¼�Ϊ˫��
					TRIGGER_CB(KEY_DOUBLE);
					//APP_LOG_INFO(RT_DEBUG_THREAD,("double click"));
					
					//�������״̬
					btn->key_state = NONE_TRIGGER;
					btn->key_last_state = NONE_TRIGGER;
				}
				else//��������£������ɿ����������ؼ�����
				{
					btn->long_time = 0;	//Detection long press failed, clear 0
					btn->timer_count = 0;//��ǰ����ʱ��Ϊ������Ϊ���˫����׼��
					
				#ifndef SINGLE_AND_DOUBLE_TRIGGER
					TRIGGER_CB(KEY_DOWM);              //click
				#endif
					//�������״̬����Ϊ���˫��
					btn->key_state = KEY_DOUBLE;
					btn->key_last_state = KEY_DOUBLE;
				}
			}
		#if KEY_LONG_DIFFERENT_TIME_FUN
			//���������֮ǰ�Ĵ����¼��ǳ���2s����⵽�ɿ�
			else if(btn->key_trigger_event == KEY_LONG_2s)
			{
				TRIGGER_CB(KEY_LONG_2s);	//Long press free

				//�������ʱ������������������״̬��¼
				btn->long_time = 0;
				btn->key_state = NONE_TRIGGER;
				//���°�����һ�εļ���¼Ϊ����
				btn->key_last_state = KEY_LONG;
			}
			//���������֮ǰ�Ĵ����¼��ǳ���3s����⵽�ɿ�
			else if(btn->key_trigger_event == KEY_LONG_3s)
			{
				TRIGGER_CB(KEY_LONG_3s);	//Long press free

				//�������ʱ������������������״̬��¼
				btn->long_time = 0;
				btn->key_state = NONE_TRIGGER;
				//���°�����һ�εļ���¼Ϊ����
				btn->key_last_state = KEY_LONG;
			}
		#endif
			//���������֮ǰ�Ĵ����¼��ǳ�������⵽�ɿ�
			else if(btn->key_trigger_event == KEY_LONG)
			{
			#if defined( LONG_FREE_TRIGGER )
				TRIGGER_CB(KEY_LONG);	//Long press
			#else
				TRIGGER_CB(KEY_LONG_FREE);	//Long press free
			#endif
				
				//�������ʱ������������������״̬��¼
				btn->long_time = 0;
				btn->key_state = NONE_TRIGGER;
				//���°�����һ�εļ���¼Ϊ����
				btn->key_last_state = KEY_LONG;
			}
			break;
		}

		case KEY_DOUBLE:
		{
			btn->timer_count++;		//Update time
			if(btn->timer_count >= KEY_DOUBLE_TIME)//����˫��
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

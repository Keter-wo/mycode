#include "AppKey.h"

#include "bsp.h"
#include "key_core.h"
#include "app_log.h"
#include "app_timer.h"
#include "custom_board.h"

#if defined(PKG_USING_KEY_SAMPLE)||defined(RT_DEBUG_KEY)

/**< key polling interval interval (in units of 1ms). */
#define KEY_TIMER_INTERVAL		( 20 )

#define KEY_TRIGGER_LEVEL		( 0 )

/*******************************************************************
 *                          Variable Declaration
 *******************************************************************/
/* 定时器的控制块 */
static app_timer_id_t     g_app_key_timer_id;

/* 按键状态值 */
static key_t key_up;
static key_t key_down;
static key_t key_left;
static key_t key_right;
static key_t key_ok;

/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//按键定时器超时处理	//app_key_timeout_handler
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
static void key_timeout_handler(void* parameter)
{
	//按键处理过程
	key_process();
	
//	APP_LOG_INFO("Time out test\r\n");
}

/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//按键up处理
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//读取按键up的电平值
static uint8_t read_up_key_level(void)
{
	return hal_gpio_read_pin(GPIO_KEY_PORT, KEY_UP_PIN);
}

//按键单击处理
static void key_up_down_callback(void *btn)
{
	APP_LOG_INFO("[up]key Click!\r\n");
}

//按键双击处理
static void key_up_double_callback(void *btn)
{
	APP_LOG_INFO("[up]key double press!\r\n");
}

//按键长按处理
static void key_up_long_callback(void *btn)
{
	APP_LOG_INFO("[up]key Long press!\r\n");
}

/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//按键down处理
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//读取按键down的电平值
static uint8_t read_down_key_level(void)
{
	return hal_gpio_read_pin(GPIO_KEY_PORT, KEY_DOWN_PIN);
}

//按键单击处理
static void key_down_down_callback(void *btn)
{
	APP_LOG_INFO("[down]key Click!\r\n");
}

//按键双击处理
static void key_down_double_callback(void *btn)
{
	APP_LOG_INFO("[down]key double press!\r\n");
}

//按键长按处理
static void key_down_long_callback(void *btn)
{
	APP_LOG_INFO("[down]key Long press!\r\n");
}

//按键长按2s处理
static void key_down_long_2s_callback(void *btn)
{
	APP_LOG_INFO("[down]key Long press 2s !\r\n");
}

//按键长按3s处理
static void key_down_long_3s_callback(void *btn)
{
	APP_LOG_INFO("[down]key Long press 3s !\r\n");
}

/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//按键ok处理
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//读取按键ok的电平值
static uint8_t read_ok_key_level(void)
{
	return app_io_read_pin(APP_IO_TYPE_AON, KEY_OK_PIN);
}

//按键单击处理
static void key_ok_down_callback(void *btn)
{
	APP_LOG_INFO("[ok]key Click!\r\n");
}

//按键双击处理
static void key_ok_double_callback(void *btn)
{
	APP_LOG_INFO("[ok]key double press!\r\n");
}

//按键长按处理
static void key_ok_long_callback(void *btn)
{
  APP_LOG_INFO("[ok]key Long press!\r\n");
}


/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//按键left处理
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//读取按键left的电平值
static uint8_t read_left_key_level(void)
{
	return hal_gpio_read_pin(GPIO_KEY_PORT, KEY_LEFT_PIN);
}

//按键单击处理
static void key_left_down_callback(void *btn)
{
	APP_LOG_INFO("[left]key Click!\r\n");
}

//按键双击处理
static void key_left_double_callback(void *btn)
{
	APP_LOG_INFO("[left]key double press!\r\n");
}

//按键长按处理
static void key_left_long_callback(void *btn)
{
	APP_LOG_INFO("[left]key Long press!\r\n");
}

/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//按键right处理
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//读取按键right的电平值
static uint8_t read_right_key_level(void)
{
	return hal_gpio_read_pin(GPIO_KEY_PORT, KEY_RIGHT_PIN);
}

//按键单击处理
static void key_right_down_callback(void *btn)
{
	APP_LOG_INFO("[right]key Click!\r\n");
}

//按键双击处理
static void key_right_double_callback(void *btn)
{
	APP_LOG_INFO("[right]key double press!\r\n");
}

//按键长按处理
static void key_right_long_callback(void *btn)
{
	APP_LOG_INFO("[right]key Long press!\r\n");
}



/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//组合按键
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//KEYS_UP_DOWN
static void keys_up_down_callback(void *btn)
{
	APP_LOG_INFO("[keys_up_down]keys_up_down press!\r\n");
}

//KEYS_LEFT_RIGHT
static void keys_left_right_callback(void *btn)
{
	APP_LOG_INFO("[keys_left_right]keys_left_right press!\r\n");
}


/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
//初始化操作
/* -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*- */
static void Key_IO_init(void)
{
    gpio_init_t GPIO_InitStructure;

    GPIO_InitStructure.pin = KEY_UP_PIN;
    GPIO_InitStructure.mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.pull = LL_GPIO_PULL_UP;
    GPIO_InitStructure.mux = GPIO_PIN_MUX_GPIO;
    hal_gpio_init(GPIO0, &GPIO_InitStructure);

    GPIO_InitStructure.pin = KEY_DOWN_PIN;
    GPIO_InitStructure.mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.pull = LL_GPIO_PULL_UP;
    GPIO_InitStructure.mux = GPIO_PIN_MUX_GPIO;
    hal_gpio_init(GPIO0, &GPIO_InitStructure);

    GPIO_InitStructure.pin = KEY_LEFT_PIN;
    GPIO_InitStructure.mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.pull = LL_GPIO_PULL_UP;
    GPIO_InitStructure.mux = GPIO_PIN_MUX_GPIO;
    hal_gpio_init(GPIO0, &GPIO_InitStructure);

    GPIO_InitStructure.pin = KEY_RIGHT_PIN;
    GPIO_InitStructure.mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.pull = LL_GPIO_PULL_UP;
    GPIO_InitStructure.mux = GPIO_PIN_MUX_GPIO;
    hal_gpio_init(GPIO0, &GPIO_InitStructure);

	/*
	AON_GPIO，（Always-on GPIO）是常开域的GPIO口，
	在GR55xx进入睡眠之后，该域的GPIO不会进入睡眠，
	可以被外部事件用来唤醒芯片。
	*/
    //app_io_pin_state_t pin_state = APP_IO_PIN_SET;
    app_io_init_t io_init = APP_IO_DEFAULT_CONFIG;
    io_init.pull = APP_IO_PULLUP;
    io_init.pin  = KEY_OK_PIN;
    app_io_init(APP_IO_TYPE_AON, &io_init);
}



static int Key_State_init(void)
{
///* --------------------------------------------------------------- *///
//	key_create("up",&key_up, read_up_key_level, KEY_TRIGGER_LEVEL);
	key_create2("up",&key_up, read_up_key_level, KEY_TRIGGER_LEVEL, 0);
    
    /* --- 普通按键事件 ---- */
	key_attach(&key_up,KEY_DOWM,key_up_down_callback);                     
	key_attach(&key_up,KEY_DOUBLE,key_up_double_callback);                 
	key_attach(&key_up,KEY_LONG_FREE,key_up_long_callback);
	
    /* --- 组合按键事件 ---- */
    key_attach(&key_up,KEYS_UP_DOWN,keys_up_down_callback);
    
    /* --- 长按定时事件 ---- */
//	key_attach(&key_up,KEY_LONG_2s,key_up_long_2s_callback);
//	key_attach(&key_up,KEY_LONG_3s,key_up_long_3s_callback);
    
	//get_key_event(&key_up);//获取当前按键的事件

///* --------------------------------------------------------------- *///
//	key_create("down",&key_down, read_down_key_level, KEY_TRIGGER_LEVEL);	
	key_create2("down",&key_down, read_down_key_level, KEY_TRIGGER_LEVEL, 1);

    /* --- 普通按键事件 ---- */
	key_attach(&key_down,KEY_DOWM,key_down_down_callback);                     
	key_attach(&key_down,KEY_DOUBLE,key_down_double_callback);                 
	key_attach(&key_down,KEY_LONG_FREE,key_down_long_callback);
	
    /* --- 组合按键事件 ---- */
    key_attach(&key_down,KEYS_UP_DOWN,keys_up_down_callback);
    
    /* --- 长按定时事件 ---- */
	key_attach(&key_down,KEY_LONG_2s,key_down_long_2s_callback);
	key_attach(&key_down,KEY_LONG_3s,key_down_long_3s_callback);
	//get_key_event(&key_down);//获取当前按键的事件

///* --------------------------------------------------------------- *///
//	key_create("left",&key_left, read_left_key_level, KEY_TRIGGER_LEVEL);	
	key_create2("left",&key_left, read_left_key_level, KEY_TRIGGER_LEVEL, 2);	

    /* --- 普通按键事件 ---- */
	key_attach(&key_left,KEY_DOWM,key_left_down_callback);                     
	key_attach(&key_left,KEY_DOUBLE,key_left_double_callback);                 
	key_attach(&key_left,KEY_LONG_FREE,key_left_long_callback);
	
    /* --- 组合按键事件 ---- */
    key_attach(&key_left,KEYS_LEFT_RIGHT,keys_left_right_callback);

    /* --- 长按定时事件 ---- */
//	key_attach(&key_left,KEY_LONG_2s,key_left_long_2s_callback);
//	key_attach(&key_left,KEY_LONG_3s,key_left_long_3s_callback);
	//get_key_event(&key_left);//获取当前按键的事件

///* --------------------------------------------------------------- *///
//	key_create("right",&key_right, read_right_key_level, KEY_TRIGGER_LEVEL);	
	key_create2("right",&key_right, read_right_key_level, KEY_TRIGGER_LEVEL, 3);	

    /* --- 普通按键事件 ---- */
	key_attach(&key_right,KEY_DOWM,key_right_down_callback);                     
	key_attach(&key_right,KEY_DOUBLE,key_right_double_callback);                 
	key_attach(&key_right,KEY_LONG_FREE,key_right_long_callback);
	
    /* --- 组合按键事件 ---- */
    key_attach(&key_right,KEYS_LEFT_RIGHT,keys_left_right_callback);
    
    /* --- 长按定时事件 ---- */
//	key_attach(&key_right,KEY_LONG_2s,key_right_long_2s_callback);
//	key_attach(&key_right,KEY_LONG_3s,key_right_long_3s_callback);
	//get_key_event(&key_right);//获取当前按键的事件

///* --------------------------------------------------------------- *///
//	key_create("ok",&key_ok, read_ok_key_level, KEY_TRIGGER_LEVEL);	

    /* --- 普通按键事件 ---- */
	key_create2("ok",&key_ok, read_ok_key_level, KEY_TRIGGER_LEVEL, 4);	
	key_attach(&key_ok,KEY_DOWM,key_ok_down_callback);                     
	key_attach(&key_ok,KEY_DOUBLE,key_ok_double_callback);                 
	key_attach(&key_ok,KEY_LONG_FREE,key_ok_long_callback);
	
    /* --- 组合按键事件 ---- */
    
    /* --- 长按定时事件 ---- */
//	key_attach(&key_ok,KEY_LONG_2s,key_ok_long_2s_callback);
//	key_attach(&key_ok,KEY_LONG_3s,key_ok_long_3s_callback);
	//get_key_event(&key_ok);//获取当前按键的事件

///* --------------------------------------------------------------- *///
	return 0;
}

/*
 * GLOBAL FUNCTION DEFINITIONS
 *****************************************************************************************
 */
//按键功能初始化
void Key_Fun_init(void)
{
	//按键硬件初始化、按键状态值初始化、按键链表创建
	Key_IO_init();
    Key_State_init();
	
	/* app_key_timer_start */
	//创建一个定时器，设置定时器的计数模式：ATIMER_REPEAT，设置计时完成后执行的回调函数
    app_timer_create(&g_app_key_timer_id, ATIMER_REPEAT, key_timeout_handler);
    //设置计时20ms
	app_timer_start(g_app_key_timer_id, KEY_TIMER_INTERVAL, NULL);
	
    APP_LOG_INFO("app_timer_create and start\r\n");
}

#endif


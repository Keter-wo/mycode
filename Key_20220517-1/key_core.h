#ifndef __KEY_CORE_H__
#define __KEY_CORE_H__

#include "stdint.h"


#define KEY_LONG_DIFFERENT_TIME_FUN 0

//#define LONG_FREE_TRIGGER
//#define RT_USING_KEY_PRINT
#define SINGLE_AND_DOUBLE_TRIGGER

#define PKG_USING_KEY_SAMPLE  1
#define RT_DEBUG_KEY		  1
#define LONG_FREE_TRIGGE	  1

#define KEY_NAME_MAX          16	//名字最大为16字节
        
#ifndef KEY_DEBOUNCE_TIME
#define KEY_DEBOUNCE_TIME     2		//Debounce time  (n-1)*call cycle
#endif

#ifndef KEY_CONTINUOS_CYCLE
#define KEY_CONTINUOS_CYCLE   1		//Double-click the trigger cycle time  (n-1)*call cycle
#endif

#ifndef KEY_LONG_CYCLE
#define KEY_LONG_CYCLE        1		//Long press trigger cycle time   (n-1)*call cycle
#endif

#ifndef KEY_DOUBLE_TIME
#define KEY_DOUBLE_TIME      12		//Double click interval  (n-1)*call cycle  Recommended at 200-600ms
#endif

#ifndef KEY_LONG_TIME
#define KEY_LONG_TIME        25		//For n seconds ((n-1)*call cycle)ms, think long press event
#endif

#define KEY_LONG_TIME_2S	 101	//(101-1)*call cycle,eg:call cycle = 20ms,is 2S

#define KEY_LONG_TIME_3S	 151	//(151-1)*call cycle,eg:call cycle = 20ms,is 3S

#define COUNT_OVERFLOW		 200	//Count overflow//200只是个防止重新计数的保护值，只需要大于规划功能时间就可以


#define TRIGGER_CB(event)   \
        if(btn->callback_function[event]) \
			btn->callback_function[event]((key_t*)btn)

typedef void (*key_callback)(void*);   //The key triggers the callback function and needs to be implemented by the user.



typedef enum {
	KEY_DOWM = 0,
	KEY_UP,
	KEY_DOUBLE,
	KEY_LONG,
	KEY_LONG_2s,
	KEY_LONG_3s,
	KEY_LONG_FREE,

    /* --- Combination_key ---- */
	KEYS_UP_DOWN,
    KEYS_LEFT_RIGHT,
	
	KEYS_OK_UP,
    KEYS_OK_DOWN,
    KEYS_OK_LEFT,
    KEYS_OK_RIGHT,
    
	KEYS_LEFT_UP,
    KEYS_LEFT_DOWN,
    KEYS_RIGHT_UP,
    KEYS_RIGHT_DOWN,
    /* ------------------------ */
	
	KEY_ALL_TRIGGER,
	number_of_event,
	NONE_TRIGGER,
}Key_Event;


typedef struct key{
	uint8_t (*read_key_level)(void);

	char name[KEY_NAME_MAX];
	uint8_t Key_index_number	   :   4;	 /* Range: 0-15 */

	uint8_t key_state;                       /* The current state of the key (pressed or bounced) */
	uint8_t key_last_state;                  /* The last key state used to determine the double click */
	uint8_t key_trigger_level      :   2;    /* key trigger level */
	uint8_t key_last_level         :   2;    /* key current level */
	
	uint8_t key_trigger_event;               /* key trigger event, click, double click, long press, etc. */
	key_callback callback_function[number_of_event];
	uint8_t key_cycle;                       /* Continuous key cycle */
	uint8_t timer_count;                        /* Timing ，count KEY_UP time */
	uint8_t debounce_time;                      /* Debounce time */
	uint8_t long_time;                          /* key press duration */
	struct key *next;
}key_t;

typedef union All_Key_State{
	struct {
		uint8_t key_up			:   1;
		uint8_t key_down		:   1;
		uint8_t key_left		:   1;
		uint8_t key_right		:   1;
		uint8_t key_ok			:   1;
		
		uint8_t 			:   1;
		uint8_t 			:   1;
		uint8_t 			:   1;
	}bit;
	
	uint8_t state;
}keySTATE;

//双按键组合：C(5,2)=5!/(2!(5-2)!)=10种情况
#define UP_DOWN         ( 0x03 )
#define LEFT_RIGHT      ( 0x0C )

#define OK_UP           ( 0x11 )
#define OK_DOWN         ( 0x12 )
#define OK_LEFT         ( 0x14 )
#define OK_RIGHT        ( 0x18 )

#define LEFT_UP         ( 0x05 )
#define LEFT_DOWN       ( 0x06 )
#define RIGHT_UP        ( 0x09 )
#define RIGHT_DOWN      ( 0x0A )
/*
//三按键组合：C(5,3)=10种情况
#define UP_OK_DOWN		( 0x13 )
#define LEFT_OK_RIGHT	( 0x1C )
*/

#define setbit(x,y)		x|=(1<<y)	/* Specify a digit as 1 */
#define clrbit(x,y)		x&=~(1<<y)	/* Set a specified digit to 0 */


/* Function declaration for external calls */

void key_create(  const char *name,
                  key_t *btn, 
                  uint8_t(*read_btn_level)(void),
                  uint8_t btn_trigger_level);
				  
void key_create2(	const char *name,key_t *btn,
					uint8_t(*read_btn_level)(void),
					uint8_t btn_trigger_level, 
					uint8_t Key_index_number);
                  
void key_attach(  key_t *btn,
				  Key_Event btn_event,
				  key_callback btn_callback);   
 
void key_delete(key_t *btn);   	
		               
void key_process(void);
  
uint8_t get_key_event(key_t *btn);

uint8_t get_key_state(key_t *btn);

#if defined(RT_USING_KEY_PRINT)
void key_process_callback(void *btn);
 
void search_key(void);    

void get_key_eventlnfo(key_t *btn);

void print_key_info(key_t* btn);   
#endif

#endif


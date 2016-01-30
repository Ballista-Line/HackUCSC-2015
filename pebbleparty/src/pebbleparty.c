#include <pebble.h>

#define DAX 0
#define DAY 1
#define DAZ 2

typedef enum{
   main_vibe,
   second_vibe
}vibe_t;

static Window *window;
static TextLayer *text_layer;

int mx, my, mz; //Maximum x,y,z acceleration
int px, py, pz; //Previous x,y,z accerlation
bool vibrated = false;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  mx = my = mz = 0; //Reset recorded max accelerations
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  
}

void click_config_provider(void *context) {
   window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
   window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
   window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void inbox_received_callback(DictionaryIterator *iterator, void *context) {
   APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
   APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
   APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
   APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 160 } });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(text_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

void vibration_reset_handler(void* v){
   vibrated = false;
}

//integer sqrt, found on stackoverflow
uint32_t misqrt(uint32_t a_nInput)
{
    uint32_t op  = a_nInput;
    uint32_t res = 0;
    uint32_t one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type


    // "one" starts at the highest power of four <= than the argument.
    while (one > op)
    {
        one >>= 2;
    }

    while (one != 0)
    {
        if (op >= res + one)
        {
            op = op - (res + one);
            res = res +  2 * one;
        }
        res >>= 1;
        one >>= 2;
    }
    return res;
}

int max(int x, int y){
   if (x > y) return x;
   return y;
}

//Custom vibration timeout as the default doesn't work
void controlled_vibrate(vibe_t mode){
      if(mode == main_vibe) vibes_short_pulse();
      if(mode == second_vibe) vibes_double_pulse();
      vibrated = true;
      app_timer_register(300, vibration_reset_handler, NULL);
}

void data_handler(AccelData *data, uint32_t samples){
   int x = data[0].x;
   int y = data[0].y;
   int z = data[0].z;

   //Check if there is a new maximum acceleration
   if(x > mx) mx = x;
   if(y > my) my = y;
   if(z > mz) mz = z;

   //Find change in accerlation
   int dx=x-px;
   int dy=y-py;
   int dz=z-pz;

   //Round values near 0 to 0
   //Gravity is very problematic
   if(abs(dx) < 100) dx = 0;
   if(abs(dy) < 100) dy = 0;
   if(abs(dz) < 100) dz = 0;

   //Sends delta A results to the phone for external use
   //This WILL have to be limited for performance reasons
   if((abs(dx) | abs(dy) | abs(dz)) > 1000) {
      DictionaryIterator *iterator;
      app_message_outbox_begin(&iterator);   

      dict_write_int(iterator, DAX, &dx, sizeof(dx), true /* signed */);
      dict_write_int(iterator, DAY, &dy, sizeof(dy), true /* signed */);
      dict_write_int(iterator, DAZ, &dz, sizeof(dz), true /* signed */);

      app_message_outbox_send();
   }
   //Vibrate if acceleration over a threshold
   if (!vibrated && max(abs(x), max(abs(y), abs(z))) > 2000 ) {
      controlled_vibrate(main_vibe);
   }

   //Prints to the pebble
   static char sbuffer[128];
   snprintf(sbuffer, sizeof(sbuffer), 
    "X,Y,Z\n %d,%d,%d\n %d,%d,%d \n %d,%d,%d\n%d", 
    x, y, z, 
    mx, my, mz,
    dx, dy, dz, vibrated
  );

   text_layer_set_text(text_layer, sbuffer);

   //Set new previous accerlations
   px = x;
   py = y;
   pz = z;
}


void init(void) {
   mx = 0;
   my = 0;
   mz = 0;

   //Sets Pebble <-> Phone message callbacks
   app_message_register_inbox_received(inbox_received_callback);
   app_message_register_inbox_dropped(inbox_dropped_callback);
   app_message_register_outbox_failed(outbox_failed_callback);
   app_message_register_outbox_sent(outbox_sent_callback);

   //Message buffer with inbox and outbox size of 1024 
   app_message_open(128, 4096);

   window = window_create();
   window_set_click_config_provider(window, click_config_provider);
   window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
   });

   uint32_t samples = 1;
   accel_data_service_subscribe(samples, data_handler);
   accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);

   const bool animated = true;
   window_stack_push(window, animated);
}

void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

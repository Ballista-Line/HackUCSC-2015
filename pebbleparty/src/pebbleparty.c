#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

static int mx, my, mz;
static bool vibrated = false;


/*
  These will be used to navigate the menus
*/

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  mx = my = mz = 0; //Reset recorded max accelerations
}
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  
}
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 160 } });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(text_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void vibration_reset_handler(void* v){
   vibrated = false;
}


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
void controlled_vibrate(){
      vibes_short_pulse();
      vibrated = true;
      app_timer_register(300, vibration_reset_handler, NULL);
}

static void data_handler(AccelData *data, uint32_t samples){
   static char sbuffer[128];

   int x = data[0].x;
   int y = data[0].y;
   int z = data[0].z;


   snprintf(sbuffer, sizeof(sbuffer), 
    "X,Y,Z\n %d,%d,%d\n %d,%d,%d \n%d", 
    x, y, z, 
    mx, my, mz, vibrated
  );

   if(x > mx) mx = x;
   if(y > my) my = y;
   if(z > mz) mz = z;


   if (!vibrated && max(abs(x), max(abs(y), abs(z))) > 3000 ) {
      controlled_vibrate();
   }


   text_layer_set_text(text_layer, sbuffer);
}


static void init(void) {
   mx = 0;
   my = 0;
   mz = 0;

   window = window_create();
   window_set_click_config_provider(window, click_config_provider);
   window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });

   uint32_t samples = 1;
   accel_data_service_subscribe(samples, data_handler);
   accel_service_set_sampling_rate(ACCEL_SAMPLING_50HZ);


  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

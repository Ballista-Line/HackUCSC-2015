#include <pebble.h>

#define DEBUG 

//AppMessage keys
#define DAX 0
#define DAY 1
#define DAZ 2
#define ACTION 3
#define MAGA 4
#define ADDRESS 5
#define MESSAGE 6
#define CONNECTED 7

//Action modes
#define MAIN 0
#define ALT 1
#define SWINGDOWN 2

//Times, in ms
#define ACTION_RESET_DELAY 500
#define VIBE_RESET_DELAY 500

//Doesn't work as a variable for some reason
#define ACCEL_SAMPLES 5

const char* primary_server = "panopticon.ballistaline.com/pebble.php";
const char* secondary_server = "faustfamily.me";
const char* custom_server = "169.233.11.119";

static Window* window;
static TextLayer* text_layer;

static ActionBarLayer* action_bar;

static ActionMenu* action_menu;
static ActionMenuLevel* root_level;

static GBitmap* menu_bmp;



typedef enum {
  primary,
  secondary,
  custom
} ServerType;

typedef struct {
  ServerType type;
} Context;



int mx, my, mz; //Maximum x,y,z acceleration
int px, py, pz; //Previous x,y,z accerlation
bool vibrated = false;
bool recent_action = false;
bool connected = false;

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
   mx = my = mz = 0; //Reset recorded max accelerations

   ActionMenuConfig config = (ActionMenuConfig) {
      .root_level = root_level,
      .colors = {
        .background = GColorChromeYellow,
        .foreground = GColorBlack,
      },
      .align = ActionMenuAlignCenter
   };

   action_menu = action_menu_open(&config);

}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
   ////TO BE MOVED LATER////
   // will be selectable

}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  
}

void click_config_provider(void *context) {
   window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
   window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
   window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void action_performed_callback(ActionMenu *action_menu, const ActionMenuItem *action, void *context) {
   ServerType server = (ServerType)action_menu_item_get_action_data(action);

   DictionaryIterator *iterator;
   app_message_outbox_begin(&iterator);

   const char* selected_server = secondary_server;

   if(server == primary) {
      selected_server = primary_server;
   } 
   else if (server == secondary) {
      selected_server = secondary_server;
   } 
   else {
      selected_server = custom_server;
   }

   dict_write_cstring(iterator, ADDRESS, selected_server);
   app_message_outbox_send();

   text_layer_set_text(text_layer, "Connecting...");
}

void inbox_received_callback(DictionaryIterator *iterator, void *context) {
   APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");

   
   Tuple* data = dict_find(iterator, MESSAGE);
   if (data) {
      //snprintf(s_buffer, sizeof(s_buffer), "Received '%s'", data->value->cstring);
      text_layer_set_text(text_layer, data->value->cstring);
   }

   data = dict_find(iterator, CONNECTED);
   if (data) {
      //snprintf(s_buffer, sizeof(s_buffer), "Received '%s'", data->value->cstring);
      connected = data->value->uint32;
   }

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
   /*
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 160 } });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(text_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  layer_add_child(window_layer, text_layer_get_layer(text_layer));*/

   Layer *window_layer = window_get_root_layer(window);
   GRect bounds = layer_get_bounds(window_layer);

   menu_bmp = gbitmap_create_with_resource(RESOURCE_ID_MENUICON);

   action_bar = action_bar_layer_create();
   action_bar_layer_set_click_config_provider(action_bar, click_config_provider);
   action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, menu_bmp);
   action_bar_layer_add_to_window(action_bar, window);

   text_layer = text_layer_create(GRect(0, 0, bounds.size.w - ACTION_BAR_WIDTH, bounds.size.h));
   text_layer_set_text(text_layer, "Welcome to Pebble Party. Choose a server from the menu.");
   text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
   text_layer_set_text_color(text_layer, GColorBlack);
   text_layer_set_background_color(text_layer, GColorClear);
   text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
   layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

void vibration_reset_handler(void* v){
   vibrated = false;
}

void action_delay_handler(void* v){
   recent_action = false;
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
void controlled_vibrate(int mode){
      vibrated = true;

      if(mode == MAIN) vibes_short_pulse();
      if(mode == ALT) vibes_double_pulse();
      app_timer_register(VIBE_RESET_DELAY, vibration_reset_handler, NULL);
}

void send_action(int mode, int magnitude) {
   recent_action = true;

   DictionaryIterator *iterator;
   app_message_outbox_begin(&iterator);
   dict_write_int(iterator, ACTION, &mode, sizeof(int), true);
   dict_write_int(iterator, MAGA, &magnitude, sizeof(int), true);
   app_message_outbox_send();

   app_timer_register(ACTION_RESET_DELAY, action_delay_handler, NULL);
}

void input_handler(AccelData *data, uint32_t samples){
   int x = 0;
   int y = 0;
   int z = 0;

   //Average the acceleration samples
   for(unsigned int i = 0; i < ACCEL_SAMPLES; i++){
      x += data[i].x;
      y += data[i].y;
      z += data[i].z;
   }

   x /= ACCEL_SAMPLES;
   y /= ACCEL_SAMPLES;
   z /= ACCEL_SAMPLES;

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
  // if(abs(dx) < 100) dx = 0;
  // if(abs(dy) < 100) dy = 0;
  // if(abs(dz) < 100) dz = 0;

   //Determine if an action has been performed
   if(((abs(dx) > 1500) || (abs(dy) > 1500) || (abs(dz) > 1500)) && !recent_action) {


      int magna = misqrt(dx*dx + dy*dy + dz*dz);

      #ifdef DEBUG
         DictionaryIterator *iterator;
         app_message_outbox_begin(&iterator);

         dict_write_int(iterator, DAX, &dx, sizeof(dx), true);
         dict_write_int(iterator, DAY, &dy, sizeof(dy), true);
         dict_write_int(iterator, DAZ, &dz, sizeof(dz), true);


         int mode = -1;
         /*
         bool forward = abs(dx) > abs(dy);
         bool sideside = abs(dy) + abs(dz) > abs(dx);

         if(forward) mode = 0;
         if(sideside) mode = 1;
         if(forward&&sideside) mode = 2;
         */

         
         if (abs(dx) > abs(dy)) {
            mode = 0;
         }
         else
         if (abs(dy) + abs(dz) > abs(dx)){
            mode = 1;
         }

         if(mode > -1) {
            recent_action = true;
            dict_write_int(iterator, ACTION, &mode, sizeof(int), true);
            dict_write_int(iterator, MAGA, &magna, sizeof(int), true);
            app_timer_register(ACTION_RESET_DELAY, action_delay_handler, NULL);
            controlled_vibrate(mode);
         }

         app_message_outbox_send();


      #else
         if (abs(dx) > abs(dy)) send_action(MAIN, magna);
         else
         if (abs(dy) + abs(dz) > abs(dx)) send_action(ALT, magna);
      #endif
   }

   if(connected){
   //Prints to the pebble
   static char sbuffer[128];
   snprintf(sbuffer, sizeof(sbuffer), 
    "Connected \n---------------\n %d\n%d\n%d\n---------------", 
    dx, dy, dz
    //mx, my, mz,
    //dx, dy, dz, vibrated
  );

   text_layer_set_text(text_layer, sbuffer);
   }
   //Set new previous accerlations
   px = x;
   py = y;
   pz = z;
}

void init_action_menu(){
   root_level = action_menu_level_create(3);

   action_menu_level_add_action(root_level, "Primary server", action_performed_callback, (void*)primary);
   action_menu_level_add_action(root_level, "Backup server", action_performed_callback, (void*)secondary);
   action_menu_level_add_action(root_level, "Custom server", action_performed_callback, (void*)custom);

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

   accel_data_service_subscribe(ACCEL_SAMPLES, input_handler);
   accel_service_set_sampling_rate(ACCEL_SAMPLING_50HZ);

   const bool animated = true;
   window_stack_push(window, animated);

   init_action_menu();
}

void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

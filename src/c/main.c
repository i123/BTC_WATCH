#include <pebble.h>

static Window *s_main_window;
static Layer *window_layer;
static TextLayer *s_time_layer;
static TextLayer *s_wday_layer;
static TextLayer *s_ccytitle_layer;
static TextLayer *s_ccyvalue_layer;
static TextLayer *s_ccytitle2_layer;
static TextLayer *s_ccyvalue2_layer;

static GFont s_time_font;
static GFont s_wday_font;
static GFont s_ccytitle_font;
static GFont s_ccyvalue_font;

// static BitmapLayer *s_background_layer;
// static GBitmap *s_background_bitmap;

static const uint8_t s_time_offset_top_percent = 31;
static const uint8_t s_ccy_offset_top_percent = 82;
static const uint8_t s_ccytitle_offset_top_percent = 85;
static const uint8_t s_ccy2_offset_top_percent = 72;
static const uint8_t s_ccy2title_offset_top_percent = 75;

static const uint8_t s_wday_offset_top_percent =0;
static int s_battery_level;
static Layer *s_battery_layer;

// static BitmapLayer *s_background_layer, *s_bt_icon_layer;
// static GBitmap *s_background_bitmap, *s_bt_icon_bitmap;
static BitmapLayer  *s_bt_icon_layer;
static GBitmap *s_bt_icon_bitmap;

static void bluetooth_callback(bool connected) {
  // Show icon if disconnected
  // layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), false);
  if(connected) {
		layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), false);
		// layer_mark_dirty(s_bt_icon_layer);
	}else{	
		layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), true);
    // Issue a vibrating alert
    vibes_double_pulse();
  }
}

static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
  
  // Update meter
  layer_mark_dirty(s_battery_layer);
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // Find the width of the bar (total width = 114px)
  int width = (s_battery_level * 16) / 100;

  // Draw the background
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
	//graphics_fill_rect(ctx, bounds, 0, GColorCyan);
  // Draw the bar
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_rect(ctx, GRect(0, 0, 20, bounds.size.h));
	graphics_draw_rect(ctx, GRect(20, 3, 2, 6));
  // Draw the bar
  graphics_context_set_fill_color(ctx, GColorGreen);
  graphics_fill_rect(ctx, GRect(2, 2, width, 8), 0, GCornerNone);
}

uint8_t relative_pixel(uint8_t percent, uint8_t max) {
  return (max * percent) / 100;
}

static void update_ui(void) {
  // Adapt the layout based on any obstructions
  GRect full_bounds = layer_get_bounds(window_layer);
  GRect unobstructed_bounds = layer_get_unobstructed_bounds(window_layer);

  if (!grect_equal(&full_bounds, &unobstructed_bounds)) {
    // Screen is obstructed
    //layer_set_hidden(bitmap_layer_get_layer(s_background_layer), true);
    text_layer_set_text_color(s_time_layer, GColorWhite);
  } else {
    // Screen is unobstructed
    //layer_set_hidden(bitmap_layer_get_layer(s_background_layer), false);
    text_layer_set_text_color(s_time_layer, GColorWhite);
  }

  GRect time_frame = layer_get_frame(text_layer_get_layer(s_time_layer));
  time_frame.origin.y = relative_pixel(s_time_offset_top_percent, unobstructed_bounds.size.h);
  layer_set_frame(text_layer_get_layer(s_time_layer), time_frame);

//   GRect weather_frame = layer_get_frame(text_layer_get_layer(s_weather_layer));
//   weather_frame.origin.y = relative_pixel(s_weather_offset_top_percent, unobstructed_bounds.size.h);
//   layer_set_frame(text_layer_get_layer(s_weather_layer), weather_frame);

	GRect ccytitle_frame = layer_get_frame(text_layer_get_layer(s_ccytitle_layer));
	ccytitle_frame.origin.y = relative_pixel(s_ccytitle_offset_top_percent, unobstructed_bounds.size.h);
	layer_set_frame(text_layer_get_layer(s_ccytitle_layer), ccytitle_frame);
	
	GRect ccyvalue_frame = layer_get_frame(text_layer_get_layer(s_ccyvalue_layer));
	ccyvalue_frame.origin.y = relative_pixel(s_ccy_offset_top_percent, unobstructed_bounds.size.h);
	layer_set_frame(text_layer_get_layer(s_ccyvalue_layer), ccyvalue_frame);

	
	GRect ccytitle2_frame = layer_get_frame(text_layer_get_layer(s_ccytitle2_layer));
	ccytitle2_frame.origin.y = relative_pixel(s_ccy2title_offset_top_percent, unobstructed_bounds.size.h);
	layer_set_frame(text_layer_get_layer(s_ccytitle2_layer), ccytitle2_frame);
	
	GRect ccyvalue2_frame = layer_get_frame(text_layer_get_layer(s_ccyvalue2_layer));
	ccyvalue2_frame.origin.y = relative_pixel(s_ccy2_offset_top_percent, unobstructed_bounds.size.h);
	layer_set_frame(text_layer_get_layer(s_ccyvalue2_layer), ccyvalue2_frame);
	
	GRect wday_frame = layer_get_frame(text_layer_get_layer(s_wday_layer));
  wday_frame.origin.y = relative_pixel(s_wday_offset_top_percent, unobstructed_bounds.size.h);
  layer_set_frame(text_layer_get_layer(s_wday_layer), wday_frame);

}

static void initialise_ui(void) {
  GRect bounds = layer_get_bounds(window_layer);

  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0,
    relative_pixel(s_time_offset_top_percent, bounds.size.h), bounds.size.w, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");

	s_time_font = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);

  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // Create CCY Layer
  s_ccyvalue_layer = text_layer_create(GRect(0,
    relative_pixel(s_ccy_offset_top_percent, bounds.size.h), bounds.size.w, 25));
  text_layer_set_background_color(s_ccyvalue_layer, GColorClear);
  text_layer_set_text_color(s_ccyvalue_layer, GColorWhite);
  text_layer_set_text_alignment(s_ccyvalue_layer, GTextAlignmentRight);
  text_layer_set_text(s_ccyvalue_layer, "Loading...");
	s_ccyvalue_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  text_layer_set_font(s_ccyvalue_layer, s_ccyvalue_font);
	layer_add_child(window_layer, text_layer_get_layer(s_ccyvalue_layer));
	
  // Create CCY Value Layer
  s_ccytitle_layer = text_layer_create(GRect(0,
    relative_pixel(s_ccy_offset_top_percent, bounds.size.h), bounds.size.w, 15));
  text_layer_set_background_color(s_ccytitle_layer, GColorClear);
  text_layer_set_text_color(s_ccytitle_layer, GColorGreen);
  text_layer_set_text_alignment(s_ccytitle_layer, GTextAlignmentLeft);
  text_layer_set_text(s_ccytitle_layer, "Loading...");
  // Create second custom font, apply it and add to Window
  s_ccytitle_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  text_layer_set_font(s_ccytitle_layer, s_ccytitle_font);
  layer_add_child(window_layer, text_layer_get_layer(s_ccytitle_layer));
	
  // Create CCY2 Layer
  s_ccyvalue2_layer = text_layer_create(GRect(0,
    relative_pixel(s_ccy2_offset_top_percent, bounds.size.h), bounds.size.w, 25));
  text_layer_set_background_color(s_ccyvalue2_layer, GColorClear);
  text_layer_set_text_color(s_ccyvalue2_layer, GColorWhite);
  text_layer_set_text_alignment(s_ccyvalue2_layer, GTextAlignmentRight);
  text_layer_set_text(s_ccyvalue2_layer, "Loading...");
  text_layer_set_font(s_ccyvalue2_layer, s_ccyvalue_font);
	layer_add_child(window_layer, text_layer_get_layer(s_ccyvalue2_layer));
	
  // Create CCY2 Value Layer
  s_ccytitle2_layer = text_layer_create(GRect(0,
    relative_pixel(s_ccy2_offset_top_percent, bounds.size.h), bounds.size.w, 15));
  text_layer_set_background_color(s_ccytitle2_layer, GColorClear);
  text_layer_set_text_color(s_ccytitle2_layer, GColorGreen);
  text_layer_set_text_alignment(s_ccytitle2_layer, GTextAlignmentLeft);
  text_layer_set_text(s_ccytitle2_layer, "Loading...");
  // Create second custom font, apply it and add to Window
  text_layer_set_font(s_ccytitle2_layer, s_ccytitle_font);
  layer_add_child(window_layer, text_layer_get_layer(s_ccytitle2_layer));
	
	// Create WeekDay Layer
  s_wday_layer = text_layer_create(GRect(0,
    relative_pixel(s_wday_offset_top_percent, bounds.size.h), bounds.size.w, 25));
  text_layer_set_background_color(s_wday_layer, GColorClear);
  text_layer_set_text_color(s_wday_layer, GColorWhite);
  text_layer_set_text_alignment(s_wday_layer, GTextAlignmentLeft);
  text_layer_set_text(s_wday_layer, "Loading...");

	s_wday_font = fonts_get_system_font(FONT_KEY_GOTHIC_24);
  text_layer_set_font(s_wday_layer, s_wday_font);
  layer_add_child(window_layer, text_layer_get_layer(s_wday_layer));
	
  // Check for obstructions
  update_ui();
}

static void destroy_ui(void) {
  // gbitmap_destroy(s_background_bitmap);
  // bitmap_layer_destroy(s_background_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_ccytitle_layer);
	text_layer_destroy(s_ccyvalue_layer);
  text_layer_destroy(s_ccytitle2_layer);
	text_layer_destroy(s_ccyvalue2_layer);
	text_layer_destroy(s_wday_layer);
  layer_destroy(s_battery_layer);
}

static void update_time(void) {
 // Get a tm structure
 time_t temp = time(NULL);
 struct tm *tick_time = localtime(&temp);

 // Write the current hours and minutes into a buffer
 static char s_buffer[8];
 strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

 // Display this time on the TextLayer
 text_layer_set_text(s_time_layer, s_buffer);
	
 static char w_buffer[15];
 strftime(w_buffer, sizeof(w_buffer), "%m/%d %a", tick_time);
 text_layer_set_text(s_wday_layer, w_buffer);
}

static void update_ccy(void) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint8(iter, 0, 0);
  app_message_outbox_send();
}

static void app_unobstructed_change(AnimationProgress progress, void *context) {
  update_ui();
}

static void main_window_load(Window *window) {
  window_layer = window_get_root_layer(window);

  // Create the UI elements
  initialise_ui();

  // Make sure the time is displayed from the start
  update_time();

  // Subscribe to the change event
  UnobstructedAreaHandlers handlers = {
    .change = app_unobstructed_change
  };
  unobstructed_area_service_subscribe(handlers, NULL);

  // Create battery meter Layer
  s_battery_layer = layer_create(GRect(122, 11, 22, 12));
  layer_set_update_proc(s_battery_layer, battery_update_proc);

  // Add to Window
  layer_add_child(window_get_root_layer(window), s_battery_layer);  
  
  // Create the Bluetooth icon GBitmap
  s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ICON);

  // Create the BitmapLayer to display the GBitmap
  s_bt_icon_layer = bitmap_layer_create(GRect(106, 8, 16, 16));
  bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_icon_layer));
  
  // Show the correct state of the BT connection from the start
  bluetooth_callback(connection_service_peek_pebble_app_connection());
}

static void main_window_unload(Window *window) {
  // fonts_unload_custom_font(s_time_font);
  // fonts_unload_custom_font(s_weather_font);
  gbitmap_destroy(s_bt_icon_bitmap);
  bitmap_layer_destroy(s_bt_icon_layer);
  unobstructed_area_service_unsubscribe();

  // Clean up the unused UI elenents
  destroy_ui();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();

  // Get ccy update every 30 minutes
  if(tick_time->tm_min % 2 == 0) {
      update_ccy();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char ccy1_value_buffer[8];
  //static char change_buffer[32];
  static char ccy1_title_buffer[32];
	static char ccy2_title_buffer[32];
	static char ccy2_value_buffer[32];
	
  // Read tuples for data
  Tuple *last_tuple = dict_find(iterator, MESSAGE_KEY_Usdt_btc_last);
  //Tuple *change_tuple = dict_find(iterator, MESSAGE_KEY_Usdt_btc_change);
	Tuple *volume_tuple = dict_find(iterator, MESSAGE_KEY_Usdt_btc_volume);
	Tuple *btc_tuple = dict_find(iterator, MESSAGE_KEY_BTC_USD);
	Tuple *usdjpy_tuple = dict_find(iterator, MESSAGE_KEY_USD_JPY);
	
	if(btc_tuple && usdjpy_tuple){
		snprintf(ccy2_title_buffer, sizeof(ccy2_title_buffer), "USDJPY");
		snprintf(ccy2_value_buffer, sizeof(ccy2_value_buffer), "%s", usdjpy_tuple->value->cstring);
		text_layer_set_text(s_ccytitle2_layer, ccy2_title_buffer);
		text_layer_set_text(s_ccyvalue2_layer, ccy2_value_buffer);
	}
	
  // If Polo data is available, use it
  if(last_tuple && volume_tuple) {
		// snprintf(change_buffer, sizeof(change_buffer), "USDT, %s", change_tuple->value->cstring);
		snprintf(ccy1_title_buffer, sizeof(ccy1_title_buffer), "USDT, %s", volume_tuple->value->cstring);
		snprintf(ccy1_value_buffer, sizeof(ccy1_value_buffer), "%s", last_tuple->value->cstring);
    // Assemble full string and display
		text_layer_set_text(s_ccytitle_layer, ccy1_title_buffer);
    text_layer_set_text(s_ccyvalue_layer, ccy1_value_buffer);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
  
  // Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  // Ensure battery level is displayed from the start
  battery_callback(battery_state_service_peek());
  
  // Register for Bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

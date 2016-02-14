/*
Copyright (C) 2016 Mark Reed

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "pebble.h"

static Window *window;
static Layer *window_layer;

static GBitmap *separator_image;
static BitmapLayer *separator_layer;
static BitmapLayer *separator_layer2;

static GBitmap *time_format_image;
static BitmapLayer *time_format_layer;

static GFont moody_font;
static GFont moody_font2;

const int MONTH_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_MONTH_JAN,
  RESOURCE_ID_IMAGE_MONTH_FEB,
  RESOURCE_ID_IMAGE_MONTH_MAR,
  RESOURCE_ID_IMAGE_MONTH_APR,
  RESOURCE_ID_IMAGE_MONTH_MAY,
  RESOURCE_ID_IMAGE_MONTH_JUN,
  RESOURCE_ID_IMAGE_MONTH_JUL,
  RESOURCE_ID_IMAGE_MONTH_AUG,
  RESOURCE_ID_IMAGE_MONTH_SEP,
  RESOURCE_ID_IMAGE_MONTH_OCT,
  RESOURCE_ID_IMAGE_MONTH_NOV,
  RESOURCE_ID_IMAGE_MONTH_DEC
};

const int DAY_NAME_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DAY_NAME_SUN,
  RESOURCE_ID_IMAGE_DAY_NAME_MON,
  RESOURCE_ID_IMAGE_DAY_NAME_TUE,
  RESOURCE_ID_IMAGE_DAY_NAME_WED,
  RESOURCE_ID_IMAGE_DAY_NAME_THU,
  RESOURCE_ID_IMAGE_DAY_NAME_FRI,
  RESOURCE_ID_IMAGE_DAY_NAME_SAT
};

static GBitmap *day_name_image;
static BitmapLayer *day_name_layer;

static GBitmap *month_image;
static BitmapLayer *month_layer;

#define TOTAL_DATE_DIGITS 2
static GBitmap *date_digits_images[TOTAL_DATE_DIGITS];
static BitmapLayer *date_digits_layers[TOTAL_DATE_DIGITS];

const int DATENUM_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DATENUM_0,
  RESOURCE_ID_IMAGE_DATENUM_1,
  RESOURCE_ID_IMAGE_DATENUM_2,
  RESOURCE_ID_IMAGE_DATENUM_3,
  RESOURCE_ID_IMAGE_DATENUM_4,
  RESOURCE_ID_IMAGE_DATENUM_5,
  RESOURCE_ID_IMAGE_DATENUM_6,
  RESOURCE_ID_IMAGE_DATENUM_7,
  RESOURCE_ID_IMAGE_DATENUM_8,
  RESOURCE_ID_IMAGE_DATENUM_9
};

#define TOTAL_TIME_DIGITS 4
static GBitmap *time_digits_images[TOTAL_TIME_DIGITS];
static BitmapLayer *time_digits_layers[TOTAL_TIME_DIGITS];

const int BIG_DIGIT_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_NUM_0,
  RESOURCE_ID_IMAGE_NUM_1,
  RESOURCE_ID_IMAGE_NUM_2,
  RESOURCE_ID_IMAGE_NUM_3,
  RESOURCE_ID_IMAGE_NUM_4,
  RESOURCE_ID_IMAGE_NUM_5,
  RESOURCE_ID_IMAGE_NUM_6,
  RESOURCE_ID_IMAGE_NUM_7,
  RESOURCE_ID_IMAGE_NUM_8,
  RESOURCE_ID_IMAGE_NUM_9
};

static TextLayer *steps_label, *dist_label;

static GBitmap *footprint_icon;
static BitmapLayer *footprint_layer;

static GBitmap *dark8_icon;
static BitmapLayer *dark8_layer;

static void health_handler(HealthEventType event, void *context) {
  static char s_value_buffer[8];
  static char s_value_buffer2[8];
  if (event == HealthEventMovementUpdate) {
    // display the step count
    snprintf(s_value_buffer, sizeof(s_value_buffer), "%d", (int)health_service_sum_today(HealthMetricStepCount));
    snprintf(s_value_buffer2, sizeof(s_value_buffer2), "%d", (int)health_service_sum_today(HealthMetricWalkedDistanceMeters));
	  
    text_layer_set_text(steps_label, s_value_buffer);
    text_layer_set_text(dist_label, s_value_buffer2);
  }
}

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
  GBitmap *old_image = *bmp_image;

  *bmp_image = gbitmap_create_with_resource(resource_id);
	
  GRect bounds = gbitmap_get_bounds(*bmp_image);

  GRect main_frame = GRect(origin.x, origin.y, bounds.size.w, bounds.size.h);
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  layer_set_frame(bitmap_layer_get_layer(bmp_layer), main_frame);

  if (old_image != NULL) {
  	gbitmap_destroy(old_image);
  }
}

void handle_bluetooth(bool connected) {
 if (!connected) {    

	vibes_short_pulse();
	 
    }
}

void force_update_a(void) {
    handle_bluetooth(bluetooth_connection_service_peek());

}

static unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }

  unsigned short display_hour = hour % 12;

  // Converts "0" to "12"
  return display_hour ? display_hour : 12;
}

static void update_time (struct tm *current_time) {
  unsigned short display_hour = get_display_hour(current_time->tm_hour);

#ifdef PBL_PLATFORM_CHALK
	int yPos = 28;
#else
	int yPos = 58;
#endif	
	
#ifdef PBL_PLATFORM_CHALK
  set_container_image(&time_digits_images[0], time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(22, yPos));
  set_container_image(&time_digits_images[1], time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(52, yPos));

  set_container_image(&time_digits_images[2], time_digits_layers[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(90, yPos));
  set_container_image(&time_digits_images[3], time_digits_layers[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(119, yPos));
#else
  set_container_image(&time_digits_images[0], time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(15, yPos));
  set_container_image(&time_digits_images[1], time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(45, yPos));

  set_container_image(&time_digits_images[2], time_digits_layers[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(83, yPos));
  set_container_image(&time_digits_images[3], time_digits_layers[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(112, yPos));
#endif		

	
  if (!clock_is_24h_style()) {
 
#ifdef PBL_PLATFORM_CHALK
  set_container_image(&time_digits_images[0], time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(20, yPos));
  set_container_image(&time_digits_images[1], time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(50, yPos));

  set_container_image(&time_digits_images[2], time_digits_layers[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(88, yPos));
  set_container_image(&time_digits_images[3], time_digits_layers[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(117, yPos));
#else
 set_container_image(&time_digits_images[0], time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(0, yPos));
  set_container_image(&time_digits_images[1], time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(31, yPos));

  set_container_image(&time_digits_images[2], time_digits_layers[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(68, yPos));
  set_container_image(&time_digits_images[3], time_digits_layers[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(99, yPos));
#endif		

	
 
	  if (current_time->tm_hour >= 12) {
		  
#ifdef PBL_PLATFORM_CHALK
      set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_PM_MODE, GPoint(146, 52));
      layer_set_hidden(bitmap_layer_get_layer(time_format_layer), false);	

#else
      set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_PM_MODE, GPoint(132, 61));
      layer_set_hidden(bitmap_layer_get_layer(time_format_layer), false);
#endif		
    } 
    else {
	  
#ifdef PBL_PLATFORM_CHALK
	  set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_AM_MODE, GPoint(146, 52));
      layer_set_hidden(bitmap_layer_get_layer(time_format_layer), false);		
		
#else
	  set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_AM_MODE, GPoint(132, 61));
      layer_set_hidden(bitmap_layer_get_layer(time_format_layer), false);
#endif		
    }

    if (display_hour/10 == 0) {
    	layer_set_hidden(bitmap_layer_get_layer(time_digits_layers[0]), true);
    } else {
    	layer_set_hidden(bitmap_layer_get_layer(time_digits_layers[0]), false);
    }
  }
}



static void update_date (int day_of_month, int days_since_sunday) {

#ifdef PBL_PLATFORM_CHALK
  set_container_image(&day_name_image, day_name_layer, DAY_NAME_IMAGE_RESOURCE_IDS[days_since_sunday], GPoint(0, 113));
  set_container_image(&date_digits_images[0], date_digits_layers[0], DATENUM_IMAGE_RESOURCE_IDS[day_of_month/10], GPoint(74, 156));
  set_container_image(&date_digits_images[1], date_digits_layers[1], DATENUM_IMAGE_RESOURCE_IDS[day_of_month%10], GPoint(88, 157));
#else
  set_container_image(&day_name_image, day_name_layer, DAY_NAME_IMAGE_RESOURCE_IDS[days_since_sunday], GPoint(0, 5));
  set_container_image(&date_digits_images[0], date_digits_layers[0], DATENUM_IMAGE_RESOURCE_IDS[day_of_month/10], GPoint(114, 30));
  set_container_image(&date_digits_images[1], date_digits_layers[1], DATENUM_IMAGE_RESOURCE_IDS[day_of_month%10], GPoint(128, 30));
#endif		

}

static void update_month (int months_since_january) {
  int month_value = months_since_january;
#ifdef PBL_PLATFORM_CHALK
  set_container_image(&month_image, month_layer, MONTH_IMAGE_RESOURCE_IDS[month_value], GPoint(0, 137));
#else
  set_container_image(&month_image, month_layer, MONTH_IMAGE_RESOURCE_IDS[month_value], GPoint(-30, 30));	
#endif		

}


static void update_display(struct tm *current_time, bool force_update_a) {

  if (current_time->tm_sec == 0 || force_update_a) {
  	// the hours and minutes
    update_time(current_time);

    if ((current_time->tm_hour == 0 && current_time->tm_min == 0) || force_update_a) {
  		// the day and date
      update_date(current_time->tm_mday, current_time->tm_wday);

	  if (current_time->tm_mday == 1 || force_update_a) {
    	update_month(current_time->tm_mon);


    	}
      }
  	}
  }


static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_display(tick_time, false);
}


static void init(void) {
  memset(&time_digits_layers, 0, sizeof(time_digits_layers));
  memset(&time_digits_images, 0, sizeof(time_digits_images));
	
  memset(&date_digits_layers, 0, sizeof(date_digits_layers));
  memset(&date_digits_images, 0, sizeof(date_digits_images));
	
  window = window_create();
  if (window == NULL) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "OOM: couldn't allocate window");
      return;
  }
	
  window_set_background_color(window, GColorBlack);
  window_stack_push(window, true /* Animated */);
  Layer *window_layer = window_get_root_layer(window);

	moody_font = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_ROBOTOCONDENSED_34 ) );
	moody_font2 = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_ROBOTOCONDENSED_26 ) );

  dark8_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DARK8);
  GRect dark8bounds = gbitmap_get_bounds(dark8_icon);		
#ifdef PBL_PLATFORM_CHALK
  GRect dark8frame = GRect(20, 29, dark8bounds.size.w, dark8bounds.size.h);
#else
  GRect dark8frame = GRect(0, 59, dark8bounds.size.w, dark8bounds.size.h);
#endif	
  dark8_layer = bitmap_layer_create(dark8frame);
  bitmap_layer_set_bitmap(dark8_layer, dark8_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(dark8_layer));
	if (clock_is_24h_style()) {
		layer_set_hidden(bitmap_layer_get_layer(dark8_layer), true);
	}
	
  footprint_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FOOTPRINT);
  GRect footprintbounds = gbitmap_get_bounds(footprint_icon);		
#ifdef PBL_PLATFORM_CHALK
  GRect footprintframe = GRect(0, 93, footprintbounds.size.w, footprintbounds.size.h);
#else
  GRect footprintframe = GRect(112, 102, footprintbounds.size.w, footprintbounds.size.h);
#endif	
  footprint_layer = bitmap_layer_create(footprintframe);
  bitmap_layer_set_bitmap(footprint_layer, footprint_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(footprint_layer));
	
	
  steps_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(0, 65, 87, 40),
    GRect(0, 94, 110, 40)));
  text_layer_set_background_color(steps_label, GColorClear);
  text_layer_set_text_color(steps_label, GColorSunsetOrange  );
  text_layer_set_text_alignment(steps_label, GTextAlignmentRight);
#ifdef PBL_PLATFORM_CHALK
  text_layer_set_font(steps_label, moody_font2);
#else
  text_layer_set_font(steps_label, moody_font);
#endif
  layer_add_child(window_layer, text_layer_get_layer(steps_label));

  dist_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(94, 65, 80, 40),
    GRect(0, 124, 110, 40)));
  text_layer_set_background_color(dist_label, GColorClear);
  text_layer_set_text_color(dist_label, GColorRajah );
#ifdef PBL_PLATFORM_CHALK
  text_layer_set_text_alignment(dist_label, GTextAlignmentLeft);
  text_layer_set_font(dist_label, moody_font2);
#else
  text_layer_set_font(dist_label, moody_font);
  text_layer_set_text_alignment(dist_label, GTextAlignmentRight);
#endif
  layer_add_child(window_layer, text_layer_get_layer(dist_label));	
	

  // Create time and date layers
  GRect dummy_frame = { {0, 0}, {0, 0} };
	
  day_name_layer = bitmap_layer_create(dummy_frame);
  layer_add_child(window_layer, bitmap_layer_get_layer(day_name_layer));
	
  month_layer = bitmap_layer_create(dummy_frame);
  layer_add_child(window_layer, bitmap_layer_get_layer(month_layer));	
	
  for (int i = 0; i < TOTAL_TIME_DIGITS; ++i) {
    time_digits_layers[i] = bitmap_layer_create(dummy_frame);
    layer_add_child(window_layer, bitmap_layer_get_layer(time_digits_layers[i]));
  }
	
  for (int i = 0; i < TOTAL_DATE_DIGITS; ++i) {
    date_digits_layers[i] = bitmap_layer_create(dummy_frame);
    layer_add_child(window_layer, bitmap_layer_get_layer(date_digits_layers[i]));
  }
	
  separator_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SEPARATOR);
  GRect bitmap_bounds = gbitmap_get_bounds(separator_image);
#ifdef PBL_PLATFORM_CHALK
  GRect frame = GRect(81, 33, bitmap_bounds.size.w, bitmap_bounds.size.h);
#else
  GRect frame = GRect(76, 60, bitmap_bounds.size.w, bitmap_bounds.size.h);
#endif
  separator_layer = bitmap_layer_create(frame);
  bitmap_layer_set_bitmap(separator_layer, separator_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(separator_layer));   
  layer_set_hidden(bitmap_layer_get_layer(separator_layer), false);

	
  if (!clock_is_24h_style()) {
	 
#ifdef PBL_PLATFORM_CHALK
  GRect frame2 = GRect(81, 33, bitmap_bounds.size.w, bitmap_bounds.size.h);
#else
  GRect frame2 = GRect(60, 60, bitmap_bounds.size.w, bitmap_bounds.size.h);
#endif
  separator_layer2 = bitmap_layer_create(frame2);
  bitmap_layer_set_bitmap(separator_layer2, separator_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(separator_layer2));   
  layer_set_hidden(bitmap_layer_get_layer(separator_layer), true);
 }

 #ifdef PBL_PLATFORM_CHALK
  GRect frame5 = GRect(81, 33, bitmap_bounds.size.w, bitmap_bounds.size.h);
#else
  GRect frame5 = GRect(144, 60, 1, 1);
#endif
  time_format_layer = bitmap_layer_create(frame5);
	
  if (clock_is_24h_style()) {    
    time_format_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_24_HOUR_MODE);
    bitmap_layer_set_bitmap(time_format_layer, time_format_image); 
  }
  layer_add_child(window_layer, bitmap_layer_get_layer(time_format_layer));
	
	
  // Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  update_display(tick_time, true);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);
  bluetooth_connection_service_subscribe(&handle_bluetooth);

	  // subscribe to health events
  if(health_service_events_subscribe(health_handler, NULL)) {
    // force initial steps display
    health_handler(HealthEventMovementUpdate, NULL);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }	
}

static void deinit(void) {
 
  bluetooth_connection_service_unsubscribe();
  tick_timer_service_unsubscribe();
  health_service_events_unsubscribe();

  text_layer_destroy( dist_label );
  text_layer_destroy( steps_label );

  layer_remove_from_parent(bitmap_layer_get_layer(time_format_layer));
  bitmap_layer_destroy(time_format_layer);
  gbitmap_destroy(time_format_image);

  layer_remove_from_parent(bitmap_layer_get_layer(separator_layer));
  layer_remove_from_parent(bitmap_layer_get_layer(separator_layer2));
  bitmap_layer_destroy(separator_layer);
  bitmap_layer_destroy(separator_layer2);
  gbitmap_destroy(separator_image);

  layer_remove_from_parent(bitmap_layer_get_layer(day_name_layer));
  bitmap_layer_destroy(day_name_layer);
  gbitmap_destroy(day_name_image);

  layer_remove_from_parent(bitmap_layer_get_layer(month_layer));
  bitmap_layer_destroy(month_layer);
  gbitmap_destroy(month_image);
	
  layer_remove_from_parent(bitmap_layer_get_layer(footprint_layer));
  bitmap_layer_destroy(footprint_layer);
  gbitmap_destroy(footprint_icon);

  layer_remove_from_parent(bitmap_layer_get_layer(dark8_layer));
  bitmap_layer_destroy(dark8_layer);
  gbitmap_destroy(dark8_icon);
	
	for (int i = 0; i < TOTAL_DATE_DIGITS; i++) {
    layer_remove_from_parent(bitmap_layer_get_layer(date_digits_layers[i]));
    gbitmap_destroy(date_digits_images[i]);
    bitmap_layer_destroy(date_digits_layers[i]);
  }

  for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
    layer_remove_from_parent(bitmap_layer_get_layer(time_digits_layers[i]));
    gbitmap_destroy(time_digits_images[i]);
    bitmap_layer_destroy(time_digits_layers[i]);
  }
		
  fonts_unload_custom_font( moody_font );
  fonts_unload_custom_font( moody_font2 );

  layer_remove_from_parent(window_layer);
  layer_destroy(window_layer);
	
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

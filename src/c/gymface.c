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

#include <pebble.h>
#include "main.h"
#include "pebble-effect-layer.h"

EffectLayer* effect_layer_invert;

static Window *window;
static Layer *window_layer;

static GBitmap *separator_image;
static BitmapLayer *separator_layer;
static BitmapLayer *separator_layer2;

static GBitmap *time_format_image;
static BitmapLayer *time_format_layer;

static GBitmap *bg_image;
static BitmapLayer *bg_layer;

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

static TextLayer *steps_label, *sleep_label, *hr_label, *dist_label, *distm_label, *distft_label, *calories_label, *normcalories_label;

static GBitmap *footprint_icon;
static BitmapLayer *footprint_layer;

static GBitmap *burn_icon;
static BitmapLayer *burn_layer;

static GBitmap *sleep_icon;
static BitmapLayer *sleep_layer;

static GBitmap *heart_icon;
static BitmapLayer *heart_layer;

static GBitmap *dark8_icon;
static BitmapLayer *dark8_layer;

BitmapLayer *layer_batt_img;

GBitmap *img_battery_100;
GBitmap *img_battery_90;
GBitmap *img_battery_80;
GBitmap *img_battery_70;
GBitmap *img_battery_60;
GBitmap *img_battery_50;
GBitmap *img_battery_40;
GBitmap *img_battery_30;
GBitmap *img_battery_20;
GBitmap *img_battery_10;
GBitmap *img_battery_charge;

int charge_percent = 0;

// A struct for our specific settings (see main.h)
ClaySettings settings;



// Initialize the default settings
static void prv_default_settings() {
	
  settings.heart = false;
  settings.invert = false;
  settings.bluetoothvibe = false;
  settings.hourlyvibe = false;
}


static void health_handler(HealthEventType event, void *context) {
  static char s_value_buffer[8];
  static char s_value_buffer2[8];
  static char s_value_buffer2a[32];
  static char s_value_buffer2a2[32];
  static char s_value_buffer2b[32];
  static char s_value_buffer3[8];
  static char s_value_buffer4[32];
  static char s_value_buffer5[32];
  static char s_value_buffer6[8];
  static char s_hrm_buffer[8];
	
const HealthMetric metric = HealthMetricWalkedDistanceMeters;
const HealthValue distance = health_service_sum_today(metric);
const HealthMetric sleepmetric = HealthMetricSleepSeconds;
const HealthValue sleep = health_service_sum_today(sleepmetric);
	
	  /** Display the Heart Rate **/
  HealthValue hrmValue = health_service_peek_current_value(HealthMetricHeartRateBPM);
	
// Get the preferred measurement system
MeasurementSystem system = health_service_get_measurement_system_for_display(metric);

	
  if (event == HealthEventMovementUpdate) {
    // display the step count
    snprintf(s_value_buffer, sizeof(s_value_buffer), "%d", (int)health_service_sum_today(HealthMetricStepCount));
    snprintf(s_value_buffer3, sizeof(s_value_buffer3), "%d", (int)health_service_sum_today(HealthMetricActiveKCalories ));
    snprintf(s_value_buffer4, sizeof(s_value_buffer4), "%d cal", (int)health_service_sum_today(HealthMetricRestingKCalories ));
	    
	  
	  int mysleep = ((sleep % 86400) / 3600);
	  int mysleepdec = (int)((sleep % 3600) / 60);
   snprintf(s_value_buffer5, sizeof(s_value_buffer5), "%dh %dm", mysleep,mysleepdec);
	  
	 	  
 //   snprintf(s_value_buffer6, sizeof(s_value_buffer6), "%d", (int)health_service_sum_today(HealthMetricHeartRateBPM  ));



  snprintf(s_hrm_buffer, sizeof(s_hrm_buffer), "%lu", (uint32_t) hrmValue);
	  
  text_layer_set_text(hr_label, s_hrm_buffer);	
	  
	  
	  
    text_layer_set_text(steps_label, s_value_buffer);
    text_layer_set_text(sleep_label, s_value_buffer5);
 //   text_layer_set_text(hr_label, s_value_buffer6);
    text_layer_set_text(dist_label, s_value_buffer2);
    text_layer_set_text(calories_label, s_value_buffer3);
    text_layer_set_text(normcalories_label, s_value_buffer4);	  
	  
	  // Format accordingly
switch(system) {
  case MeasurementSystemMetric: {
	
		    // Convert to km
    int km = (int)((float)distance / 1000);
    int km2 = (int)((float)distance / 100);
	
    snprintf(s_value_buffer2a, sizeof(s_value_buffer2a), "%d m", (int)distance);    
    snprintf(s_value_buffer2b, sizeof(s_value_buffer2b), "%d.%d km", (int)km,(int)km2);    

  } 
	break;
	
  case MeasurementSystemImperial: {
    // Convert to imperial first
    int feet = (int)((float)distance * 3.28F);
	  
  uint16_t current_dist = 0;
    uint16_t current_dist_int = 0;
    uint16_t current_dist_dec = 0;
	  
		current_dist = (int)health_service_sum_today(metric);
		current_dist /= 1.6;
        
        current_dist_int = current_dist/1000;
        current_dist_dec = (current_dist%1000)/100;
	  
    snprintf(s_value_buffer2a2, sizeof(s_value_buffer2a2), "%d ft", (int)feet);
    snprintf(s_value_buffer2b, sizeof(s_value_buffer2b), "%d.%d mi", current_dist_int, current_dist_dec);
	  
  } 
	break;
  case MeasurementSystemUnknown:
  default:
    APP_LOG(APP_LOG_LEVEL_INFO, "unknown");
}

// Display to user in correct units
    text_layer_set_text(distm_label, s_value_buffer2a);
    text_layer_set_text(distft_label, s_value_buffer2a2);
    text_layer_set_text(dist_label, s_value_buffer2b);
	 
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

void handle_battery(BatteryChargeState charge_state) {

    if (charge_state.is_charging) {
        bitmap_layer_set_bitmap(layer_batt_img, img_battery_charge);

    } else {
        if (charge_state.charge_percent <= 10) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_10);
        } else if (charge_state.charge_percent <= 20) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_20);
		} else if (charge_state.charge_percent <= 30) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_30);
		} else if (charge_state.charge_percent <= 40) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_40);
		} else if (charge_state.charge_percent <= 50) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_50);
        } else if (charge_state.charge_percent <= 60) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_60);
		} else if (charge_state.charge_percent <= 70) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_70);
		} else if (charge_state.charge_percent <= 80) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_80);
		} else if (charge_state.charge_percent <= 90) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_90);
		} else if (charge_state.charge_percent <= 99) {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
		} else {
            bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
        }      						
    }
    charge_percent = charge_state.charge_percent;
}


void handle_bluetooth(bool connected) {
  if(!connected && settings.bluetoothvibe) {
    //vibe!
    vibes_long_pulse(); 
    layer_set_hidden(bitmap_layer_get_layer(day_name_layer), connected);
  }
    layer_set_hidden(bitmap_layer_get_layer(day_name_layer), !connected);
}

void force_update(void) {
    handle_bluetooth(bluetooth_connection_service_peek());
    handle_battery(battery_state_service_peek());
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
	int yPos = 20;
#endif	
	
#ifdef PBL_PLATFORM_CHALK
  set_container_image(&time_digits_images[0], time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(30, yPos));
  set_container_image(&time_digits_images[1], time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(54, yPos));

  set_container_image(&time_digits_images[2], time_digits_layers[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(94, yPos));
  set_container_image(&time_digits_images[3], time_digits_layers[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(121, yPos));
#else
  set_container_image(&time_digits_images[0], time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(24, yPos));
  set_container_image(&time_digits_images[1], time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(50, yPos));

  set_container_image(&time_digits_images[2], time_digits_layers[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(90, yPos));
  set_container_image(&time_digits_images[3], time_digits_layers[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(115, yPos));
#endif		

	
  if (!clock_is_24h_style()) {
 
#ifdef PBL_PLATFORM_CHALK
  set_container_image(&time_digits_images[0], time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(25, yPos));
  set_container_image(&time_digits_images[1], time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(54, yPos));

  set_container_image(&time_digits_images[2], time_digits_layers[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(92, yPos));
  set_container_image(&time_digits_images[3], time_digits_layers[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(117, yPos));
#else
 set_container_image(&time_digits_images[0], time_digits_layers[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(15, yPos));
  set_container_image(&time_digits_images[1], time_digits_layers[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(42, yPos));

  set_container_image(&time_digits_images[2], time_digits_layers[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(76, yPos));
  set_container_image(&time_digits_images[3], time_digits_layers[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(102, yPos));
#endif		

	  if (current_time->tm_hour >= 12) {
		  
#ifdef PBL_PLATFORM_CHALK
      set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_PM_MODE, GPoint(144, 46));
      layer_set_hidden(bitmap_layer_get_layer(time_format_layer), false);	

#else
      set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_PM_MODE, GPoint(130, 23));
      layer_set_hidden(bitmap_layer_get_layer(time_format_layer), false);
#endif		
    } 
    else {
	  
#ifdef PBL_PLATFORM_CHALK
	  set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_AM_MODE, GPoint(144, 46));
      layer_set_hidden(bitmap_layer_get_layer(time_format_layer), false);		
		
#else
	  set_container_image(&time_format_image, time_format_layer, RESOURCE_ID_IMAGE_AM_MODE, GPoint(130, 23));
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
	
  if(settings.hourlyvibe) {
    //vibe!
    vibes_short_pulse();
  }

#ifdef PBL_PLATFORM_CHALK
  set_container_image(&day_name_image, day_name_layer, DAY_NAME_IMAGE_RESOURCE_IDS[days_since_sunday], GPoint(10, 64));
  set_container_image(&date_digits_images[0], date_digits_layers[0], DATENUM_IMAGE_RESOURCE_IDS[day_of_month/10], GPoint(107, 64));
  set_container_image(&date_digits_images[1], date_digits_layers[1], DATENUM_IMAGE_RESOURCE_IDS[day_of_month%10], GPoint(116, 64));
#else
  set_container_image(&day_name_image, day_name_layer, DAY_NAME_IMAGE_RESOURCE_IDS[days_since_sunday], GPoint(0, 2));
  set_container_image(&date_digits_images[0], date_digits_layers[0], DATENUM_IMAGE_RESOURCE_IDS[day_of_month/10], GPoint(95, 2));
  set_container_image(&date_digits_images[1], date_digits_layers[1], DATENUM_IMAGE_RESOURCE_IDS[day_of_month%10], GPoint(104, 2));
#endif		

}

static void update_month (int months_since_january) {
  int month_value = months_since_january;
#ifdef PBL_PLATFORM_CHALK
  set_container_image(&month_image, month_layer, MONTH_IMAGE_RESOURCE_IDS[month_value], GPoint(134, 64));
#else
  set_container_image(&month_image, month_layer, MONTH_IMAGE_RESOURCE_IDS[month_value], GPoint(117, 2));	
#endif		

}

static void update_display(struct tm *current_time, bool force_update) {

  if (current_time->tm_sec == 0 || force_update) {
  	// the hours and minutes
    update_time(current_time);

    if ((current_time->tm_hour == 0 && current_time->tm_min == 0) || force_update) {
  		// the day and date
      update_date(current_time->tm_mday, current_time->tm_wday);

	  if (current_time->tm_mday == 1 || force_update) {
    	update_month(current_time->tm_mon);

    	}
      }
  	}
  }

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_display(tick_time, false);
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {
	  

  // invert
  if (settings.invert && effect_layer_invert == NULL) {
    // Add inverter layer
    Layer *window_layer = window_get_root_layer(window);

#ifdef PBL_PLATFORM_CHALK
	  effect_layer_invert = effect_layer_create(GRect(0, 79, 180, 101));
#else
	  effect_layer_invert = effect_layer_create(GRect(0, 61, 144, 107));
#endif
	  effect_layer_add_effect(effect_layer_invert, effect_invert_bw_only, NULL);
    layer_add_child(window_layer, effect_layer_get_layer(effect_layer_invert));
  
  } else if (!settings.invert && effect_layer_invert != NULL) {
    // Remove Inverter layer
   layer_remove_from_parent(effect_layer_get_layer(effect_layer_invert));
   effect_layer_destroy(effect_layer_invert);
   effect_layer_invert = NULL;
  }	

if (settings.heart) {
		layer_set_hidden(text_layer_get_layer(hr_label), false);
		layer_set_hidden(bitmap_layer_get_layer(heart_layer), false);
	} else {
		layer_set_hidden(text_layer_get_layer(hr_label), true);
		layer_set_hidden(bitmap_layer_get_layer(heart_layer), true);
	}	
  
	  // Refresh display

  // Get current time
  time_t now2 = time( NULL );
  struct tm *tick_time = localtime( &now2 );

  // Force update to Refresh display
  handle_tick(tick_time, MONTH_UNIT + DAY_UNIT + HOUR_UNIT + MINUTE_UNIT );

}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
	
  // Invert
  Tuple *invert_t = dict_find(iter, MESSAGE_KEY_invert);
  if (invert_t) {
    settings.invert = invert_t->value->int32 == 1;
  }
	
  // heart
  Tuple *heart_t = dict_find(iter, MESSAGE_KEY_heart);
  if (heart_t) {
    settings.heart = heart_t->value->int32 == 1;
  }

  // Bluetoothvibe
  Tuple *animations_t = dict_find(iter, MESSAGE_KEY_bluetoothvibe);
  if (animations_t) {
    settings.bluetoothvibe = animations_t->value->int32 == 1;
  }

  // hourlyvibe
  Tuple *hourlyvibe_t = dict_find(iter, MESSAGE_KEY_hourlyvibe);
  if (hourlyvibe_t) {
    settings.hourlyvibe = hourlyvibe_t->value->int32 == 1;
  }
	
  // Save the new settings to persistent storage
  prv_save_settings();
}


static void init(void) {
	
  prv_load_settings();
	
  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);
	
	
  memset(&time_digits_layers, 0, sizeof(time_digits_layers));
  memset(&time_digits_images, 0, sizeof(time_digits_images));
	
  memset(&date_digits_layers, 0, sizeof(date_digits_layers));
  memset(&date_digits_images, 0, sizeof(date_digits_images));
	
  const int inbound_size = 128;
  const int outbound_size = 128;
  app_message_open(inbound_size, outbound_size);  
	
  window = window_create();
  if (window == NULL) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "OOM: couldn't allocate window");
      return;
  }
	
  window_set_background_color(window, GColorBlack);
  window_stack_push(window, true /* Animated */);
  Layer *window_layer = window_get_root_layer(window);

  bg_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG);
  GRect bgbounds = gbitmap_get_bounds(bg_image);		
#ifdef PBL_PLATFORM_CHALK
  GRect bgframe = GRect(0, 78, bgbounds.size.w, bgbounds.size.h);
#else
  GRect bgframe = GRect(0, 60, bgbounds.size.w, bgbounds.size.h);
#endif	
  bg_layer = bitmap_layer_create(bgframe);
  bitmap_layer_set_bitmap(bg_layer, bg_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(bg_layer));
	
	
	
	moody_font = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_ROBOTOCONDENSED_14 ) );
	moody_font2 = fonts_load_custom_font( resource_get_handle( RESOURCE_ID_FONT_ROBOTOCONDENSED_20 ) );

  dark8_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DARK8);
  GRect dark8bounds = gbitmap_get_bounds(dark8_icon);		
#ifdef PBL_PLATFORM_CHALK
  GRect dark8frame = GRect(28, 29, dark8bounds.size.w, dark8bounds.size.h);
#else
  GRect dark8frame = GRect(15, 21, dark8bounds.size.w, dark8bounds.size.h);
#endif	
  dark8_layer = bitmap_layer_create(dark8frame);
  bitmap_layer_set_bitmap(dark8_layer, dark8_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(dark8_layer));
	if (clock_is_24h_style()) {
		layer_set_hidden(bitmap_layer_get_layer(dark8_layer), true);
	}
	
// icons	
	
  footprint_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FOOTPRINT);
  GRect footprintbounds = gbitmap_get_bounds(footprint_icon);		
#ifdef PBL_PLATFORM_CHALK
  GRect footprintframe = GRect(115, 150, footprintbounds.size.w, footprintbounds.size.h);
#else
  GRect footprintframe = GRect(124, 86, footprintbounds.size.w, footprintbounds.size.h);
#endif	
  footprint_layer = bitmap_layer_create(footprintframe);
  bitmap_layer_set_bitmap(footprint_layer, footprint_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(footprint_layer));
	
  burn_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BURN);
  GRect burnbounds = gbitmap_get_bounds(burn_icon);		
#ifdef PBL_PLATFORM_CHALK
  GRect burnframe = GRect(149, 106, burnbounds.size.w, burnbounds.size.h);
#else
  GRect burnframe = GRect(124, 108, burnbounds.size.w, burnbounds.size.h);
#endif	
  burn_layer = bitmap_layer_create(burnframe);
  bitmap_layer_set_bitmap(burn_layer, burn_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(burn_layer));

  sleep_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SLEEP);
  GRect sleepbounds = gbitmap_get_bounds(sleep_icon);		
#ifdef PBL_PLATFORM_CHALK
  GRect sleepframe = GRect(134, 130, sleepbounds.size.w, sleepbounds.size.h);
#else
  GRect sleepframe = GRect(123, 130, sleepbounds.size.w, sleepbounds.size.h);
#endif	
  sleep_layer = bitmap_layer_create(sleepframe);
  bitmap_layer_set_bitmap(sleep_layer, sleep_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(sleep_layer));	
	
  heart_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEART);
  GRect heartbounds = gbitmap_get_bounds(heart_icon);		
#ifdef PBL_PLATFORM_CHALK
  GRect heartframe = GRect(1, 1, heartbounds.size.w, heartbounds.size.h);
#else
  GRect heartframe = GRect(125, 150, heartbounds.size.w, heartbounds.size.h);
#endif	
  heart_layer = bitmap_layer_create(heartframe);
  bitmap_layer_set_bitmap(heart_layer, heart_icon);
  layer_add_child(window_layer, bitmap_layer_get_layer(heart_layer));	


	
// text labels

  dist_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(84, 79, 86, 23),
    GRect(0, 61, 141, 23)));
  text_layer_set_background_color(dist_label, GColorClear);
  text_layer_set_text_color(dist_label, GColorWhite  );
  text_layer_set_font(dist_label, moody_font2);
  text_layer_set_text_alignment(dist_label, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(dist_label));	
	
  distm_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(0, 84, 71, 23),
    GRect(0, 67, 61, 23)));
  text_layer_set_background_color(distm_label, GColorClear);
  text_layer_set_text_color(distm_label, GColorWhite  );
  text_layer_set_font(distm_label, moody_font);
  text_layer_set_text_alignment(distm_label, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(distm_label));	
	
  distft_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(0, 85, 71, 23),
    GRect(0, 67, 61, 23)));
  text_layer_set_background_color(distft_label, GColorClear);
  text_layer_set_text_color(distft_label, GColorWhite  );
#ifdef PBL_PLATFORM_CHALK
  text_layer_set_text_alignment(distft_label, GTextAlignmentRight);
  text_layer_set_font(distft_label, moody_font);
#else
  text_layer_set_font(distft_label, moody_font);
  text_layer_set_text_alignment(distft_label, GTextAlignmentRight);
#endif
  layer_add_child(window_layer, text_layer_get_layer(distft_label));	
	
  calories_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(0, 102, 142, 23),
    GRect(0, 104, 117, 23)));
  text_layer_set_text_color(calories_label, GColorWhite  );
  text_layer_set_font(calories_label, moody_font2);
  text_layer_set_background_color(calories_label, GColorClear);
  text_layer_set_text_alignment(calories_label, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(calories_label));	
	
  normcalories_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(0, 108, 73, 23),
    GRect(0, 109, 51, 23)));
  text_layer_set_font(normcalories_label, moody_font);
  text_layer_set_background_color(normcalories_label, GColorClear);
  text_layer_set_text_color(normcalories_label, GColorWhite   );
  text_layer_set_text_alignment(normcalories_label, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(normcalories_label));	
	
  steps_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(0, 147, 109, 23),
    GRect(0, 82, 117, 23)));
  text_layer_set_background_color(steps_label, GColorClear);
  text_layer_set_text_color(steps_label, GColorWhite   );
  text_layer_set_text_alignment(steps_label, GTextAlignmentRight);
  text_layer_set_font(steps_label, moody_font2);
  layer_add_child(window_layer, text_layer_get_layer(steps_label));

  sleep_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(0, 125, 120, 23),
    GRect(0, 125,117, 23)));
  text_layer_set_background_color(sleep_label, GColorClear);
  text_layer_set_text_color(sleep_label, GColorWhite   );
  text_layer_set_text_alignment(sleep_label, GTextAlignmentRight);
  text_layer_set_font(sleep_label, moody_font2);
  layer_add_child(window_layer, text_layer_get_layer(sleep_label));
	
  hr_label = text_layer_create(PBL_IF_ROUND_ELSE(
    GRect(1, 1, 1, 1),
    GRect(0, 145, 117, 23)));
  text_layer_set_background_color(hr_label, GColorClear);
  text_layer_set_text_color(hr_label, GColorWhite   );
  text_layer_set_text_alignment(hr_label, GTextAlignmentRight);
  text_layer_set_font(hr_label, moody_font2);
  layer_add_child(window_layer, text_layer_get_layer(hr_label));
	
	
    img_battery_100   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_100);
    img_battery_90   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_90);
    img_battery_80   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_80);
    img_battery_70   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_70);
    img_battery_60   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_60);
    img_battery_50   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_50);
    img_battery_40   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_40);
    img_battery_30   = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_30);
    img_battery_20    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_20);
    img_battery_10    = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_10);
    img_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_CHARGING);
	
#ifdef PBL_PLATFORM_CHALK
  layer_batt_img  = bitmap_layer_create(GRect(35, 3, 110, 23));
  bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_batt_img));
#else
  layer_batt_img  = bitmap_layer_create(GRect(0, 56, 144, 4));
  bitmap_layer_set_bitmap(layer_batt_img, img_battery_100);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_batt_img));	
#endif

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
  GRect frame = GRect(83, 28, bitmap_bounds.size.w, bitmap_bounds.size.h);
#else
  GRect frame = GRect(79, 19, bitmap_bounds.size.w, bitmap_bounds.size.h);
#endif
  separator_layer = bitmap_layer_create(frame);
  bitmap_layer_set_bitmap(separator_layer, separator_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(separator_layer));   
  layer_set_hidden(bitmap_layer_get_layer(separator_layer), false);
	
  if (!clock_is_24h_style()) {
	 
#ifdef PBL_PLATFORM_CHALK
  GRect frame2 = GRect(81, 28, bitmap_bounds.size.w, bitmap_bounds.size.h);
#else
  GRect frame2 = GRect(68, 19, bitmap_bounds.size.w, bitmap_bounds.size.h);
#endif
  separator_layer2 = bitmap_layer_create(frame2);
  bitmap_layer_set_bitmap(separator_layer2, separator_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(separator_layer2));   
  layer_set_hidden(bitmap_layer_get_layer(separator_layer), true);
 }

 #ifdef PBL_PLATFORM_CHALK
  GRect frame5 = GRect(81, 33, bitmap_bounds.size.w, bitmap_bounds.size.h);
#else
  GRect frame5 = GRect(144, 19, 1, 1);
#endif
  time_format_layer = bitmap_layer_create(frame5);
	
  if (clock_is_24h_style()) {    
    time_format_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_24_HOUR_MODE);
    bitmap_layer_set_bitmap(time_format_layer, time_format_image); 
  }
  layer_add_child(window_layer, bitmap_layer_get_layer(time_format_layer));
	
  force_update();

  prv_update_display();


	
	// Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  update_display(tick_time, true);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
    battery_state_service_subscribe(&handle_battery);

	  // subscribe to health events
  if(health_service_events_subscribe(health_handler, NULL)) {
    // force initial steps display
    health_handler(HealthEventMovementUpdate, NULL);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
  }	
}

static void deinit(void) {
 
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  tick_timer_service_unsubscribe();
  health_service_events_unsubscribe();

  text_layer_destroy( hr_label );
  text_layer_destroy( distft_label );
  text_layer_destroy( distm_label );
  text_layer_destroy( dist_label );
  text_layer_destroy( normcalories_label );
  text_layer_destroy( sleep_label );
  text_layer_destroy( steps_label );
  text_layer_destroy( calories_label );

  layer_remove_from_parent(bitmap_layer_get_layer(time_format_layer));
  bitmap_layer_destroy(time_format_layer);
  gbitmap_destroy(time_format_image);

  layer_remove_from_parent(bitmap_layer_get_layer(bg_layer));
  bitmap_layer_destroy(bg_layer);
  gbitmap_destroy(bg_image);
	
  layer_remove_from_parent(bitmap_layer_get_layer(heart_layer));
  bitmap_layer_destroy(heart_layer);
  gbitmap_destroy(heart_icon);
	
  layer_remove_from_parent(bitmap_layer_get_layer(sleep_layer));
  bitmap_layer_destroy(sleep_layer);
  gbitmap_destroy(sleep_icon);
	
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

  layer_remove_from_parent(bitmap_layer_get_layer(burn_layer));
  bitmap_layer_destroy(burn_layer);
  gbitmap_destroy(burn_icon);

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
		
  layer_remove_from_parent(bitmap_layer_get_layer(layer_batt_img));
  bitmap_layer_destroy(layer_batt_img);
	
  gbitmap_destroy(img_battery_100);
  gbitmap_destroy(img_battery_90);
  gbitmap_destroy(img_battery_80);
  gbitmap_destroy(img_battery_70);
  gbitmap_destroy(img_battery_60);
  gbitmap_destroy(img_battery_50);
  gbitmap_destroy(img_battery_40);
  gbitmap_destroy(img_battery_30);
  gbitmap_destroy(img_battery_20);
  gbitmap_destroy(img_battery_10);
  gbitmap_destroy(img_battery_charge);	
	
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

#include "pebble.h"

static Window *window;

static GBitmap *background_image;
static BitmapLayer *background_layer;

const int DAY_PART_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE00,
  RESOURCE_ID_IMAGE01,
  RESOURCE_ID_IMAGE02,
  RESOURCE_ID_IMAGE03,
  RESOURCE_ID_IMAGE04,
  RESOURCE_ID_IMAGE05,
  RESOURCE_ID_IMAGE06,
  RESOURCE_ID_IMAGE07,
  RESOURCE_ID_IMAGE08,
  RESOURCE_ID_IMAGE09,
  RESOURCE_ID_IMAGE0A,
  RESOURCE_ID_IMAGE0B,
  RESOURCE_ID_IMAGE0C,
  RESOURCE_ID_IMAGE0D,
  RESOURCE_ID_IMAGE0E,  
  RESOURCE_ID_IMAGE0F
};

static GBitmap *day_part_image;
static BitmapLayer *day_part_layer;


static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
  GBitmap *old_image = *bmp_image;

  *bmp_image = gbitmap_create_with_resource(resource_id);
  GRect frame = (GRect) {
    .origin = origin,
    .size = (*bmp_image)->bounds.size
  };
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

  if (old_image != NULL) {
  	gbitmap_destroy(old_image);
  }
}


static void update_display(struct tm *current_time) {
  unsigned short daypart = (60*(current_time->tm_hour) + current_time->tm_min)/90;

  //set_container_image(&day_name_image, day_name_layer, DAY_NAME_IMAGE_RESOURCE_IDS[current_time->tm_wday], GPoint(69, 61));
  set_container_image(&day_part_image, day_part_layer, DAY_PART_IMAGE_RESOURCE_IDS[daypart], GPoint(16, 20));

}


static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_display(tick_time);
}


static void init(void) {
  window = window_create();
  if (window == NULL) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "OOM: couldn't allocate window");
      return;
  }
  window_stack_push(window, true /* Animated */);
  Layer *window_layer = window_get_root_layer(window);

  background_image = gbitmap_create_with_resource(RESOURCE_ID_BLACK);
  background_layer = bitmap_layer_create(layer_get_frame(window_layer));
  bitmap_layer_set_bitmap(background_layer, background_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

 // Create time and date layers
  GRect dummy_frame = { {0, 0}, {0, 0} };
  day_part_layer = bitmap_layer_create(dummy_frame);
  layer_add_child(window_layer, bitmap_layer_get_layer(day_part_layer));  
  
  // Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);

  update_display(tick_time);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}


static void deinit(void) {
  //layer_remove_from_parent(bitmap_layer_get_layer(background_layer));
  //bitmap_layer_destroy(background_layer);
  //gbitmap_destroy(background_image);

  layer_remove_from_parent(bitmap_layer_get_layer(day_part_layer));
  bitmap_layer_destroy(day_part_layer);
  gbitmap_destroy(day_part_image);

}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

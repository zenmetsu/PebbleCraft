#include "pebble.h"

typedef struct GCompOpInfo {
  char *name;
  GCompOp op;
} GCompOpInfo;

// These are the compositing modes available
static GCompOpInfo gcompops[] = {
  {"GCompOpAssign", GCompOpAssign},
  {"GCompOpAssignInverted", GCompOpAssignInverted},
  {"GCompOpOr", GCompOpOr},
  {"GCompOpAnd", GCompOpAnd},
  {"GCompOpClear", GCompOpClear},
  {"GCompOpSet", GCompOpSet}
};

static const int num_gcompops = ARRAY_LENGTH(gcompops);

static int current_gcompop = 0;

static Window *window;

static Layer *layer;

// We will use a bitmap to composite with a large circle
static GBitmap *image;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  uint8_t animFrame= 0;

  animFrame = (60*(tick_time->tm_hour) + tick_time->tm_min)/90;
  switch (animFrame) {
    case 0: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE00);
      break;
    case 1: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE01);
      break; 
    case 2: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE02);
      break; 
    case 3: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE03);
      break; 
    case 4: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE04);
      break; 
    case 5: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE05);
      break; 
    case 6: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE06);
      break; 
    case 7: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE07);
      break; 
    case 8: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE08);
      break; 
    case 9: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE09);
      break; 
    case 10: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE0A);
      break; 
    case 11: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE0B);
      break; 
    case 12: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE0C);
      break; 
    case 13: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE0D);
      break; 
    case 14: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE0E);
      break; 
    case 15: 
      image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE0F);
      break;                                                        
  }  
  layer_mark_dirty(layer);
}

// This is a layer update callback where compositing will take place
static void layer_update_callback(Layer *layer, GContext* ctx) {
  GRect bounds = layer_get_frame(layer);

  // Draw the large circle the image will composite with
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 0, 0);

  // Use the image size to help center the image
  GRect destination = image->bounds;

  // Center horizontally using the window frame size
  destination.origin.x = (bounds.size.w-destination.size.w)/2;
  destination.origin.y = (bounds.size.h-destination.size.h)/2;

  // Set the current compositing operation
  // This will only cause bitmaps to composite
  graphics_context_set_compositing_mode(ctx, gcompops[current_gcompop].op);

  // Draw the bitmap; it will use current compositing operation set
  graphics_draw_bitmap_in_rect(ctx, image, destination);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (--current_gcompop < 0) {
    current_gcompop = num_gcompops-1;
  }
  layer_mark_dirty(layer);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (++current_gcompop >= num_gcompops) {
    current_gcompop = 0;
  }
  layer_mark_dirty(layer);
}

static void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

int main(void) {
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Then use the respective resource loader to obtain the resource for use
  // In this case, we load the image
  image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE00);

  window = window_create();
  window_stack_push(window, true /* Animated */);

  window_set_click_config_provider(window, config_provider);

  // Initialize the layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer = layer_create(bounds);

  // Set up the update layer callback
  layer_set_update_proc(layer, layer_update_callback);

  // Add the layer to the window for display
  layer_add_child(window_layer, layer);

  update_time();
  // Enter the main loop
  app_event_loop();

  // Cleanup the image
  gbitmap_destroy(image);

  layer_destroy(layer);
  window_destroy(window);
}



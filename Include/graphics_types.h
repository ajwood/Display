#ifndef  DEF_GRAPHICS_TYPES
#define  DEF_GRAPHICS_TYPES

#include  <def_standard.h>
#include  <def_objects.h>
#include  <def_queue.h>
#include  <def_stack.h>
#include  <def_colours.h>
#include  <def_graphics_dependent.h>

typedef  struct
{
    Boolean     perspective_flag;
    Point       origin;
    Vector      x_axis, y_axis, line_of_sight;
    Real        front_distance, back_distance;
    Real        perspective_distance;
    Real        desired_aspect;
    Real        window_width, window_height;
    Real        scale_factors[N_DIMENSIONS];
    Transform   modeling_transform;
} view_struct;

typedef enum { AMBIENT_LIGHT,
               DIRECTIONAL_LIGHT,
               POINT_LIGHT,
               SPOT_LIGHT
             } light_types;

typedef  struct
{
    Boolean       state;
    light_types   light_type;
    Colour        colour;
    Vector        direction;
    Point         position;
    Real          spot_exponent;
    Real          spot_angle;
} light_struct;

typedef  struct
{
    Colour         colour_below;

    Colour         min_colour;
    Real           min_value;

    Colour         max_colour;
    Real           max_value;

    Colour         colour_above;

    Real           size_over_range;

    Pixel_colour   colour_below_pixel;
    Pixel_colour   colour_above_pixel;

    Colour_spaces  interpolation_space;
    int            colour_table_size;
    Pixel_colour   *colour_table;
} colour_coding_struct;

typedef  enum  {
                   NO_EVENT,
                   TERMINATE_EVENT,
                   KEYBOARD_EVENT,
                   MOUSE_MOVEMENT_EVENT,
                   LEFT_MOUSE_DOWN_EVENT,
                   LEFT_MOUSE_UP_EVENT,
                   MIDDLE_MOUSE_DOWN_EVENT,
                   MIDDLE_MOUSE_UP_EVENT,
                   RIGHT_MOUSE_DOWN_EVENT,
                   RIGHT_MOUSE_UP_EVENT,
                   WINDOW_REDRAW_EVENT,
                   WINDOW_RESIZE_EVENT,
                   NUM_EVENT_TYPES
               } event_types;

typedef  struct
{
    event_types   event_type;
    Window_id     window_id;

    union
    {
        int    key_pressed;
        Point  mouse_position;
    } event_data;

} event_struct;

typedef  QUEUE_STRUCT( event_struct )   event_queue_struct;

typedef  Status  event_function_decl();

typedef  Status  (*event_function_type)();

#define  DECL_EVENT_FUNCTION( f )  Status   f()

#define  DEF_EVENT_FUNCTION( f )   Status   f( graphics, event ) \
                                                graphics_struct  *graphics; \
                                                event_struct     *event;

#define  MAX_ACTION_STACK  5
#define  MAX_ACTIONS       10

typedef  struct
{
    int                   stack_index;
    int                   last_index[MAX_ACTION_STACK];
    event_function_type   actions[MAX_ACTIONS];
} action_table_entry;

typedef  struct
{
    action_table_entry  event_info[(int) NUM_EVENT_TYPES];
} action_table_struct;

typedef  struct
{
    Boolean       line_active;
    Point         line_origin;
    Point         line_direction;
} point_position_struct;

typedef  struct
{
    int            object_index;
    object_struct  *model_object;
} selection_entry;

typedef  STACK_STRUCT( selection_entry )   selection_struct;

typedef  struct
{
    Boolean        last_was_interrupted;
    object_struct  *object_interrupted;
    int            n_items_done;
    int            next_item;
    Real           interrupt_at;

    int            size_of_interrupted;
    int            interval_of_check;

    Boolean        current_interrupted;
} update_interrupted_struct;

typedef  struct
{
    Point   first_corner;
    Point   second_corner;
} viewport_picking_struct;

typedef  struct
{
    Point   origin;
    Real    box_size[N_DIMENSIONS];
    Real    axis_size;
} cursor_struct;

typedef  struct
{
    int           x_size;
    int           y_size;
    int           n_steps;
    int           current_step;
    String        base_filename;
    Transform     transform;
    Pixel_colour  *image_storage;
} film_loop_struct;

typedef  struct
{
    polygons_struct  *polygons;
    Boolean          polygons_set;
    int              n_vertices;
    int              n_vertices_alloced;
    int              *vertices;

    int              n_undos;
    int              n_undos_alloced;
    int              *undo_indices;

    int              n_invisible;
    int              n_invisible_alloced;
    int              *invisible_indices;
} surface_edit_struct;

typedef  struct
{
    lines_struct  *lines;
    int           n_points_alloced;
    int           n_indices_alloced;
    int           n_end_indices_alloced;
} contour_struct;

typedef  struct
{
    contour_struct           contours[3];
    int                      models_changed_id;
    int                      axis;
    polygons_struct          *current_polygons;
    int                      poly_index;
    object_traverse_struct   object_traverse;
} cursor_contours_struct;

typedef  struct
{
    Boolean          picking_points;
    lines_struct     *lines;
    int              n_points_alloced;
    int              n_indices_alloced;
    int              n_end_indices_alloced;
    Boolean          prev_point_exists;
    Point            prev_point;
    int              prev_poly_index;
    polygons_struct  *prev_polygons;
} surface_curve_struct;

#endif

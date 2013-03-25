#ifndef  DEF_SLICE
#define  DEF_SLICE

/* ----------------------------------------------------------------------------
@COPYRIGHT  :
              Copyright 1993,1994,1995 David MacDonald,
              McConnell Brain Imaging Centre,
              Montreal Neurological Institute, McGill University.
              Permission to use, copy, modify, and distribute this
              software and its documentation for any purpose and without
              fee is hereby granted, provided that the above copyright
              notice appear in all copies.  The author and McGill University
              make no representations about the suitability of this
              software for any purpose.  It is provided "as is" without
              express or implied warranty.
---------------------------------------------------------------------------- */

#ifndef lint
static char display_slice_rcsid[] = "$Header: /private-cvsroot/visualization/Display/Include/slice.h,v 1.73 1998/02/20 15:00:00 david Exp $";
#endif

#include  <volume_io.h>
#include  <atlas.h>
#include  <display_types.h>

#define   N_SLICE_VIEWS   4

#define   OBLIQUE_VIEW_INDEX    (N_SLICE_VIEWS-1)

typedef enum {
               VOLUME_INDEX,
               X_VOXEL_PROBE_INDEX,
               Y_VOXEL_PROBE_INDEX,
               Z_VOXEL_PROBE_INDEX,
               X_WORLD_PROBE_INDEX,
               Y_WORLD_PROBE_INDEX,
               Z_WORLD_PROBE_INDEX,
               VOXEL_PROBE_INDEX,
               VAL_PROBE_INDEX,
               LABEL_PROBE_INDEX,
               RATIO_PROBE_INDEX,
               N_READOUT_MODELS     } Slice_readout_indices;

typedef  struct
{
    Real              min_threshold;
    Real              max_threshold;
    Neighbour_types   connectivity;
    int               n_starts_alloced;
    int               *y_starts;
    int               x_mouse_start, y_mouse_start;
    Real              mouse_scale_factor;
    VIO_BOOL           fast_updating_allowed;
    VIO_BOOL           cursor_follows_paintbrush;
} segmenting_struct;

typedef struct
{
    Real             top_offset;
    Real             bottom_offset;
    Real             left_offset;
    Real             bar_width;
    Real             tick_width;
    int              desired_n_intervals;
} colour_bar_struct;

typedef  struct
{
    VIO_BOOL   crop_visible;
    int       axis_being_moved[2];
    int       limit_being_moved[2];
    int       view_index;
    Real      start_voxel[N_DIMENSIONS];
    Real      limits[2][N_DIMENSIONS];
    STRING    filename;
} crop_struct;

typedef  struct
{
    int              volume_index;
    int              axis_index;
    int              slice_index;
    int              **saved_labels;
} slice_undo_struct;

typedef enum { UPDATE_SLICE, UPDATE_LABELS, UPDATE_BOTH } Update_types;

typedef  struct
{
    STRING                 filename;
    Volume                 volume;
    Volume                 labels;
    VIO_General_transform      original_transform;
    STRING                 labels_filename;
    int                    n_labels;
    int                    offset;
    Colour                 *colour_table;
    Colour                 *label_colour_table;
    Real                   label_colour_opacity;
    colour_coding_struct   colour_coding;
    VIO_BOOL                display_labels;
    Real                   opacity;
    Real                   current_voxel[N_DIMENSIONS];

    struct
    {
        VIO_BOOL                visibility;
        int                    n_pixels_alloced;
        int                    n_label_pixels_alloced;
        Real                   x_axis[N_DIMENSIONS];
        Real                   y_axis[N_DIMENSIONS];
        Real                   x_trans, y_trans;
        Real                   x_scaling, y_scaling;
        VIO_BOOL                update_flag;
        VIO_BOOL                update_labels_flag;
        Filter_types           filter_type;
        Real                   filter_width;
        int                    n_pixels_redraw;
        VIO_BOOL                update_in_progress[2];
        int                    x_min_update[2];
        int                    x_max_update[2];
        int                    y_min_update[2];
        int                    y_max_update[2];
        int                    edge_index[2];
    }  views[N_SLICE_VIEWS];
} loaded_volume_struct;

typedef  struct
{
    int           prev_viewport_x_size;
    int           prev_viewport_y_size;
    int           used_viewport_x_size;
    int           used_viewport_y_size;
    VIO_BOOL       update_cursor_flag;
    VIO_BOOL       update_text_flag;
    VIO_BOOL       update_cross_section_flag;
    VIO_BOOL       update_crop_flag;
    VIO_BOOL       update_atlas_flag;
    VIO_BOOL       update_composite_flag;

    int           n_atlas_pixels_alloced;
    int           n_composite_pixels_alloced;

    VIO_BOOL       use_sub_region;

    VIO_BOOL       sub_region_specified;
    int           x_min;
    int           x_max;
    int           y_min;
    int           y_max;
    VIO_BOOL       prev_sub_region_specified;
    int           prev_x_min;
    int           prev_x_max;
    int           prev_y_min;
    int           prev_y_max;
} slice_view_struct;

typedef  struct
{
    int                    n_volumes;
    loaded_volume_struct   *volumes;
    int                    current_volume_index;

    VIO_BOOL                toggle_undo_feature;

    VIO_BOOL                crop_labels_on_output_flag;
    VIO_BOOL                share_labels_flag;
    colour_bar_struct      colour_bar;

    VIO_BOOL				   print_probe_ratio;
    int					   ratio_volume_index_numerator;
    int                    ratio_volume_index_denominator;

    VIO_BOOL                using_transparency;
    VIO_BOOL                degrees_continuity;

    slice_view_struct      slice_views[N_SLICE_VIEWS];
    void                   *render_storage;

    Real                   x_split, y_split;

    segmenting_struct      segmenting;
    atlas_struct           atlas;
    crop_struct            crop;

    Real                   x_brush_radius, y_brush_radius, z_brush_radius;
    int                    current_paint_label;
    int                    current_erase_label;
    int                    painting_view_index;
    object_struct          *brush_outline;
    slice_undo_struct      undo;

    lines_struct           unscaled_histogram_lines;
    object_struct          *histogram_object;

    VIO_BOOL                cursor_visibility;

    int                    cross_section_index;
    VIO_BOOL                cross_section_visibility;
    VIO_BOOL                cross_section_vector_present;
    Real                   cross_section_vector[MAX_DIMENSIONS];

    VIO_BOOL                update_slice_dividers_flag;
    VIO_BOOL                update_colour_bar_flag;
    VIO_BOOL                update_probe_flag;

    VIO_BOOL                viewport_update_flags[N_MODELS][2];

    VIO_BOOL                incremental_update_allowed;
    Real                   allowable_slice_update_time;
    Real                   total_slice_update_time1;
    Real                   total_slice_update_time2;
    int                    current_update_volume;
    int                    current_update_view;

    Real                   volume_rotation_step;
    Real                   volume_translation_step;
    Real                   volume_scale_step;

} slice_window_struct;


#endif


#include  <def_display.h>

public  DEF_MENU_FUNCTION( set_paint_x_brush_radius )   /* ARGSUSED */
{
    Real            x_brush_radius;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter x brush size: " );

        if( input_real( stdin, &x_brush_radius ) == OK &&
            x_brush_radius >= 0.0 )
            slice_window->slice.x_brush_radius = x_brush_radius;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_paint_x_brush_radius )   /* ARGSUSED */
{
    String           text;
    Real             x_brush_radius;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        x_brush_radius = slice_window->slice.x_brush_radius;
    else
        x_brush_radius = Default_x_brush_radius;

    (void) sprintf( text, label, x_brush_radius );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_paint_y_brush_radius )   /* ARGSUSED */
{
    Real            y_brush_radius;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter y brush size: " );

        if( input_real( stdin, &y_brush_radius ) == OK &&
            y_brush_radius >= 0.0 )
            slice_window->slice.y_brush_radius = y_brush_radius;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_paint_y_brush_radius )   /* ARGSUSED */
{
    String           text;
    Real             y_brush_radius;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        y_brush_radius = slice_window->slice.y_brush_radius;
    else
        y_brush_radius = Default_y_brush_radius;

    (void) sprintf( text, label, y_brush_radius );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_paint_z_brush_radius )   /* ARGSUSED */
{
    Real            z_brush_radius;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter out of plane brush size: " );

        if( input_real( stdin, &z_brush_radius ) == OK &&
            z_brush_radius >= 0.0 )
            slice_window->slice.z_brush_radius = z_brush_radius;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_paint_z_brush_radius )   /* ARGSUSED */
{
    String           text;
    Real             z_brush_radius;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        z_brush_radius = slice_window->slice.z_brush_radius;
    else
        z_brush_radius = Default_z_brush_radius;

    (void) sprintf( text, label, z_brush_radius );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_current_paint_label )   /* ARGSUSED */
{
    int             label;
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        print( "Enter current paint label: " );

        if( input_int( stdin, &label ) == OK &&
            label >= 0 && label <= LOWER_AUXILIARY_BITS )
            slice_window->slice.current_paint_label = label;

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_current_paint_label )   /* ARGSUSED */
{
    String           text;
    int              current_label;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        current_label = slice_window->slice.current_paint_label;
    else
        current_label = Default_paint_label;

    (void) sprintf( text, label, current_label );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_label_colour )   /* ARGSUSED */
{
    display_struct   *slice_window;
    String           line;
    Colour           col;

    if( get_slice_window( display, &slice_window ) &&
        slice_window->slice.current_paint_label > 0 )
    {
        print( "Enter the colour for label %d: ",
               slice_window->slice.current_paint_label );

        if( input_line( stdin, line, MAX_STRING_LENGTH ) == OK )
        {
            col = convert_string_to_colour( line );

            add_new_label( slice_window,
                   ACTIVE_BIT | slice_window->slice.current_paint_label, col );

            set_slice_window_update( display->associated[SLICE_WINDOW], 0 );
            set_slice_window_update( display->associated[SLICE_WINDOW], 1 );
            set_slice_window_update( display->associated[SLICE_WINDOW], 2 );
        }
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_label_colour )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( save_labels )   /* ARGSUSED */
{
    Status         status;
    FILE           *file;
    int            i;
    String         filename;
    display_struct *slice_window;
    Volume         volume;
    Colour         colour_table[LOWER_AUXILIARY_BITS+1];

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        print( "Enter filename to save: " );
        if( input_string( stdin, filename, MAX_STRING_LENGTH, ' ' ) == OK )
        {
            for_less( i, 0, LOWER_AUXILIARY_BITS+1 )
            {
                if( slice_window->slice.label_colours_used[i|ACTIVE_BIT] )
                    colour_table[i] = slice_window->slice.
                                        label_colours[i|ACTIVE_BIT];
                else
                    colour_table[i] = BLACK;
            }

            status = open_file_with_default_suffix( filename, "lmk",
                                            WRITE_FILE, ASCII_FORMAT, &file );

            if( status == OK )
                status = output_labels_as_landmarks( file, volume,
                      display->three_d.default_marker_size,
                      display->three_d.default_marker_patient_id,
                      colour_table );

            if( status == OK )
            status = close_file( file );

            print( "Done saving.\n" );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(save_labels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( load_labels )   /* ARGSUSED */
{
    Status         status;
    FILE           *file;
    String         filename;
    display_struct *slice_window;
    Volume         volume;

    if( get_slice_window_volume( display, &volume ) &&
        get_slice_window( display, &slice_window ) )
    {
        print( "Enter filename to load: " );
        if( input_string( stdin, filename, MAX_STRING_LENGTH, ' ' ) == OK )
        {
            status = open_file_with_default_suffix( filename, "lmk",
                                            READ_FILE, ASCII_FORMAT, &file );

            if( status == OK )
                status = input_landmarks_as_labels( file, volume );

            if( status == OK )
                status = close_file( file );

            set_slice_window_update( slice_window, 0 );
            set_slice_window_update( slice_window, 1 );
            set_slice_window_update( slice_window, 2 );

            print( "Done loading.\n" );
        }

        (void) input_newline( stdin );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(load_labels )   /* ARGSUSED */
{
    return( OK );
}

private  void  copy_labels_from_adjacent_slice(
    display_struct   *display,
    int              src_offset )
{
    int              dest_index[N_DIMENSIONS], src_index[N_DIMENSIONS];
    int              axis_index;
    display_struct   *slice_window;
    Volume           volume;

    if( get_voxel_under_mouse( display, &dest_index[0], &dest_index[1],
                               &dest_index[2], &axis_index ) &&
        get_slice_window_volume( display, &volume) &&
        get_slice_window( display, &slice_window ) )
    {
        src_index[X] = 0;
        src_index[Y] = 0;
        src_index[Z] = 0;
        src_index[axis_index] = dest_index[axis_index] + src_offset;

        if( cube_is_within_volume( volume, src_index ) )
        {
            copy_labels_slice_to_slice( volume, axis_index,
                                        src_index[axis_index],
                                        dest_index[axis_index] );

            set_slice_window_update( slice_window, 0 );
            set_slice_window_update( slice_window, 1 );
            set_slice_window_update( slice_window, 2 );
        }
    }
}

public  DEF_MENU_FUNCTION( copy_labels_from_lower_slice )   /* ARGSUSED */
{
    copy_labels_from_adjacent_slice( display, -1 );

    return( OK );
}

public  DEF_MENU_UPDATE(copy_labels_from_lower_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( copy_labels_from_higher_slice )   /* ARGSUSED */
{
    copy_labels_from_adjacent_slice( display, 1 );

    return( OK );
}

public  DEF_MENU_UPDATE(copy_labels_from_higher_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_display_labels )   /* ARGSUSED */
{
    display_struct  *slice_window;

    if( get_slice_window( display, &slice_window ) )
    {
        slice_window->slice.display_labels =!slice_window->slice.display_labels;
        set_slice_window_update( slice_window, 0 );
        set_slice_window_update( slice_window, 1 );
        set_slice_window_update( slice_window, 2 );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_display_labels )   /* ARGSUSED */
{
    String           text;
    Boolean          display_labels;
    display_struct   *slice_window;

    if( get_slice_window( display, &slice_window ) )
        display_labels = Initial_display_labels;
    else
        display_labels = slice_window->slice.display_labels;

    set_text_on_off( label, text, display_labels );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}
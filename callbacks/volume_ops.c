
#include  <def_graphics.h>
#include  <def_math.h>
#include  <def_files.h>

public  Boolean  get_current_volume( graphics, volume )
    graphics_struct   *graphics;
    volume_struct     **volume;
{
    Status          status;
    Boolean         found;
    object_struct   *current_object, *object;
    Boolean         get_current_object();
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();

    found = get_slice_window_volume( graphics, volume );

    if( !found )
    {
        if( get_current_object( graphics, &current_object ) )
        {
            status = initialize_object_traverse( &object_traverse,
                                                 1, &current_object );

            if( status == OK )
            {
                while( get_next_object_traverse(&object_traverse, &object) )
                {
                    if( !found && object->object_type == VOLUME )
                    {
                        found = TRUE;
                        *volume = object->ptr.volume;
                    }
                }
            }
        }
    }

    return( found );
}

public  Boolean  get_slice_view_index_under_mouse( graphics, view_index )
    graphics_struct  *graphics;
    int              *view_index;
{
    Boolean          found;
    volume_struct    *volume;
    graphics_struct  *slice_window;
    Point            *mouse;
    int              x, y;
    void             get_mouse_in_pixels();
    Boolean          find_slice_view_mouse_is_in();

    found = FALSE;

    if( get_current_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        mouse = &slice_window->mouse_position;

        get_mouse_in_pixels( slice_window, mouse, &x, &y );

        if( find_slice_view_mouse_is_in( slice_window, x, y, view_index ) )
            found = TRUE;
    }

    return( found );
}

public  Boolean  get_axis_index_under_mouse( graphics, axis_index )
    graphics_struct  *graphics;
    int              *axis_index;
{
    Boolean          found;
    int              view_index;
    graphics_struct  *slice_window;

    found = get_slice_view_index_under_mouse( graphics, &view_index );

    if( found )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        *axis_index = 
             slice_window->slice.slice_views[view_index].axis_map[Z];
    }

    return( found );
}

public  DEF_MENU_FUNCTION( advance_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            graphics_models_have_changed();
    int             nx, ny, nz;
    void            get_volume_size();

    if( get_current_volume(graphics,&volume) )
    {
        volume->slice_visibilities[volume->current_slice] = OFF;
        ++volume->current_slice;
        get_volume_size( volume, &nx, &ny, &nz );
        if( volume->current_slice >= nz )
        {
            volume->current_slice = 0;
        }

        volume->slice_visibilities[volume->current_slice] = ON;

        graphics_models_have_changed( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(advance_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( retreat_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            graphics_models_have_changed();
    int             nx, ny, nz;
    void            get_volume_size();

    if( get_current_volume(graphics,&volume) )
    {
        volume->slice_visibilities[volume->current_slice] = OFF;
        --volume->current_slice;
        if( volume->current_slice < 0 )
        {
            get_volume_size( volume, &nx, &ny, &nz );
            volume->current_slice = nz-1;
        }
        volume->slice_visibilities[volume->current_slice] = ON;

        graphics_models_have_changed( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(retreat_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( next_marked_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            graphics_models_have_changed();
    int             i, nx, ny, nz;
    void            get_volume_size();

    if( get_current_volume(graphics,&volume) )
    {
        get_volume_size( volume, &nx, &ny, &nz );

        i = volume->current_slice;

        do
        {
            ++i;
            if( i >= nz )
                i = 0;
        } while( i != volume->current_slice && !volume->slice_marked_flags[i] );

        if( i != volume->current_slice )
        {
            volume->slice_visibilities[volume->current_slice] = OFF;

            volume->current_slice = i;
            volume->slice_visibilities[volume->current_slice] = ON;
        }

        graphics_models_have_changed( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(next_marked_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( prev_marked_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            graphics_models_have_changed();
    int             i, nx, ny, nz;
    void            get_volume_size();

    if( get_current_volume(graphics,&volume) )
    {
        get_volume_size( volume, &nx, &ny, &nz );

        i = volume->current_slice;

        do
        {
            --i;
            if( i < 0 )
                i = nz - 1;
        } while( i != volume->current_slice && !volume->slice_marked_flags[i] );

        if( i != volume->current_slice )
        {
            volume->slice_visibilities[volume->current_slice] = OFF;

            volume->current_slice = i;
            volume->slice_visibilities[volume->current_slice] = ON;
        }

        graphics_models_have_changed( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(prev_marked_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( toggle_marked_slice )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            set_update_required();

    if( get_current_volume(graphics,&volume) )
    {
        if( volume->slice_marked_flags[volume->current_slice] )
        {
            volume->slice_visibilities[volume->current_slice] = OFF;
            volume->slice_marked_flags[volume->current_slice] = OFF;
        }
        else
        {
            volume->slice_visibilities[volume->current_slice] = ON;
            volume->slice_marked_flags[volume->current_slice] = ON;
        }
    }

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(toggle_marked_slice )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(open_slice_window )   /* ARGSUSED */
{
    Status           status;
    Status           create_slice_window();
    volume_struct    *volume;

    status = OK;

    if( get_current_volume( graphics, &volume ) &&
        graphics->associated[SLICE_WINDOW] == (graphics_struct *) 0 )
    {
        status = create_slice_window( graphics, volume );
    }

    return( status );
}

public  DEF_MENU_UPDATE(open_slice_window )   /* ARGSUSED */
{
    return( OK );
}

private  Status  change_current_slice_by_one( graphics, delta )
    graphics_struct  *graphics;
    int              delta;
{
    Status           status;
    graphics_struct  *slice_window;
    volume_struct    *volume;
    int              index[3], sizes[3], axis_index;
    Boolean          get_voxel_under_mouse();
    void             set_slice_window_update();
    void             get_volume_size();
    void             get_current_voxel();
    void             rebuild_probe();
    void             rebuild_cursor();
    void             set_update_required();

    status = OK;

    if( get_axis_index_under_mouse( graphics, &axis_index ) &&
        get_slice_window_volume( graphics, &volume ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        get_volume_size( volume, &sizes[X], &sizes[Y],
                         &sizes[Z] );

        get_current_voxel( slice_window,
                           &index[X], &index[Y], &index[Z] );

        index[axis_index] += delta;

        if( index[axis_index] >= 0 && index[axis_index] < sizes[axis_index] )
        {
            if( set_current_voxel( slice_window, index[X], index[Y],
                                   index[Z] ) )
            {
                rebuild_probe( slice_window );
                rebuild_cursor( slice_window, 0 );
                rebuild_cursor( slice_window, 1 );
                rebuild_cursor( slice_window, 2 );
                set_slice_window_update( slice_window, axis_index );

                if( update_cursor_from_voxel( slice_window ) )
                {
                    set_update_required( slice_window->
                                         associated[THREE_D_WINDOW],
                                         OVERLAY_PLANES );
                }
            }
        }
    }

    return( status );
}

public  DEF_MENU_FUNCTION(move_slice_plus)   /* ARGSUSED */
{
    Status           status;

    status = change_current_slice_by_one( graphics, 1 );

    return( status );
}

public  DEF_MENU_UPDATE(move_slice_plus )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(move_slice_minus)   /* ARGSUSED */
{
    Status           status;

    status = change_current_slice_by_one( graphics, -1 );

    return( status );
}

public  DEF_MENU_UPDATE(move_slice_minus )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(double_slice_voxels)   /* ARGSUSED */
{
    Status           status;
    graphics_struct  *slice_window;
    int              view_index;
    Boolean          get_slice_view_index_under_mouse();
    void             set_slice_window_update();
    void             set_update_required();

    status = OK;

    if( get_slice_view_index_under_mouse( graphics, &view_index ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        slice_window->slice.slice_views[view_index].x_scale *= 2.0;
        slice_window->slice.slice_views[view_index].y_scale *= 2.0;
        set_slice_window_update( slice_window, view_index );
        set_update_required( slice_window, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(double_slice_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(halve_slice_voxels)   /* ARGSUSED */
{
    Status           status;
    graphics_struct  *slice_window;
    int              view_index;
    Boolean          get_slice_view_index_under_mouse();
    void             set_slice_window_update();
    void             set_update_required();

    status = OK;

    if( get_slice_view_index_under_mouse( graphics, &view_index ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        slice_window->slice.slice_views[view_index].x_scale *= 0.5;
        slice_window->slice.slice_views[view_index].y_scale *= 0.5;
        set_slice_window_update( slice_window, view_index );
        set_update_required( slice_window, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(halve_slice_voxels )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(toggle_lock_slice)   /* ARGSUSED */
{
    Status           status;
    graphics_struct  *slice_window;
    int              axis_index, view_index;
    void             get_mouse_in_pixels();
    Boolean          get_slice_view_index_under_mouse();
    void             set_slice_window_update();
    void             set_update_required();

    status = OK;

    if( get_slice_view_index_under_mouse( graphics, &view_index ) )
    {
        slice_window = graphics->associated[SLICE_WINDOW];

        axis_index = 
           slice_window->slice.slice_views[view_index].axis_map[Z];
        slice_window->slice.slice_locked[axis_index] =
            !slice_window->slice.slice_locked[axis_index];

        set_slice_window_update( slice_window, view_index );
        set_update_required( slice_window, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(toggle_lock_slice)    /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_rotating_slice_mode )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            start_rotating_slice();

    if( get_current_volume(graphics,&volume) )
    {
        start_rotating_slice( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_rotating_slice_mode )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(set_translating_slice_mode )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            start_translating_slice();

    if( get_current_volume(graphics,&volume) )
    {
        start_translating_slice( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_translating_slice_mode )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(reset_slice_transform )   /* ARGSUSED */
{
    volume_struct   *volume;
    void            make_identity_transform();
    void            set_update_required();

    if( get_current_volume(graphics,&volume) )
    {
        make_identity_transform( &volume->slice_transforms[
                                     volume->current_slice] );
    }

    set_update_required( graphics, NORMAL_PLANES );

    return( OK );
}

public  DEF_MENU_UPDATE(reset_slice_transform )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(output_slice_transforms )   /* ARGSUSED */
{
    volume_struct   *volume;
    int             z, nx, ny, nz;
    FILE            *file;
    Status          status;
    String          filename;
    Real            degrees, x_trans, y_trans;
    void            get_volume_size();
    void            convert_2d_transform_to_rotation_translation();

    status = OK;

    if( get_current_volume(graphics,&volume) )
    {
        PRINT( "Enter filename: " );

        status = input_string( stdin, filename, MAX_STRING_LENGTH, ' ' );

        (void) input_newline( stdin );

        if( status == OK )
            status = open_file_with_default_suffix( filename, "xfm",
                                            WRITE_FILE, ASCII_FORMAT, &file );

        if( status == OK )
        {
            get_volume_size( volume, &nx, &ny, &nz );

            for_less( z, 0, nz )
            {
                convert_2d_transform_to_rotation_translation(
                        &volume->slice_transforms[z], &degrees,
                        &x_trans, &y_trans );

                status = io_real( file, WRITE_FILE, ASCII_FORMAT, &degrees );

                if( status == OK )
                    status = io_real( file, WRITE_FILE, ASCII_FORMAT, &x_trans);

                if( status == OK )
                    status = io_real( file, WRITE_FILE, ASCII_FORMAT, &y_trans);

                if( status == OK )
                    status = io_newline( file, WRITE_FILE, ASCII_FORMAT );
            }
        }

        if( status == OK )
            status = close_file( file );

        PRINT( "Done\n" );
    }

    return( status );
}

public  DEF_MENU_UPDATE(output_slice_transforms )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION(colour_code_objects )   /* ARGSUSED */
{
    object_struct           *object, *current_object;
    volume_struct           *volume;
    colour_coding_struct    *colour_coding;
    Status                  status;
    object_traverse_struct  object_traverse;
    Status                  initialize_object_traverse();
    void                    colour_code_object();
    void                    set_update_required();

    status = OK;

    if( get_current_object(graphics,&current_object) &&
        get_slice_window_volume( graphics, &volume ) )
    {
        colour_coding =
                 &graphics->associated[SLICE_WINDOW]->slice.colour_coding;

        status = initialize_object_traverse( &object_traverse, 1,
                                             &current_object );

        while( get_next_object_traverse(&object_traverse,&object) )
        {
            if( object->object_type == POLYGONS )
            {
                colour_code_object( colour_coding, volume,
                                    &object->ptr.polygons->colour_flag,
                                    &object->ptr.polygons->colours,
                                    object->ptr.polygons->n_points,
                                    object->ptr.polygons->points );
            }
            else if( object->object_type == QUADMESH )
            {
                colour_code_object( colour_coding, volume,
                                    &object->ptr.quadmesh->colour_flag,
                                    &object->ptr.quadmesh->colours,
                                    object->ptr.quadmesh->m *
                                    object->ptr.quadmesh->n,
                                    object->ptr.quadmesh->points );
            }
            else if( object->object_type == LINES )
            {
                colour_code_object( colour_coding, volume,
                                    &object->ptr.lines->colour_flag,
                                    &object->ptr.lines->colours,
                                    object->ptr.lines->n_points,
                                    object->ptr.lines->points );
            }
        }

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(colour_code_objects )   /* ARGSUSED */
{
    return( OK );
}

private  void  colour_code_object( colour_coding, volume,
                                   colour_flag, colours, n_points, points )
    colour_coding_struct  *colour_coding;
    volume_struct         *volume;
    Colour_flags          *colour_flag;
    Colour                *colours[];
    int                   n_points;
    Point                 points[];
{
    Status   status;
    int      i;
    Real     val;
    Boolean  evaluate_volume_at_point();
    void     get_colour_coding();

    status = OK;

    if( *colour_flag != PER_VERTEX_COLOURS )
    {
        REALLOC( status, *colours, n_points );
        *colour_flag = PER_VERTEX_COLOURS;
    }

    if( status == OK )
    {
        for_less( i, 0, n_points )
        {
            (void) evaluate_volume_at_point( volume,
                                             Point_x(points[i]),
                                             Point_y(points[i]),
                                             Point_z(points[i]), FALSE,
                                             &val, (Real *) 0,
                                             (Real *) 0, (Real *) 0);

            get_colour_coding( colour_coding, val, &(*colours)[i] );
        }
    }
}

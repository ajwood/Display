
#include  <display.h>

private    DEF_EVENT_FUNCTION( check_updated );

private  Status  create_film_loop_header(
    char   base_filename[],
    int    window_width,
    int    window_height,
    int    n_steps );
private  void  create_frame_filename(
    char   base_filename[],
    int    step,
    char   frame_filename[] );
private  Status  save_image_to_file(
    display_struct    *display );
private  void  display_next_frame(
    display_struct   *display );

public  Status  start_film_loop(
    display_struct   *display,
    char             base_filename[],
    int              axis_index,
    int              n_steps )
{
    Status    status;
    Real      angle;
    int       x_size, y_size;

    add_action_table_function( &display->action_table, NO_EVENT,
                               check_updated );

    angle = 2.0 * PI / n_steps;

    make_rotation_transform( angle, axis_index,
                             &display->three_d.film_loop.transform );

    display->three_d.film_loop.n_steps = n_steps;
    display->three_d.film_loop.current_step = 1;

    G_get_window_size( display->window, &x_size, &y_size );

    if( (x_size & 1) == 1 ) --x_size;
    if( (y_size & 1) == 1 ) --y_size;

    display->three_d.film_loop.x_size = x_size;
    display->three_d.film_loop.y_size = y_size;
    (void) strcpy( display->three_d.film_loop.base_filename, base_filename );

    status = create_film_loop_header( base_filename, x_size, y_size, n_steps );

    return( status );
}

private  void  end_film_loop(
    display_struct   *display )
{
    remove_action_table_function( &display->action_table, NO_EVENT,
                                  check_updated );

    print( "Done film loop.\n" );
}

private  DEF_EVENT_FUNCTION( check_updated )
    /* ARGSUSED */
{
    Status    status;

    status = OK;

    if( window_is_up_to_date( display ) )
    {
        print( "Frame %d/%d\n", display->three_d.film_loop.current_step,
               display->three_d.film_loop.n_steps );

        status = save_image_to_file( display );

        if( status == OK )
        {
            ++display->three_d.film_loop.current_step;

            if( display->three_d.film_loop.current_step <= 
                display->three_d.film_loop.n_steps )
            {
                display_next_frame( display );

                update_view( display );

                set_update_required( display, NORMAL_PLANES );
            }
            else
                end_film_loop( display );
        }
        else
            end_film_loop( display );
    }

    return( OK );
}

private  Status  create_film_loop_header(
    char   base_filename[],
    int    window_width,
    int    window_height,
    int    n_steps )
{
    Status  status;
    int     i;
    FILE    *file;
    STRING  header_name;
    STRING  frame_filename;
    STRING  no_dirs;

    (void) strcpy( header_name, base_filename );   
    (void) strcat( header_name, ".flm" );   

    status = open_file( header_name, WRITE_FILE, ASCII_FORMAT, &file );

    if( status == OK )
        status = io_int( file, WRITE_FILE, ASCII_FORMAT, &window_width );

    if( status == OK )
        status = io_int( file, WRITE_FILE, ASCII_FORMAT, &window_height );

    if( status == OK )
        status = io_colour( file, WRITE_FILE, ASCII_FORMAT,
                            &Initial_background_colour );

    if( status == OK )
        status = io_newline( file, WRITE_FILE, ASCII_FORMAT );

    for_less( i, 0, n_steps )
    {
        create_frame_filename( base_filename, i+1, frame_filename );

        strip_off_directories( frame_filename, no_dirs );

        if( status == OK )
            status = output_string( file, no_dirs );

        if( status == OK )
            status = io_newline( file, WRITE_FILE, ASCII_FORMAT );
    }

    if( status == OK )
        status = close_file( file );

    return( status );
}

private  void  create_frame_filename(
    char   base_filename[],
    int    step,
    char   frame_filename[] )
{
    (void) sprintf( frame_filename, "%s_%d.rgb", base_filename, step );
}

private  Status  save_image_to_file(
    display_struct    *display )
{
    Status         status;
    STRING         frame_filename;

    create_frame_filename( display->three_d.film_loop.base_filename,
                           display->three_d.film_loop.current_step,
                           frame_filename );

    status = save_window_to_file( display, frame_filename );

    return( status );
}

private  void  display_next_frame(
    display_struct   *display )
{
    apply_transform_in_view_space( display,
                                   &display->three_d.film_loop.transform );
}

#include  <stdio.h>
#include  <def_graphics.h>
#include  <def_globals.h>

int  main( argc, argv )
    int     argc;
    char    *argv[];
{
    graphics_struct  *graphics;
    graphics_struct  *menu;
    Status           status;
    Status           initialize_graphics();
    Status           initialize_globals();
    Status           initialize_menu();
    Status           load_graphics_file();
    Status           create_graphics_window();
    Status           main_event_loop();
    Status           terminate_graphics();
    void             reset_view_parameters();
    void             update_view();
    void             set_model_scale();
    void             rebuild_selected_list();
    void             output_alloc_to_file();

    status = initialize_globals();

    if( status == OK )
    {
        status = initialize_graphics();
    }

    if( status == OK )
    {
        status = create_graphics_window( THREE_D_WINDOW,
                                         &graphics, argv[1], 0, 0 );
    }

    if( status == OK )
    {
        status = create_graphics_window( MENU_WINDOW, &menu, argv[1],
                                         Menu_window_width,
                                         Menu_window_height );
    }

    if( status == OK )
    {
        graphics->associated[THREE_D_WINDOW] = graphics;
        graphics->associated[MENU_WINDOW] = menu;
        graphics->associated[SLICE_WINDOW] = (graphics_struct *) 0;

        menu->associated[THREE_D_WINDOW] = graphics;
        menu->associated[MENU_WINDOW] = menu;
        menu->associated[SLICE_WINDOW] = (graphics_struct *) 0;

        status = initialize_menu( menu );
    }

    if( status == OK && argc > 1 )
    {
        status = load_graphics_file( graphics, argv[1] );
    }

    if( status == OK )
    {
        rebuild_selected_list( graphics, menu );
    }

    if( status == OK )
    {
        reset_view_parameters( graphics, &Default_line_of_sight,
                               &Default_horizontal );

        update_view( graphics );
    }

    if( status == OK )
    {
        status = main_event_loop();
    }

    (void) terminate_graphics();

    output_alloc_to_file( ".alloc_stats" );

    if( status != OK )
    {
        PRINT( "Program ended with error %d\n", (int) status );
    }

    return( (int) status );
}
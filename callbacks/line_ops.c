
#include  <def_graphics.h>
#include  <def_files.h>

public  DEF_MENU_FUNCTION( smooth_current_lines )   /* ARGSUSED */
{
    Status          status;
    Status          smooth_lines();
    object_struct   *current_object;
    Boolean         get_current_object();
    void            set_update_required();
    Real            smooth_distance;

    status = OK;

    if( get_current_object( graphics, &current_object ) &&
        current_object->object_type == LINES )
    {
        PRINT( "Enter smoothing distance: " );
        (void) scanf( "%f", &smooth_distance );

        status = smooth_lines( current_object->ptr.lines, smooth_distance );

        set_update_required( graphics, NORMAL_PLANES );
    }

    return( status );
}

public  DEF_MENU_UPDATE(smooth_current_lines )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( make_current_line_tube )   /* ARGSUSED */
{
    Status          status;
    Status          convert_lines_to_tubes();
    object_struct   *current_object;
    Boolean         get_current_object();
    void            graphics_models_have_changed();
    Real            radius;
    int             n_around;

    status = OK;

    if( get_current_object( graphics, &current_object ) &&
        current_object->object_type == LINES )
    {
        PRINT( "Enter n_around radius: " );
        (void) scanf( "%d %f", &n_around, &radius );

        status = convert_lines_to_tubes( graphics,
                                         current_object->ptr.lines, n_around,
                                         radius );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(make_current_line_tube )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( convert_line_to_spline_points )   /* ARGSUSED */
{
    Status          status;
    Status          create_line_spline();
    Status          create_object();
    Status          add_object_to_model();
    void            graphics_models_have_changed();
    object_struct   *current_object, *object;
    Boolean         get_current_object();
    lines_struct    new_lines;
    model_struct    *get_current_model();
    render_struct   *render;
    render_struct   *get_main_render();

    status = OK;

    if( get_current_object( graphics, &current_object ) &&
        current_object->object_type == LINES )
    {
        render = get_main_render( graphics );

        status = create_line_spline( current_object->ptr.lines,
                                     render->n_curve_segments,
                                     &new_lines );

        if( status == OK )
            status = create_object( &object, LINES );

        if( status == OK )
        {
            *(object->ptr.lines) = new_lines;
            status = add_object_to_model( get_current_model(graphics), object );

            graphics_models_have_changed( graphics );
        }
    }

    return( status );
}

public  DEF_MENU_UPDATE(convert_line_to_spline_points )   /* ARGSUSED */
{
    return( OK );
}

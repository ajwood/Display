
#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_math.h>
#include  <def_stdio.h>

public  DEF_MENU_FUNCTION( reset_polygon_visibility )   /* ARGSUSED */
{
    int              i;
    polygons_struct  *polygons;
    void             graphics_models_have_changed();
    void             set_polygons_visibilities();

    if( get_current_polygons(graphics,&polygons) )
    {
        set_polygons_visibilities( polygons, TRUE );

        if( polygons->colour_flag == PER_ITEM_COLOURS )
        {
            for_less( i, 0, polygons->n_items )
                polygons->colours[i] = Visible_segmenting_colour;
        }

        graphics_models_have_changed( graphics );
    }

    return( OK );
}

public  DEF_MENU_UPDATE(reset_polygon_visibility )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( remove_invisible_parts_of_polygon )   /* ARGSUSED */
{
    Status           status;
    Status           remove_invisible_polygons();
    polygons_struct  *polygons;
    void             graphics_models_have_changed();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = remove_invisible_polygons( polygons );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(remove_invisible_parts_of_polygon)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_n_paint_polygons )   /* ARGSUSED */
{
    int   n;

    PRINT( "Enter # paint polygons: " );
    if( scanf( "%d", &n ) == 1 && n >= 0 )
    {
        graphics->three_d.surface_edit.n_paint_polygons = n;
    }

    return( OK );
}

public  DEF_MENU_UPDATE(set_n_paint_polygons)   /* ARGSUSED */
{
    String   text;
    void     set_menu_text();

    (void) sprintf( text, label,
                    graphics->three_d.surface_edit.n_paint_polygons );

    set_menu_text( menu_window, menu_entry, text );

    return( OK );
}

public  DEF_MENU_FUNCTION( set_connected_invisible )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            intersection_point;
    void             graphics_models_have_changed();
    Status           status;
    Status           set_visibility_around_poly();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &intersection_point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                polygons->n_items,
                TRUE, OFF, TRUE, &Invisible_segmenting_colour );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_connected_invisible )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_invisible )   /* ARGSUSED */
{
    Status           status;
    Status           set_visibility_around_poly();
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;
    void             graphics_models_have_changed();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                       graphics->three_d.surface_edit.n_paint_polygons,
                       TRUE, OFF,
                       TRUE, &Invisible_segmenting_colour );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paint_invisible)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_visible )   /* ARGSUSED */
{
    Status           status;
    Status           set_visibility_around_poly();
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;
    void             graphics_models_have_changed();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                         graphics->three_d.surface_edit.n_paint_polygons,
                         TRUE, TRUE,
                         TRUE, &Visible_segmenting_colour );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paint_visible)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_connected_vis_colour )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            intersection_point;
    void             graphics_models_have_changed();
    Status           status;
    Status           set_visibility_around_poly();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &intersection_point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                 polygons->n_items,
                 FALSE, OFF, TRUE, &Visible_segmenting_colour );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_connected_vis_colour )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_connected_invis_colour )   /* ARGSUSED */
{
    polygons_struct  *polygons;
    int              poly_index;
    Point            intersection_point;
    void             graphics_models_have_changed();
    Status           status;
    Status           set_visibility_around_poly();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &intersection_point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
               polygons->n_items,
               FALSE, OFF, TRUE, &Invisible_segmenting_colour );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_connected_invis_colour )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_invis_colour )   /* ARGSUSED */
{
    Status           status;
    Status           set_visibility_around_poly();
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;
    void             graphics_models_have_changed();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                       graphics->three_d.surface_edit.n_paint_polygons,
                       FALSE, OFF,
                       TRUE, &Invisible_segmenting_colour );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paint_invis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( paint_vis_colour )   /* ARGSUSED */
{
    Status           status;
    Status           set_visibility_around_poly();
    polygons_struct  *polygons;
    int              poly_index;
    Point            point;
    void             graphics_models_have_changed();

    status = OK;

    if( get_mouse_scene_intersection( graphics, &polygons, &poly_index,
                                      &point ) )
    {
        status = set_visibility_around_poly( polygons, poly_index,
                         graphics->three_d.surface_edit.n_paint_polygons,
                         FALSE, OFF,
                         TRUE, &Visible_segmenting_colour );

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(paint_vis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_visibility_from_colour )   /* ARGSUSED */
{
    int              i;
    Status           status;
    Status           create_polygons_visibilities();
    polygons_struct  *polygons;
    void             graphics_models_have_changed();

    status = OK;

    if( get_current_polygons(graphics,&polygons) && 
        polygons->colour_flag == PER_ITEM_COLOURS )
    {
        status = create_polygons_visibilities( polygons );

        for_less( i, 0, polygons->n_items )
        {
            polygons->visibilities[i] =
                    ( !equal_colours(&Invisible_segmenting_colour,
                                     &polygons->colours[i]) );
        }

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_visibility_from_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( set_vis_to_invis_colour )   /* ARGSUSED */
{
    int              i;
    Status           status;
    Status           set_polygon_per_item_colours();
    polygons_struct  *polygons;
    void             graphics_models_have_changed();

    status = OK;

    if( get_current_polygons(graphics,&polygons) )
    {
        status = set_polygon_per_item_colours( polygons );

        for_less( i, 0, polygons->n_items )
        {
            if( polygons->visibilities == (Smallest_int *) 0 ||
                polygons->visibilities[i] )
            {
                polygons->colours[i] = Invisible_segmenting_colour;
            }
        }

        graphics_models_have_changed( graphics );
    }

    return( status );
}

public  DEF_MENU_UPDATE(set_vis_to_invis_colour)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( crop_above_plane )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_UPDATE(crop_above_plane)   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_FUNCTION( crop_below_plane )   /* ARGSUSED */
{
    return( OK );
}

public  DEF_MENU_UPDATE(crop_below_plane)   /* ARGSUSED */
{
    return( OK );
}

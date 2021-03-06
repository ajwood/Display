#include  <display.h>

#ifndef lint
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/cursor/contours.c,v 1.8 1995-10-19 15:51:03 david Exp $";
#endif

public  void  create_cursor_contours(
    display_struct   *display )
{
    int             axis;
    object_struct   *object;
    lines_struct    *lines;
    model_struct    *model;

    if( G_has_overlay_planes() )
        model = get_graphics_model( display, OVERLAY_MODEL );
    else
        model = get_graphics_model( display, MISCELLANEOUS_MODEL );

    for_less( axis, 0, N_DIMENSIONS )
    {
        object = create_object( LINES );
        lines = get_lines_ptr( object );
        initialize_lines( lines, Cursor_colour );
        lines->line_thickness = 2.0;

        add_object_to_model( model, object );

        display->three_d.cursor_contours.contours[axis].lines=object->ptr.lines;
        display->three_d.cursor_contours.contours[axis].n_points_alloced = 0;
        display->three_d.cursor_contours.contours[axis].n_indices_alloced = 0;
        display->three_d.cursor_contours.contours[axis].n_end_indices_alloced=0;
    }
}

#ifdef DELETE
public  void  update_cursor_contours(
    display_struct   *display )
{
    int                      axis;
    Vector                   plane_normal;
    Real                     plane_constant;
    cursor_contours_struct   *contours;

    if( Show_cursor_contours )
    {
        contours = &display->three_d.cursor_contours;

        for_less( axis, 0, N_DIMENSIONS )
        {
            fill_Vector( plane_normal, 0.0, 0.0, 0.0 );
            Vector_coord(plane_normal,axis) = 1.0;

            plane_constant = Point_coord( display->three_d.cursor.origin,
                                          axis );

            intersect_plane_with_polygons( display, &plane_normal,
                           plane_constant,
                           contours->contours[axis].lines,
                           &contours->contours[axis].n_points_alloced,
                           &contours->contours[axis].n_indices_alloced,
                           &contours->contours[axis].n_end_indices_alloced );
        }
    }
}
#endif

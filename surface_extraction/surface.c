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
static char rcsid[] = "$Header: /private-cvsroot/visualization/Display/surface_extraction/surface.c,v 1.60 1996-05-23 13:48:34 david Exp $";
#endif


#include  <display.h>

private  BOOLEAN  find_close_voxel_containing_range(
    Volume                     volume,
    Volume                     label_volume,
    unsigned_byte              voxel_done_flags[],
    surface_extraction_struct  *surface_extraction,
    int                        x,
    int                        y,
    int                        z,
    int                        found_indices[] );
private  void  add_voxel_neighbours(
    Volume                              volume,
    Volume                              label_volume,
    int                                 sizes[],
    int                                 x,
    int                                 y,
    int                                 z,
    BOOLEAN                             surface_only,
    surface_extraction_struct           *surface_extraction,
    bitlist_3d_struct                   *voxels_queued,
    voxel_queue_struct                  *voxel_queue );
private  void  delete_edge_points_no_longer_needed(
    surface_extraction_struct       *surface_extraction,
    Volume                          volume,
    int                             voxel_index[],
    unsigned_byte                   voxel_done_flags[],
    hash_table_struct               *edge_points );

private  BOOLEAN  surface_voxel_is_within_volume(
    surface_extraction_struct   *surface_extraction,
    int                         indices[] )
{

    return( indices[X] >= surface_extraction->min_limits[X] &&
            indices[X] <= surface_extraction->max_limits[X] &&
            indices[Y] >= surface_extraction->min_limits[Y] &&
            indices[Y] <= surface_extraction->max_limits[Y] &&
            indices[Z] >= surface_extraction->min_limits[Z] &&
            indices[Z] <= surface_extraction->max_limits[Z] );
}

public  void  start_surface_extraction_at_point(
    display_struct     *display,
    Volume             volume,
    Volume             label_volume,
    BOOLEAN            binary_flag,
    BOOLEAN            voxellate_flag,
    Real               min_value,
    Real               max_value,
    int                x,
    int                y,
    int                z )
{
    int                         dim, sizes[N_DIMENSIONS];
    int                         indices[N_DIMENSIONS];
    surface_extraction_struct   *surface_extraction;
    int                         voxel_indices[N_DIMENSIONS];
    int                         offset;
    int                         min_crop[N_DIMENSIONS], max_crop[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    surface_extraction = &display->three_d.surface_extraction;

    surface_extraction->volume = volume;
    surface_extraction->label_volume = label_volume;
    surface_extraction->binary_flag = binary_flag;
    surface_extraction->voxellate_flag = voxellate_flag;
    surface_extraction->min_value = min_value;
    surface_extraction->max_value = max_value;

    indices[X] = x;
    indices[Y] = y;
    indices[Z] = z;

    if( voxellate_flag )
        offset = 0;
    else
        offset = 1;

    get_volume_crop_limits( display, min_crop, max_crop );

    for_less( dim, 0, N_DIMENSIONS )
    {
        surface_extraction->min_limits[dim] = min_crop[dim];
        surface_extraction->max_limits[dim] = MIN( sizes[dim]-1-offset,
                                                   max_crop[dim] );

        surface_extraction->min_changed_limits[dim] =
                                   surface_extraction->min_limits[dim];
        surface_extraction->max_changed_limits[dim] =
                                   surface_extraction->max_limits[dim];
        surface_extraction->min_modified[dim] = 0;
        surface_extraction->max_modified[dim] = -1;
        surface_extraction->not_changed_since[dim] =
                                   surface_extraction->min_limits[dim];

        if( voxellate_flag )
        {
            surface_extraction->current_voxel[dim] =
                                    surface_extraction->min_limits[dim];
        }
        else
        {
            surface_extraction->current_voxel[dim] = indices[dim];
        }
    }

    if( !surface_voxel_is_within_volume( surface_extraction, indices ) )
    {
        /*--- turn off surface extraction */

        surface_extraction->volume = NULL;
        surface_extraction->label_volume = NULL;
        print( "Starting voxel is not within crop limits.\n" );
        return;
    }

    if( voxellate_flag )
    {
        initialize_edge_points( &surface_extraction->faces_done );
        INITIALIZE_QUEUE( surface_extraction->deleted_faces );
    }
    else
    {
        initialize_voxel_queue( &surface_extraction->voxels_to_do );
        initialize_voxel_done_flags( &surface_extraction->voxel_done_flags,
                                     surface_extraction->min_limits,
                                     surface_extraction->max_limits );
    }

    initialize_voxel_flags( &surface_extraction->voxel_state,
                            surface_extraction->min_limits,
                            surface_extraction->max_limits );
    initialize_edge_points( &surface_extraction->edge_points );

    surface_extraction->n_voxels_with_surface = 0;

    if( !voxellate_flag )
    {
        if( find_close_voxel_containing_range( volume, label_volume,
                  surface_extraction->voxel_done_flags, surface_extraction,
                  x, y, z, voxel_indices ) )
        {
            insert_in_voxel_queue( &surface_extraction->voxels_to_do,
                                   voxel_indices );

            set_voxel_flag( &surface_extraction->voxel_state,
                            surface_extraction->min_limits,
                            voxel_indices );

            start_surface_extraction( display );
        }
        else
            print( "No surface voxels found.\n" );
    }
    else
        start_surface_extraction( display );
}

private  BOOLEAN  find_close_voxel_containing_range(
    Volume                     volume,
    Volume                     label_volume,
    unsigned_byte              voxel_done_flags[],
    surface_extraction_struct  *surface_extraction,
    int                        x,
    int                        y,
    int                        z,
    int                        found_indices[] )
{
    BOOLEAN                   found, voxel_contains;
    int                       sizes[MAX_DIMENSIONS], voxel[N_DIMENSIONS];
    BOOLEAN                   voxel_done;
    voxel_queue_struct        voxels_to_check;
    int                       insert[N_DIMENSIONS];
    bitlist_3d_struct         voxels_searched;

    get_volume_sizes( volume, sizes );

    insert[X] = MIN( x, sizes[X]-2 );
    insert[Y] = MIN( y, sizes[Y]-2 );
    insert[Z] = MIN( z, sizes[Z]-2 );

    found = FALSE;

    initialize_voxel_flags( &voxels_searched, surface_extraction->min_limits,
                                              surface_extraction->max_limits );

    initialize_voxel_queue( &voxels_to_check );

    insert_in_voxel_queue( &voxels_to_check, insert );

    set_voxel_flag( &voxels_searched, surface_extraction->min_limits, insert );

    while( !found && voxels_remaining(&voxels_to_check) )
    {
        get_next_voxel_from_queue( &voxels_to_check, voxel );

        voxel_contains = voxel_contains_surface( volume, label_volume,
                                                 surface_extraction, voxel );

        voxel_done = (int) get_voxel_done_flag( surface_extraction->min_limits,
                                                surface_extraction->max_limits,
                                                voxel_done_flags, voxel );

        if( voxel_contains && voxel_done == 0 )
        {
            found_indices[X] = voxel[X];
            found_indices[Y] = voxel[Y];
            found_indices[Z] = voxel[Z];
            found = TRUE;
        }
        else if( voxel_contains || voxel_done == 0 )
        {
            add_voxel_neighbours( volume, label_volume, sizes,
                                  voxel[X], voxel[Y], voxel[Z],
                                  voxel_done, surface_extraction,
                                  &voxels_searched, &voxels_to_check );
        }
    }

    delete_voxel_flags( &voxels_searched );

    delete_voxel_queue( &voxels_to_check );

    return( found );
}

public  BOOLEAN  some_voxels_remaining_to_do(
    surface_extraction_struct   *surface_extraction )
{
    BOOLEAN   remaining_to_do;

    if( surface_extraction->voxellate_flag )
    {
        remaining_to_do = surface_extraction->min_changed_limits[X] <=
                          surface_extraction->max_changed_limits[X] ||
                          surface_extraction->min_modified[X] <=
                          surface_extraction->max_modified[X];
    }
    else
        remaining_to_do = voxels_remaining( &surface_extraction->voxels_to_do );

    return( remaining_to_do );
}

private  void  update_changed_limits(
    surface_extraction_struct  *ext )
{
    int   dim, min_range[N_DIMENSIONS], max_range[N_DIMENSIONS];

    if( ext->min_modified[X] > ext->max_modified[X] )
        return;

    for_less( dim, 0, N_DIMENSIONS )
    {
        min_range[dim] = MAX( ext->min_limits[dim], ext->min_modified[dim]-1 );
        max_range[dim] = MIN( ext->max_limits[dim], ext->max_modified[dim]+1 );
    }

    if( ext->min_changed_limits[X] > ext->max_changed_limits[X] )
    {
        for_less( dim, 0, N_DIMENSIONS )
        {
            ext->min_changed_limits[dim] = min_range[dim];
            ext->max_changed_limits[dim] = max_range[dim];
            ext->not_changed_since[dim] = min_range[dim];
            ext->current_voxel[dim] = min_range[dim];
        }
    }
    else
    {
        if( min_range[X] <= ext->current_voxel[X] )
        {
            if( max_range[X] >= ext->current_voxel[X] )
            {
                for_less( dim, 0, N_DIMENSIONS )
                    ext->not_changed_since[dim] = ext->current_voxel[dim];
            }
            else if( max_range[X] >= ext->not_changed_since[X] )
            {
                ext->not_changed_since[X] = max_range[X]+1;
                for_less( dim, 1, N_DIMENSIONS )
                    ext->not_changed_since[dim] = ext->min_modified[dim];
            }
        }

        for_less( dim, 0, N_DIMENSIONS )
        {
            if( min_range[dim] < ext->min_changed_limits[dim] )
                ext->min_changed_limits[dim] = min_range[dim];
            if( max_range[dim] > ext->max_changed_limits[dim] )
                ext->max_changed_limits[dim] = max_range[dim];
        }
    }

    ext->min_modified[X] = 0;
    ext->max_modified[X] = -1;
}

public  BOOLEAN  extract_more_surface(
    display_struct    *display )
{
    int                         dim, n_voxels_done, sizes[N_DIMENSIONS];
    int                         voxel_index[N_DIMENSIONS], last_changed_dim;
    surface_extraction_struct   *surface_extraction;
    Volume                      volume, label_volume;
    Real                        stop_time;
    BOOLEAN                     voxellate_flag, changed;

    changed = FALSE;

    n_voxels_done = 0;

    surface_extraction = &display->three_d.surface_extraction;
    volume = surface_extraction->volume;
    label_volume = surface_extraction->label_volume;
    voxellate_flag = surface_extraction->voxellate_flag;

    if( voxellate_flag )
        update_changed_limits( surface_extraction );

    get_volume_sizes( volume, sizes );

    stop_time = current_realtime_seconds() + Max_seconds_per_voxel_update;

    while( (n_voxels_done < Min_voxels_per_update ||
           (n_voxels_done < Max_voxels_per_update &&
           current_realtime_seconds() < stop_time) ) &&
           some_voxels_remaining_to_do( surface_extraction ) )
    {
        if( voxellate_flag )
        {
            voxel_index[X] = surface_extraction->current_voxel[X];
            voxel_index[Y] = surface_extraction->current_voxel[Y];
            voxel_index[Z] = surface_extraction->current_voxel[Z];
            dim = N_DIMENSIONS-1;
            while( dim >= 0 )
            {
                ++surface_extraction->current_voxel[dim];
                if( surface_extraction->current_voxel[dim] <=
                    surface_extraction->max_changed_limits[dim] )
                    break;

                surface_extraction->current_voxel[dim] =
                                 surface_extraction->min_changed_limits[dim];

                --dim;
            }

            last_changed_dim = dim;

            for_less( dim, 0, N_DIMENSIONS )
            {
                if( surface_extraction->current_voxel[dim] !=
                    surface_extraction->not_changed_since[dim] )
                    break;
            }

            if( dim == N_DIMENSIONS )
            {
                for_less( dim, 0, N_DIMENSIONS )
                {
                    surface_extraction->min_changed_limits[dim] = 0;
                    surface_extraction->max_changed_limits[dim] = -1;
                }
            }
            else if( last_changed_dim < 0 &&
                     surface_extraction->not_changed_since[X] <
                     surface_extraction->max_changed_limits[X] )
            {
                surface_extraction->max_changed_limits[X] =
                                surface_extraction->not_changed_since[X];

                if( surface_extraction->not_changed_since[Y] !=
                    surface_extraction->min_changed_limits[Y] ||
                    surface_extraction->not_changed_since[Z] !=
                    surface_extraction->min_changed_limits[Z] )
                {
                    ++surface_extraction->max_changed_limits[X];
                }

                for_less( dim, 0, N_DIMENSIONS )
                {
                    surface_extraction->not_changed_since[dim] =
                                surface_extraction->min_changed_limits[dim];
                }
            }
            else if( last_changed_dim == 0 &&
                     surface_extraction->current_voxel[X] ==
                     surface_extraction->min_changed_limits[X]+1 &&
                     surface_extraction->not_changed_since[X] ==
                     surface_extraction->min_changed_limits[X] &&
                     surface_extraction->not_changed_since[Y] ==
                     surface_extraction->min_changed_limits[Y] &&
                     surface_extraction->not_changed_since[Z] ==
                     surface_extraction->min_changed_limits[Z] )
            {
                ++surface_extraction->min_changed_limits[X];
                ++surface_extraction->not_changed_since[X];
            }
        }
        else
        {
            get_next_voxel_from_queue( &surface_extraction->voxels_to_do,
                                       voxel_index );
        }

        if( !voxellate_flag )
            reset_voxel_flag( &surface_extraction->voxel_state,
                              surface_extraction->min_limits,  voxel_index );

        if( extract_voxel_surface( volume, label_volume,
                                   surface_extraction, voxel_index,
                            surface_extraction->n_voxels_with_surface == 0) )
        {
            changed = TRUE;

            ++surface_extraction->n_voxels_with_surface;

            if( !voxellate_flag )
            {
                delete_edge_points_no_longer_needed( surface_extraction,
                                     volume, voxel_index,
                                     surface_extraction->voxel_done_flags,
                                     &surface_extraction->edge_points );

                add_voxel_neighbours( volume, label_volume, sizes,
                            voxel_index[X], voxel_index[Y], voxel_index[Z],
                            TRUE, surface_extraction,
                            &surface_extraction->voxel_state,
                            &surface_extraction->voxels_to_do );
            }
        }

        ++n_voxels_done;
    }

    if( !voxellate_flag &&
        !some_voxels_remaining_to_do( surface_extraction ) )
    {
        print( "Surface extraction finished\n" );
        stop_surface_extraction( display );
        update_all_menu_text( display );
    }

    return( changed );
}

public  void  tell_surface_extraction_range_of_labels_changed(
    display_struct    *display,
    int               volume_index,
    int               range[2][N_DIMENSIONS] )
{
    int                         dim;
    Volume                      label_volume;
    surface_extraction_struct   *surface_extraction;

    display = get_three_d_window( display );
    surface_extraction = &display->three_d.surface_extraction;

    label_volume = get_nth_label_volume( display, volume_index );

    if( surface_extraction->volume == label_volume ||
        surface_extraction->label_volume == label_volume )
    {
        if( surface_extraction->min_modified[X] >
            surface_extraction->max_modified[X] )
        {
            for_less( dim, 0, N_DIMENSIONS )
            {
                surface_extraction->min_modified[dim] = range[0][dim];
                surface_extraction->max_modified[dim] = range[1][dim];
            }
        }
        else
        {
            for_less( dim, 0, N_DIMENSIONS )
            {
                if( range[0][dim] < surface_extraction->min_modified[dim] )
                    surface_extraction->min_modified[dim] = range[0][dim];
                if( range[1][dim] > surface_extraction->max_modified[dim] )
                    surface_extraction->max_modified[dim] = range[1][dim];
            }
        }
    }
}

public  void  tell_surface_extraction_label_changed(
    display_struct    *display,
    int               volume_index,
    int               x,
    int               y,
    int               z )
{
    int   range[2][N_DIMENSIONS];

    range[0][X] = x;
    range[1][X] = x;
    range[0][Y] = y;
    range[1][Y] = y;
    range[0][Z] = z;
    range[1][Z] = z;

    tell_surface_extraction_range_of_labels_changed( display, volume_index,
                                                     range );
}

private  void  add_voxel_neighbours(
    Volume                          volume,
    Volume                          label_volume,
    int                             sizes[],
    int                             x,
    int                             y,
    int                             z,
    BOOLEAN                         surface_only,
    surface_extraction_struct       *surface_extraction,
    bitlist_3d_struct               *voxels_queued,
    voxel_queue_struct              *voxel_queue )
{
    int                      x_offset, y_offset, z_offset;
    int                      neighbour[N_DIMENSIONS];

    for_inclusive( x_offset, -1, 1 )
    {
        neighbour[X] = x + x_offset;

        for_inclusive( y_offset, -1, 1 )
        {
            neighbour[Y] = y + y_offset;
            for_inclusive( z_offset, -1, 1 )
            {
                neighbour[Z] = z + z_offset;
                if( (x_offset != 0 || y_offset != 0 || z_offset != 0) &&
                    surface_voxel_is_within_volume( surface_extraction,
                                                    neighbour ) &&
                    !get_voxel_flag( voxels_queued,
                                     surface_extraction->min_limits,
                                     neighbour ) &&
                    get_voxel_done_flag( surface_extraction->min_limits,
                                         surface_extraction->max_limits,
                                         surface_extraction->voxel_done_flags,
                                         neighbour ) != VOXEL_COMPLETELY_DONE )
                {
                    set_voxel_flag( voxels_queued,
                                    surface_extraction->min_limits, neighbour );
                    if( !surface_only ||
                        voxel_contains_surface( volume, label_volume,
                                                surface_extraction, neighbour ))
                    {
                        insert_in_voxel_queue( voxel_queue, neighbour );
                    }
                }
            }
        }
    }
}

private  void  delete_edge_points_no_longer_needed(
    surface_extraction_struct       *surface_extraction,
    Volume                          volume,
    int                             voxel_index[],
    unsigned_byte                   voxel_done_flags[],
    hash_table_struct               *edge_points )
{
    int                 axis_index, a1, a2;
    int                 x, y, dx, dy, dz;
    BOOLEAN             all_four_done;
    BOOLEAN             voxel_done[3][3][3];
    int                 indices[N_DIMENSIONS];
    int                 sizes[N_DIMENSIONS];

    get_volume_sizes( volume, sizes );

    for_inclusive( dx, -1, 1 )
    {
        indices[X] = voxel_index[X] + dx;
        for_inclusive( dy, -1, 1 )
        {
            indices[Y] = voxel_index[Y] + dy;
            for_inclusive( dz, -1, 1 )
            {
                indices[Z] = voxel_index[Z] + dz;

                if( !surface_voxel_is_within_volume( surface_extraction,
                                                     indices ) ||
                    get_voxel_done_flag( surface_extraction->min_limits,
                                         surface_extraction->max_limits,
                                         voxel_done_flags, indices )
                                              == VOXEL_COMPLETELY_DONE )
                {
                    voxel_done[dx+1][dy+1][dz+1] = TRUE;
                }
                else
                {
                    voxel_done[dx+1][dy+1][dz+1] = FALSE;
                }
            }
        }
    }

    for_less( axis_index, 0, N_DIMENSIONS )
    {
        a1 = (axis_index + 1) % N_DIMENSIONS;
        a2 = (axis_index + 2) % N_DIMENSIONS;

        for_less( x, 0, 2 )
        {
            for_less( y, 0, 2 )
            {
                all_four_done = TRUE;

                for_less( dx, 0, 2 )
                {
                    for_less( dy, 0, 2 )
                    {
                        indices[axis_index] = 1;
                        indices[a1] = x + dx;
                        indices[a2] = y + dy;

                        if( !voxel_done[indices[X]]
                                       [indices[Y]]
                                       [indices[Z]] )
                        {
                            all_four_done = FALSE;
                            break;
                        }
                    }
                }

                if( all_four_done )
                {
                    indices[axis_index] = voxel_index[axis_index];
                    indices[a1] = voxel_index[a1] + x;
                    indices[a2] = voxel_index[a2] + y;

                    remove_edge_point( sizes, edge_points,
                                       indices[X], indices[Y], indices[Z],
                                       axis_index );
                }
            }
        }
    }
}

public  void  remove_empty_polygons(
    polygons_struct  *polygons )
{
    int    new_n_polys, new_indices, size, v, poly, p1, p2, old_indices;
    int    *new_point_index, new_n_points, p;

    new_n_polys = 0;
    new_indices = 0;
    old_indices = 0;

    /*--- remove any polygons that have less than 3 vertices, or all vertices
          are the same */

    for_less( poly, 0, polygons->n_items )
    {
        size = GET_OBJECT_SIZE( *polygons, poly );

        for_less( v, 0, size-1 )
        {
            p1 = polygons->indices[old_indices+v];
            p2 = polygons->indices[old_indices+v+1];
            if( p1 != p2 )
                break;
        }

        /*--- if not degenerate polygon, keep it in the set */

        if( size >= 3 || v < size-1 )
        {
            for_less( v, 0, size )
            {
                polygons->indices[new_indices+v] =
                              polygons->indices[old_indices+v];
            }

            new_indices += size;
            polygons->end_indices[new_n_polys] = new_indices;
            ++new_n_polys;
        }

        old_indices += size;
    }

    polygons->n_items = new_n_polys;

    /*--- remove any unused vertices */

    ALLOC( new_point_index, polygons->n_points );
    for_less( p, 0, polygons->n_points )
        new_point_index[p] = FALSE;

    for_less( v, 0, new_indices )
        new_point_index[polygons->indices[v]] = TRUE;

    new_n_points = 0;
    for_less( p, 0, polygons->n_points )
    {
        if( new_point_index[p] )
        {
            new_point_index[p] = new_n_points;
            polygons->points[new_n_points] = polygons->points[p];
            polygons->normals[new_n_points] = polygons->normals[p];
            ++new_n_points;
        }
    }

    for_less( v, 0, new_indices )
        polygons->indices[v] = new_point_index[polygons->indices[v]];

    polygons->n_points = new_n_points;

    FREE( new_point_index );
}

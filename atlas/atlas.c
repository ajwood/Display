#include  <def_graphics.h>
#include  <def_globals.h>
#include  <def_files.h>
#include  <def_string.h>

public  void  initialize_atlas( atlas )
    atlas_struct   *atlas;
{
    atlas->input = FALSE;
    atlas->enabled = FALSE;
    atlas->opacity = Initial_atlas_opacity;
    atlas->transparent_threshold = Initial_atlas_transparent_threshold;
    atlas->slice_tolerance[X] = Initial_atlas_tolerance_x;
    atlas->slice_tolerance[Y] = Initial_atlas_tolerance_y;
    atlas->slice_tolerance[Z] = Initial_atlas_tolerance_z;
    atlas->flipped[X] = FALSE;
    atlas->flipped[Y] = FALSE;
    atlas->flipped[Z] = FALSE;
    atlas->slice_lookup[X] = (atlas_position_struct **) 0;
    atlas->slice_lookup[Y] = (atlas_position_struct **) 0;
    atlas->slice_lookup[Z] = (atlas_position_struct **) 0;
    atlas->n_pixel_maps = 0;
    atlas->n_pages = 0;
}

private  Status  input_atlas( atlas, filename )
    atlas_struct   *atlas;
    char           filename[];
{
    Status   status;
    Status   input_pixel_map();
    FILE     *file;
    String   *image_filenames, image_filename;
    char     axis_letter;
    Real     mm_position;
    int      i, axis_index;
    void     extract_directory();
    String   atlas_directory;

    extract_directory( filename, atlas_directory );

    status = open_file( filename, READ_FILE, ASCII_FORMAT, &file );

    if( status == OK )
    {
        atlas->n_pixel_maps = 0;
        atlas->n_pages = 0;
        image_filenames = (String *) 0;

        while( input_string( file, image_filename, MAX_STRING_LENGTH, ' ' )
                                                      == OK )
        {
            status = input_nonwhite_character( file, &axis_letter );

            if( status == OK && axis_letter >= 'x' && axis_letter <= 'z' )
                axis_index = axis_letter - 'x';
            else if( status == OK && axis_letter >= 'X' && axis_letter <= 'Z' )
                axis_index = axis_letter - 'X';
            else
                status = ERROR;

            if( status == OK )
                status = input_real( file, &mm_position );

            if( status == OK )
            {
                for_less( i, 0, atlas->n_pixel_maps )
                {
                    if( strcmp( image_filenames[i], image_filename ) == 0 )
                        break;
                }

                if( i == atlas->n_pixel_maps )
                {
                    SET_ARRAY_SIZE( status, image_filenames,
                                    atlas->n_pixel_maps, atlas->n_pixel_maps+1,
                                    DEFAULT_CHUNK_SIZE );
                    (void) strcpy( image_filenames[atlas->n_pixel_maps],
                                   image_filename );
                    SET_ARRAY_SIZE( status, atlas->pixel_maps,
                                    atlas->n_pixel_maps, atlas->n_pixel_maps+1,
                                    DEFAULT_CHUNK_SIZE );
                    status = input_pixel_map( atlas_directory, image_filename,
                                  &atlas->pixel_maps[atlas->n_pixel_maps] );
                    ++atlas->n_pixel_maps;
                }

                if( status == OK )
                {
                    SET_ARRAY_SIZE( status, atlas->pages,
                                    atlas->n_pages, atlas->n_pages+1,
                                    DEFAULT_CHUNK_SIZE );
                    atlas->pages[atlas->n_pages].axis = axis_index;
                    atlas->pages[atlas->n_pages].axis_position = mm_position;
                    atlas->pages[atlas->n_pages].n_resolutions = 0;
                    ADD_ELEMENT_TO_ARRAY( status,
                          atlas->pages[atlas->n_pages].n_resolutions,
                          atlas->pages[atlas->n_pages].pixel_map_indices,
                          i, DEFAULT_CHUNK_SIZE );
                    ++atlas->n_pages;
                }
            }
        }
    }

    if( status == OK )
        status = close_file( file );

    if( status == OK && atlas->n_pixel_maps > 0 )
        FREE( status, image_filenames );

    atlas->input = TRUE;

    if( status != OK )
        (void) printf( "Error inputting atlas.\n" );

    return( status );
}

private  Status  input_pixel_map( default_directory, image_filename, pixels )
    char           image_filename[];
    pixels_struct  *pixels;
{
    Status         status;
    Status         input_object_type();
    Status         io_pixels();
    String         absolute_filename;
    File_formats   format;
    object_types   object_type;
    Boolean        eof;
    FILE           *file;
    void           get_absolute_filename();

    get_absolute_filename( image_filename, default_directory,
                           absolute_filename );

    status = open_file( absolute_filename, READ_FILE, BINARY_FORMAT, &file );

    if( status == OK )
        status = input_object_type( file, &object_type, &format, &eof );

    if( status == OK && !eof && object_type == PIXELS )
        status = io_pixels( file, READ_FILE, format, pixels );
    else
        status = ERROR;

    if( status == OK && pixels->pixel_type != Pixel8 )
        status = ERROR;

    if( status == OK )
        status = close_file( file );

    return( status );
}

public  void  regenerate_atlas_lookup( slice_window )
    graphics_struct   *slice_window;
{
    Status                  status;
    volume_struct           *volume;
    atlas_struct            *atlas;
    int                     sizes[3], axis, i;
    void                    get_volume_size();
    atlas_position_struct   *get_closest_atlas_slice();

    (void) get_slice_window_volume( slice_window, &volume );
    get_volume_size( volume, &sizes[X], &sizes[Y], &sizes[Z] );
    atlas = &slice_window->slice.atlas;

    status = OK;

    for_less( axis, 0, 3 )
    {
        if( atlas->slice_lookup[axis] != (atlas_position_struct **) 0 )
            FREE( status, atlas->slice_lookup[axis] )

        if( status == OK )
            ALLOC( status, atlas->slice_lookup[axis], sizes[axis] );

        for_less( i, 0, sizes[axis] )
        {
             atlas->slice_lookup[axis][i] = get_closest_atlas_slice(
                                              i, sizes, axis, atlas );
        }
    }
}

private  atlas_position_struct  *get_closest_atlas_slice( voxel_index,
                                                          sizes, axis, atlas )
    int           voxel_index;
    int           sizes[3];
    int           axis;
    atlas_struct  *atlas;
{
    int                     i;
    Real                    min_dist, dist;
    atlas_position_struct   *closest_so_far;
    Real                    get_distance_from_voxel();

    closest_so_far = (atlas_position_struct *) 0;
    min_dist = 0.0;

    for_less( i, 0, atlas->n_pages )
    {
        if( atlas->pages[i].axis == axis )
        {
            dist = get_distance_from_voxel( voxel_index, sizes, axis,
                                            atlas->pages[i].axis_position );

            if( dist <= atlas->slice_tolerance[axis] &&
                (closest_so_far == (atlas_position_struct *) 0 ||
                 dist < min_dist) )
            {
                closest_so_far = &atlas->pages[i];
                min_dist = dist;
            }
        }
    }

    return( closest_so_far );
}

private  Real  get_distance_from_voxel( voxel_index, sizes, axis_index,
                                        mm_coordinate )
    int   voxel_index;
    int   sizes[3];
    int   axis_index;
    Real  mm_coordinate;
{
    Real   voxel_indices[3], tal_voxel[3], tal_mm[3];
    Real   distance;
    void   convert_voxel_to_talairach();
    void   convert_talairach_to_mm();

    voxel_indices[X] = 0;
    voxel_indices[Y] = 0;
    voxel_indices[Z] = 0;
    voxel_indices[axis_index] = (Real) voxel_index;

    convert_voxel_to_talairach( (Real) voxel_indices[X],
                                (Real) voxel_indices[Y],
                                (Real) voxel_indices[Z],
                                sizes[X], sizes[Y], sizes[Z],
                                &tal_voxel[X], &tal_voxel[Y], &tal_voxel[Z] );

    convert_talairach_to_mm( tal_voxel[X], tal_voxel[Y], tal_voxel[Z],
                             &tal_mm[X], &tal_mm[Y], &tal_mm[Z] );

    distance = ABS( tal_mm[axis_index] - mm_coordinate );

    return( distance );
}

public  void  set_atlas_state( slice_window, state )
    graphics_struct   *slice_window;
    Boolean           state;
{
    Status   status;

    status = OK;

    if( state && !slice_window->slice.atlas.input )
    {
        PRINT( "Inputting atlas..." );
        (void) flush_file( stdout );
        status = input_atlas( &slice_window->slice.atlas, Atlas_filename );

        if( status == OK )
            regenerate_atlas_lookup( slice_window );

        (void) printf( "done\n" );
    }

    slice_window->slice.atlas.enabled = state;
}

public  void  blend_in_atlas( atlas, image, image_x_size, image_y_size,
                  voxel_start_indices,
                  a1, a2, axis_index, dx, dy, x_volume_size, y_volume_size )
    atlas_struct  *atlas;
    Pixel_colour  image[];
    int           image_x_size;
    int           image_y_size;
    int           voxel_start_indices[3];
    int           a1;
    int           a2;
    int           axis_index;
    Real          dx;
    Real          dy;
    int           x_volume_size;
    int           y_volume_size;
{
    int            x, y, atlas_x_size, atlas_y_size;
    int            atlas_multiplier_x, atlas_multiplier_y;
    int            x_pixel, y_pixel, x_pixel_start, y_pixel_start;
    int            r_atlas, g_atlas, b_atlas, r_voxel, g_voxel, b_voxel;
    int            r, g, b, transparent_threshold;
    Pixel_colour   voxel_pixel, atlas_pixel, *lookup, *pixels;
    unsigned  char *atlas_image;
    Pixel_colour   *get_pixels8_lookup_table();
    Real           opacity;

    if( !atlas->enabled || atlas->opacity <= 0.0 ||
        atlas->slice_lookup[axis_index] == (atlas_position_struct **) 0 ||
        atlas->slice_lookup[axis_index]
            [voxel_start_indices[axis_index]] == (atlas_position_struct *) 0 ||
        !find_appropriate_atlas_image( atlas->pixel_maps,
                      atlas->slice_lookup[axis_index]
                      [voxel_start_indices[axis_index]],
                      ROUND( x_volume_size / dx),
                      ROUND( y_volume_size / dy), 
                      &atlas_image, &atlas_x_size, &atlas_y_size,
                      &atlas_multiplier_x, &atlas_multiplier_y ) )
    {
        return;
    }

    opacity = atlas->opacity;
    transparent_threshold = atlas->transparent_threshold;
    lookup = get_pixels8_lookup_table();

    x_pixel_start = voxel_start_indices[a1] / dx / atlas_multiplier_x;
    y_pixel_start = voxel_start_indices[a2] / dy / atlas_multiplier_y;

    for_less( y, 0, image_y_size )
    {
        pixels = &image[IJ(y,0,image_x_size)];

        y_pixel = y_pixel_start + y / atlas_multiplier_y;

        if( y_pixel >= 0 && y_pixel < atlas_y_size )
        {
            for_less( x, 0, image_x_size )
            {
                x_pixel = x_pixel_start + x / atlas_multiplier_x;

                if( x_pixel >= 0 && x_pixel < atlas_x_size )
                {
                    atlas_pixel = lookup[
                      atlas_image[IJ(y_pixel,x_pixel,atlas_x_size)]];
                    r_atlas = Pixel_colour_r(atlas_pixel);
                    g_atlas = Pixel_colour_g(atlas_pixel);
                    b_atlas = Pixel_colour_b(atlas_pixel);

                    if( r_atlas <= transparent_threshold ||
                        g_atlas <= transparent_threshold ||
                        b_atlas <= transparent_threshold )
                    {
                        voxel_pixel = *pixels;
                        r_voxel = Pixel_colour_r(voxel_pixel);
                        g_voxel = Pixel_colour_g(voxel_pixel);
                        b_voxel = Pixel_colour_b(voxel_pixel);

                        r = ROUND( r_voxel + (r_atlas - r_voxel) * opacity );
                        g = ROUND( g_voxel + (g_atlas - g_voxel) * opacity );
                        b = ROUND( b_voxel + (b_atlas - b_voxel) * opacity );
                        *pixels = RGB_255_TO_PIXEL( r, g, b );
                    }
                }

                ++pixels;
            }
        }
    }
}

private  Boolean  find_appropriate_atlas_image( atlas_images, atlas_page,
                      x_volume_size, y_volume_size,
                      atlas_image, atlas_x_size, atlas_y_size,
                      atlas_multiplier_x, atlas_multiplier_y )
    pixels_struct           *atlas_images;
    atlas_position_struct   *atlas_page;
    int                     x_volume_size;
    int                     y_volume_size;
    unsigned char           *atlas_image[];
    int                     *atlas_x_size;
    int                     *atlas_y_size;
    int                     *atlas_multiplier_x;
    int                     *atlas_multiplier_y;
{
    int             i, image_index;
    pixels_struct   *pixels;

    image_index = -1;

    for_less( i, 0, atlas_page->n_resolutions )
    {
        pixels = &atlas_images[atlas_page->pixel_map_indices[i]];

        if( pixels->x_max < x_volume_size &&
            pixels->y_max < y_volume_size &&
            (image_index < 0 || pixels->x_max > *atlas_x_size ||
             pixels->y_max > *atlas_y_size) )
        {
            image_index = i;
            *atlas_x_size = pixels->x_max+1;
            *atlas_y_size = pixels->y_max+1;
        }
    }

    if( image_index >= 0 )
    {
        *atlas_image =
              atlas_images[atlas_page->pixel_map_indices[image_index]].
                           data.pixels8;
        *atlas_multiplier_x = x_volume_size / *atlas_x_size;
        *atlas_multiplier_y = y_volume_size / *atlas_y_size;
    }

    return( image_index >= 0 );
}
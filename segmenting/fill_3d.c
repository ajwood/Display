#include  <def_objects.h>
#include  <def_queue.h>
#include  <def_bitlist.h>
#include  <def_files.h>

typedef struct
{
    int  x, y, z;
} xyz_struct;

public  Status  fill_connected_voxels_3d( volume, x, y, z,
                                          max_threshold )
    volume_struct   *volume;
    int             x, y, z;
    int             max_threshold;
{
    Status                       status;
    int                          tx, ty, tz, dx, dy, dz;
    int                          nx, ny, nz;
    void                         get_volume_size();
    void                         set_voxel_label_flag();
    xyz_struct                   entry;
    QUEUE_STRUCT( xyz_struct )   queue;
    bitlist_3d_struct            bitlist;
    int                          n_done, val;
    const  Real                  update_every = 10.0;
    Real                         next_message_time;
    Real                         current_realtime_seconds();

    get_volume_size( volume, &nx, &ny, &nz );

    status = create_bitlist_3d( nx, ny, nz, &bitlist );

    if( status == OK )
    {
        INITIALIZE_QUEUE( queue );

        set_bitlist_bit_3d( &bitlist, x, y, z, TRUE );
        if( get_voxel_activity_flag( volume, x, y, z ) )
        {
            GET_VOLUME_DATA( val, *volume, x, y, z );
            if( val >= max_threshold )
            {
                set_voxel_label_flag( volume, x, y, z, TRUE );

                entry.x = x;
                entry.y = y;
                entry.z = z;
                INSERT_IN_QUEUE( status, queue, entry );
            }
        }
    }

    n_done = 0;
    next_message_time = current_realtime_seconds() + update_every;

    while( !IS_QUEUE_EMPTY( queue ) )
    {
        REMOVE_FROM_QUEUE( queue, entry );

        x = entry.x;
        y = entry.y;
        z = entry.z;

        for_inclusive( dx, -1, 1 )
        for_inclusive( dy, -1, 1 )
        for_inclusive( dz, -1, 1 )
        if( dx != 0 || dy != 0 || dz != 0 )
        {
            tx = x + dx;
            ty = y + dy;
            tz = z + dz;

            if( tx >= 0 && tx < nx &&
                ty >= 0 && ty < ny &&
                tz >= 0 && tz < nz &&
                !get_bitlist_bit_3d( &bitlist, tx, ty, tz ) )
            {
                set_bitlist_bit_3d( &bitlist, tx, ty, tz, TRUE );

                if( get_voxel_activity_flag( volume, tx, ty, tz ) )
                {
                    GET_VOLUME_DATA( val, *volume, tx, ty, tz );
                    if( val >= max_threshold )
                    {
                        set_voxel_label_flag( volume, tx, ty, tz, TRUE );
                        entry.x = tx;
                        entry.y = ty;
                        entry.z = tz;
                        if( status == OK )
                            INSERT_IN_QUEUE( status, queue, entry );
                    }
                }
            }
        }

        if( (++n_done % 20) == 0 &&
            current_realtime_seconds() > next_message_time )
        {
            next_message_time = current_realtime_seconds() + update_every;
            PRINT( "N in queue = %d\n", NUMBER_IN_QUEUE(queue) );
        }
    }

    if( status == OK )
        status = delete_bitlist_3d( &bitlist );

    if( status == OK )
        DELETE_QUEUE( status, queue );

    return( status );
}

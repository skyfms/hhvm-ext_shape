<?hh

/*** Standard functions from PECL shape extension */

<<__Native>> function shp_open (string $filename, string $access): mixed;
<<__Native>> function shp_create (string $filename, string $access): mixed;
<<__Native>> function shp_close (resource $shp): bool;
<<__Native>> function shp_read_object (resource $shp, int $ord): mixed;
<<__Native>> function shp_destroy_object (resource $shp_object): bool;
<<__Native>> function shp_rewind_object (resource $shp_handle, resource $shp_object): bool;
<<__Native>> function shp_write_object (resource $shp_handle, int $entity_num, resource $shp_object): mixed;
<<__Native>> function shp_get_info (resource $shp_handle): mixed;
<<__Native>> function shp_compute_extents (resource $shp_object): bool;

// Workaround for HHVM extension limitation of maximum 6 arguments.
function shp_create_object ()
{
    $args = func_get_args();
    return shp_create_object_impl($args);
}
<<__Native>> function shp_create_object_impl (array $args): mixed;

// PECL have varargs here, so we aren't 100% compatible. But in correct code
// you want to pass all of them anyway, else function returns false in PHP.
<<__Native>> function shp_create_simple_object (
    int $shp_type, int $vertices_num, array $padfX, array $padfY, array $padfZ
): mixed;

/*** Extension */

// Returns PHP array with SHP object properties or FALSE.
<<__Native>> function shp_get_array_from_object (resource $shp_object): mixed;


/* ----------------------------------------------------------------
 **
 ** kernel:  clear_buffer
 **
 ** Purpose:
 **
 ** input:
 **
 ** output: 
 **
 ** ----------------------------------------------------------------
 */

__kernel void clear_buffer(
    __global int* cm_row)
{
  int i = get_global_id(0);
  cm_row[i]= 0;
}

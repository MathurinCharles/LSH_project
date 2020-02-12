/* ----------------------------------------------------------------
 **
 ** kernel:  compute_cm
 **
 ** Purpose: Compute the rows of the characteristic matrix
 **
 ** input: doc a vector of chars (the document)
 **
 ** output: A vector of int (0 or 1) indicating is the shingle is in the document
 **
 ** ----------------------------------------------------------------
 */

__kernel void compute_cm(
    __global char* document,
    __global int* cm_row,
    const unsigned int kshingsize,
    const unsigned int k)
{
  int i = get_global_id(0);
  int hash = 0;
  for (int j = 0; j < k; j++) {
      hash = 255*hash + (int)document[i+j];
  }
  cm_row[hash%kshingsize]= 1;
}

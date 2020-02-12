/* ----------------------------------------------------------------
 **
 ** kernel:  write_sim
 **
 ** Purpose: Compute the Jaccard similarity between two documents
 **
 ** input: the bool* of the 2 documents
 **
 ** output: The cardinal of the intersection and union
 **
 ** ----------------------------------------------------------------
 */

__kernel void write_sim(
    __global int* doc1,
    __global int* doc2,
    __global int* inter_union)
{
  int i = get_global_id(0);
  int prod = doc1[i]*doc2[i];
  int sum = doc1[i]+doc2[i];
  inter_union[0] = inter_union[0] + prod;
  inter_union[1] = inter_union[1] + sum - prod;
}

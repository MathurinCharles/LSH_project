/* ----------------------------------------------------------------
 **
 ** kernel:  generate_shingles
 **
 ** Purpose: Generates the k-shingles from a document
 **
 ** input: doc a vector of chars (the document)
 **
 ** output: A vector of the k-shingles generated
 **
 ** ----------------------------------------------------------------
 */

__kernel void generate_shingles(
    __global char* doc,
    __global char*kshings,
    const unsigned int k)
{
  int i = get_global_id(0);
  char* kshingle(k);
  for (int j = 0; j < k; j++) {
      kshingle[i+j] = doc[i+j];
  }
  kshings[i] = kshingle

}

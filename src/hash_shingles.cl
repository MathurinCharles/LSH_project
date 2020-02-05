/* ----------------------------------------------------------------
 **
 ** kernel:  hash_shingles
 **
 ** Purpose: Hash a shingle
 **
 ** input: A vector of shingle (vector of strings of length k)
 **
 ** output: A map of the hash values of the shingles (the key and the value are equal to the hash value % the biggest size of all vector of shingles)
 **
 ** ----------------------------------------------------------------
 */

__kernel void jsim(
    __global vector<string> kshing,
    __global map<int, int> bucket,
    const hash<string> h,
    const unsigned int kshing_size,
    const unsigned int kshing_size_max)
{
  int i = get_global_id(0);
  if(i < kshing_size)  {
    bucket[h(kshings[i]) % kshing_size_max] = h(kshings[i]) % kshing_size_max;
  }
}

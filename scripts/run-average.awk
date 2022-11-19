#!/usr/bin/awk -f
# Copied from https://pmitev.github.io/to-awk-or-not/More_awk/Running_average/
# Thanks!
{
  if (!col)  col = 1
  if (!size) size= 5
  mod= NR%size; 
  if(NR <= size){count++}
  else{ sum-= array[mod] };
  sum+= $(col); array[mod]= $(col);
  print sum/count
}

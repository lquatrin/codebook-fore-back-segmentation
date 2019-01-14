#include "codeword.h"

CodeWord::CodeWord ()
{
  for (int i = 0; i < MAX_SIZE_ATRIBUTES; i++)
  {
    attributes[i] = 0.f;
  }
  for (int i = 0; i < MAX_SIZE_TUPLE; i++)
  {
    tuple[i] = 0.f;
  }
}

void CodeWord::SetAttributes (float c[], int n)
{
  for (int i = 0; i < MAX_SIZE_ATRIBUTES && i < n; i++)
  {
    attributes[i] = c[i];
  }
}

void CodeWord::SetTuple (float t[], int n)
{
  for (int i = 0; i < MAX_SIZE_TUPLE && i < n; i++)
  {
    tuple[i] = t[i];
  }
}

void CodeWord::SetAttributesValueAt (float value, int index)
{
  attributes[index] = value;
}

void CodeWord::SetTupleValueAt (float value, int index)
{
  tuple[index] = value;
}

float* CodeWord::GetAttributes ()
{
  return attributes;
}

float CodeWord::GetAttributesValueAt (int index)
{
  return attributes[index];
}

float* CodeWord::GetTuple ()
{
  return tuple;
}

float CodeWord::GetTupleValueAt (int index)
{
  return tuple[index];
}


#ifndef CODEWORD_H
#define CODEWORD_H

#define MAX_SIZE_ATRIBUTES 3
#define MAX_SIZE_TUPLE 6

struct CodeWord
{
public:
  CodeWord ();

  void SetAttributes (float c[], int n);
  void SetAttributesValueAt (float value, int index);
  void SetTuple (float t[], int n);
  void SetTupleValueAt (float value, int index);
  float* GetAttributes ();
  float GetAttributesValueAt (int index);
  float* GetTuple ();
  float GetTupleValueAt (int index);

  float attributes[MAX_SIZE_ATRIBUTES];
  float tuple[MAX_SIZE_TUPLE];
};

#endif
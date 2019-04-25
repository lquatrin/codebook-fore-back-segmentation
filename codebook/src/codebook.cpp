#include "codebook.h"

#include <algorithm>

#define RGB_ALFA 0.5f //0.4 ~ 0.7
#define RGB_BETA 1.3f //1.1 ~ 1.5

#include "CIEDE2000.h"

CodeBook::CodeBook (int w, int h)
{
  codewords.clear();

  col = w;
  row = h;
}

CodeBook::~CodeBook () {}

int CodeBook::SearchCodeWord (float values[], float thresholds[])
{
  // Para cada CodeWord dentro do CodeBook
  // selecionado:
  for(int i = 0 ; i < (int)codewords.size() ; i++)
  {
    // Cálcula se o píxel de entrada corresponde
    // ao CodeWord selecionado. Caso afirmativo,
    // retorna o index deste CodeWord
    if(IsBackground(values, thresholds, i))
    {
      CodeWord aux = codewords[i];
      codewords[i] = codewords[0];
      codewords[0] = aux;
      return 0;
    }
  }
  return -1;
}

///////////////////////////////////////////////////////////////////
// ColorSpace RGB
CodeBookRGB::CodeBookRGB (int w, int h)
  : CodeBook(w, h)
{

}

CodeBookRGB::~CodeBookRGB ()
{

}

void CodeBookRGB::UpdateMatchedCodeWord (float c[])
{
  float* tuple = codewords[0].GetTuple();
  float* colors = codewords[0].GetAttributes();
  colors[0] = ((tuple[2] * colors[0]) + c[0]) / (tuple[2] + 1);
  colors[1] = ((tuple[2] * colors[1]) + c[1]) / (tuple[2] + 1);
  colors[2] = ((tuple[2] * colors[2]) + c[2]) / (tuple[2] + 1);
  codewords[0].SetAttributes(colors, 3);

  float I = sqrt(pow(c[0], 2) + pow(c[1], 2) + pow(c[2], 2));
  tuple[0] = std::min(I, tuple[0]);
  tuple[1] = std::max(I, tuple[1]);
  if ((int)tuple[2] < 2147483647)  tuple[2] = tuple[2] + 1;

  codewords[0].SetTupleValueAt(tuple[0], 0);
  codewords[0].SetTupleValueAt(tuple[1], 1);
  codewords[0].SetTupleValueAt(tuple[2], 2);
}

bool CodeBookRGB::IsBackground (float c[], float thresholds[], int i)
{
  float I = sqrt(pow(c[0], 2) + pow(c[1], 2) + pow(c[2], 2));
  return Colordist(c, codewords[i].attributes) <= thresholds[0] &&
    Brightness(I, codewords[i].GetTupleValueAt(0), codewords[i].GetTupleValueAt(1));
}

void CodeBookRGB::WrapAroundLambda (float N)
{
  int sizeTuple = 6;
  for (int i = 0; i < (int)codewords.size(); i++)
  {
    float lambdaResult =
      std::max(codewords[i].tuple[sizeTuple - 3], (N - codewords[i].tuple[sizeTuple - 1] +
        codewords[i].tuple[sizeTuple - 2] - 1));
    codewords[i].SetTupleValueAt(lambdaResult, sizeTuple - 3);
  }
}

void CodeBookRGB::RemoveUnderutilizedCodeWords (int N)
{
  int sizeTuple = 6;
  int T = N / 2;
  for (int i = 0; i < (int)codewords.size();)
  {
    if (codewords[i].tuple[sizeTuple - 3] > T) { codewords.erase(codewords.begin() + i); }
    else { i++; }
  }
}

bool CodeBookRGB::Brightness (float I, float Imin, float Imax)
{
  float Ilow = RGB_ALFA * Imax;
  float Ihi = std::min(RGB_BETA*Imax, Imin / RGB_ALFA);

  return (Ilow <= I && I <= Ihi);
}

float CodeBookRGB::Colordist (float c[], float colors[])
{
  float xt2 = pow(c[0], 2) + pow(c[1], 2) + pow(c[2], 2);
  float vm2 = pow(colors[0], 2) + pow(colors[1], 2) + pow(colors[2], 2);
  float xt_vi2 = pow(c[0] * colors[0] + c[1] * colors[1] + c[2] * colors[2], 2);

  float p2 = xt_vi2 / vm2;
  return (sqrt(xt2 - p2));
}

///////////////////////////////////////////////////////////////////
// ColorSpace LAB
CodeBookLAB::CodeBookLAB (int w, int h)
  : CodeBook(w, h)
{

}

CodeBookLAB::~CodeBookLAB ()
{

}

void CodeBookLAB::UpdateMatchedCodeWord (float c[])
{
  float* tuple = codewords[0].tuple;
  float* atributes = codewords[0].attributes;

  /*
  atributes[0] = ((tuple[0]*atributes[0])+c[0])/(tuple[0]+1);
  atributes[1] = ((tuple[0]*atributes[1])+c[1])/(tuple[0]+1);
  atributes[2] = ((tuple[0]*atributes[2])+c[2])/(tuple[0]+1);
  codewords[0].atributes[0] = atributes[0];
  codewords[0].atributes[1] = atributes[1];
  codewords[0].atributes[2] = atributes[2];
  */

  if ((int)tuple[0] < 2147483647)  tuple[0] = tuple[0] + 1;
  codewords[0].tuple[0] = tuple[0];
}

bool CodeBookLAB::IsBackground (float c[], float thresholds[], int i)
{
  return CIEDE2000(c[0], c[1], c[2], codewords[i].attributes[0],
    codewords[i].attributes[1], codewords[i].attributes[2]) < thresholds[0];
}

void CodeBookLAB::WrapAroundLambda (float N)
{
  int sizeTuple = 4;
  for (int i = 0; i < (int)codewords.size(); i++)
  {
    float lambdaResult =
      std::max(codewords[i].tuple[sizeTuple - 3], (N - codewords[i].tuple[sizeTuple - 1] +
        codewords[i].tuple[sizeTuple - 2] - 1));
    codewords[i].SetTupleValueAt(lambdaResult, sizeTuple - 3);
  }
}

void CodeBookLAB::RemoveUnderutilizedCodeWords (int N)
{
  int sizeTuple = 4;
  int T = N / 2;
  for (int i = 0; i < (int)codewords.size();)
  {
    if (codewords[i].tuple[sizeTuple - 3] > T) { codewords.erase(codewords.begin() + i); }
    else { i++; }
  }
}

///////////////////////////////////////////////////////////////////
// ColorSpace YCrCb
CodeBookYCrCb::CodeBookYCrCb (int w, int h)
  : CodeBook(w, h)
{

}

CodeBookYCrCb::~CodeBookYCrCb ()
{

}

void CodeBookYCrCb::UpdateMatchedCodeWord (float c[])
{
  float* tuple = codewords[0].tuple;
  float* ycrcb = codewords[0].attributes;
  ycrcb[0] = ((tuple[0] * ycrcb[0]) + c[0]) / (tuple[0] + 1);
  ycrcb[1] = ((tuple[0] * ycrcb[1]) + c[1]) / (tuple[0] + 1);
  ycrcb[2] = ((tuple[0] * ycrcb[2]) + c[2]) / (tuple[0] + 1);
 
  codewords[0].attributes[0] = ycrcb[0];
  codewords[0].attributes[1] = ycrcb[1];
  codewords[0].attributes[2] = ycrcb[2];

  if ((int)tuple[0] < 2147483647)  tuple[0] = tuple[0] + 1;
  codewords[0].tuple[0] = tuple[0];
}

bool CodeBookYCrCb::IsBackground (float colors[], float thresholds[], int indexToCodeWord)
{
  return (CrominanceDistance(colors, indexToCodeWord) < thresholds[0] &&
          LuminanceDistance(colors, indexToCodeWord) < thresholds[1]);

}

void CodeBookYCrCb::WrapAroundLambda (float N)
{
  int sizeTuple = 4;
  for (int i = 0; i < (int)codewords.size(); i++)
  {
    float lambdaResult =
      std::max(codewords[i].tuple[sizeTuple - 3], (N - codewords[i].tuple[sizeTuple - 1] +
        codewords[i].tuple[sizeTuple - 2] - 1));
    codewords[i].SetTupleValueAt(lambdaResult, sizeTuple - 3);
  }
}

void CodeBookYCrCb::RemoveUnderutilizedCodeWords (int N)
{
  int sizeTuple = 4;
  int T = N / 2;
  for (int i = 0; i < (int)codewords.size();)
  {
    if (codewords[i].tuple[sizeTuple - 3] > T) { codewords.erase(codewords.begin() + i); }
    else { i++; }
  }
}

float CodeBookYCrCb::LuminanceDistance (float colors[], int indexToCodeWord)
{
  return abs(colors[0] - codewords[indexToCodeWord].attributes[0]);
}

float CodeBookYCrCb::CrominanceDistance (float colors[], int indexToCodeWord)
{
  return sqrt(pow(colors[1] - codewords[indexToCodeWord].attributes[1], 2) 
            + pow(colors[2] - codewords[indexToCodeWord].attributes[2], 2));
}

///////////////////////////////////////////////////////////////////
// ColorSpace HSV
CodeBookHSV::CodeBookHSV (int w, int h)
  : CodeBook(w, h)
{

}

CodeBookHSV::~CodeBookHSV ()
{

}

void CodeBookHSV::UpdateMatchedCodeWord (float c[])
{
  /*float* tuple = codewords[0].tuple;
  float* hsv = codewords[0].atributes;
  hsv[0] = ((tuple[0]*hsv[0])+c[0])/(tuple[0]+1);
  hsv[1] = ((tuple[0]*hsv[1])+c[1])/(tuple[0]+1);
  hsv[2] = ((tuple[0]*hsv[2])+c[2])/(tuple[0]+1);
  codewords[0].atributes[0] = hsv[0];
  codewords[0].atributes[1] = hsv[1];
  codewords[0].atributes[2] = hsv[2];

  if((int)tuple[0] < 2147483647)  tuple[0] = tuple[0] + 1 ;
  codewords[0].tuple[0] = tuple[0];*/
}

bool CodeBookHSV::IsBackground (float colors[], float thresholds[], int indexToCodeWord)
{
  return (ColorDistortion(colors, indexToCodeWord) < thresholds[0] 
    && ValueDistortion(colors, indexToCodeWord) < thresholds[1]);
}

void CodeBookHSV::WrapAroundLambda (float N)
{
  int sizeTuple = 4;
  for (int i = 0; i < (int)codewords.size(); i++)
  {
    float lambdaResult =
      std::max(codewords[i].tuple[sizeTuple - 3], (N - codewords[i].tuple[sizeTuple - 1] +
        codewords[i].tuple[sizeTuple - 2] - 1));
    codewords[i].SetTupleValueAt(lambdaResult, sizeTuple - 3);
  }
}

void CodeBookHSV::RemoveUnderutilizedCodeWords (int N)
{
  int sizeTuple = 4;
  int T = N / 2;
  for (int i = 0; i < (int)codewords.size();)
  {
    if (codewords[i].tuple[sizeTuple - 3] > T) { codewords.erase(codewords.begin() + i); }
    else { i++; }
  }
}

float CodeBookHSV::ColorDistortion (float colors[], int indexToCodeWord)
{
  float x1 = colors[1] * cos(2.f * (float)M_PI * colors[0]);
  float y1 = colors[1] * sin(2.f * (float)M_PI * colors[0]);
  float x2 = codewords[indexToCodeWord].attributes[1] 
    * cos(2.f * (float)M_PI * codewords[indexToCodeWord].attributes[0]);
  float y2 = codewords[indexToCodeWord].attributes[1] 
    * sin(2.f * (float)M_PI * codewords[indexToCodeWord].attributes[0]);

  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

float CodeBookHSV::ValueDistortion (float colors[], int indexToCodeWord)
{
  return abs(codewords[indexToCodeWord].attributes[2] - colors[2]);
}


///////////////////////////////////////////////////////////////////
// ColorSpace HLS
CodeBookHLS::CodeBookHLS (int w, int h)
  : CodeBook(w, h)
{

}

CodeBookHLS::~CodeBookHLS ()
{

}

void CodeBookHLS::UpdateMatchedCodeWord (float c[])
{
  float* tuple = codewords[0].tuple;
  float* hls = codewords[0].attributes;
  hls[0] = ((tuple[0] * hls[0]) + c[0]) / (tuple[0] + 1);
  hls[1] = ((tuple[0] * hls[1]) + c[1]) / (tuple[0] + 1);
  hls[2] = ((tuple[0] * hls[2]) + c[2]) / (tuple[0] + 1);
  codewords[0].attributes[0] = hls[0];
  codewords[0].attributes[1] = hls[1];
  codewords[0].attributes[2] = hls[2];

  if ((int)tuple[0] < 2147483647)  tuple[0] = tuple[0] + 1;

  codewords[0].tuple[0] = tuple[0];
}

// h = 0~360, l = 0~256, s = 0~256
bool CodeBookHLS::IsBackground (float colors[], float thresholds[], int indexToCodeWord)
{
  return HLSDistorted(colors, indexToCodeWord) < thresholds[0];
}

void CodeBookHLS::WrapAroundLambda (float N)
{
  int sizeTuple = 4;
  for (int i = 0; i < (int)codewords.size(); i++)
  {
    float lambdaResult =
      std::max(codewords[i].tuple[sizeTuple - 3], (N - codewords[i].tuple[sizeTuple - 1] +
        codewords[i].tuple[sizeTuple - 2] - 1));
    codewords[i].SetTupleValueAt(lambdaResult, sizeTuple - 3);
  }
}

void CodeBookHLS::RemoveUnderutilizedCodeWords (int N)
{
  int sizeTuple = 4;
  int T = N / 2;
  for (int i = 0; i < (int)codewords.size();)
  {
    if (codewords[i].tuple[sizeTuple - 3] > T) { codewords.erase(codewords.begin() + i); }
    else { i++; }
  }
}

float CodeBookHLS::HLSDistorted (float values[], int indexToCodeWord)
{
  float l_min = std::min(128 - abs(128 - values[2]), 128 - abs(128 - codewords[indexToCodeWord].attributes[2])) / 128;
  float s_min = std::min(values[1], codewords[indexToCodeWord].attributes[1]) / 256;

  float d_h = std::min(abs(values[0] - codewords[indexToCodeWord].attributes[0]), 360 - abs(values[0] - codewords[indexToCodeWord].attributes[0])) * l_min * s_min;
  float d_l = values[1] - codewords[indexToCodeWord].attributes[1];
  float d_s = (values[2] - codewords[indexToCodeWord].attributes[2]) * s_min;

  /***Constants***/
  float alfa = 1.f;
  float beta = 1.f;
  float gama = 1.f;
  /***Constants***/

  return sqrt(pow(d_h*alfa, 2) + pow(d_l*beta, 2) + pow(d_s*gama, 2));
}
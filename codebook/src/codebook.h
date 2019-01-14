#ifndef CODEBOOK_H
#define CODEBOOK_H


#include "codeword.h"
#include <vector>

#include "opencv2/opencv.hpp"

enum CodeBookColorSpace {
  RGB = 0,
  LAB = 1,
  /*
  cvCvtColor(src,dst,code); // src -> dst
  code    = CV_<X>2<Y>
  <X>/<Y> = RGB, BGR, GRAY, HSV, YCrCb, XYZ, Lab, Luv, HLS

  cvCvtColor(cimg,gimg,CV_BGR2GRAY); // cimg -> gimg

  YCrCb -> BGR
  cvtColor(frame,frame, CV_YCrCb2BGR);

  BGR -> YCrCb
  cvtColor(frame,frame, CV_BGR2YCrCb);

  Y -> 0 - 255
  Cr -> 0 - 255 (para -1 a 1)
  Cb -> 0 - 255 (para -1 a 1)
  */
  YCrCb = 2,
  /*
  cvCvtColor(src,dst,code); // src -> dst
  code    = CV_<X>2<Y>
  <X>/<Y> = RGB, BGR, GRAY, HSV, YCrCb, XYZ, Lab, Luv, HLS
  
  cvCvtColor(cimg,gimg,CV_BGR2GRAY); // cimg -> gimg
  
  HSV -> BGR
  cvtColor(frame,frame, CV_HSV2BGR);
  
  BGR -> HSV
  cvtColor(frame,frame, CV_BGR2HSV);
  
  H -> 0(Vermelho -> amarelo) - 180(Vermelho)
  S -> 0(Branco) - 255(Cor total)
  V -> 0(preto) - 255(branco)
  */
  HSV = 3,
  /*
  cvCvtColor(src,dst,code); // src -> dst
  code    = CV_<X>2<Y>
  <X>/<Y> = RGB, BGR, GRAY, HSV, YCrCb, XYZ, Lab, Luv, HLS

  cvCvtColor(cimg,gimg,CV_BGR2GRAY); // cimg -> gimg

  HLS -> BGR
  cvtColor(frame,frame, CV_HLS2BGR);

  BGR -> HLS
  cvtColor(frame,frame, CV_BGR2HLS);
  H -> 0 - 180
  L -> 0 - 255
  S -> 0 - 255
  */
  HLS = 4,
};

class CodeBook
{
public:
  CodeBook (int w, int h);
  ~CodeBook ();
  
  int SearchCodeWord (float values[], float thresholds[]);        

  virtual void UpdateMatchedCodeWord (float c[]) = 0;
  virtual bool IsBackground (float c[], float thresholds[], int i) = 0;
  // N = number of frames
  // sizeTuple = 6(RGB) or 4(HSV, HLS, LAB, YCbCr)
  virtual void WrapAroundLambda (float N) = 0;
  virtual void RemoveUnderutilizedCodeWords (int N) = 0;
 
  int row, col;
  std::vector<CodeWord> codewords;
};

///////////////////////////////////////////////////////////////////
// ColorSpace RGB
class CodeBookRGB : public CodeBook
{
public:
  CodeBookRGB (int w, int h);
  ~CodeBookRGB ();

  virtual void UpdateMatchedCodeWord (float c[]);
  virtual bool IsBackground (float c[], float thresholds[], int i);
  virtual void WrapAroundLambda(float N);
  virtual void RemoveUnderutilizedCodeWords (int N);

  bool Brightness (float I, float Imin, float Imax);
  float Colordist (float c[], float colors[]);
};

///////////////////////////////////////////////////////////////////
// ColorSpace LAB
class CodeBookLAB : public CodeBook
{
public:
  CodeBookLAB (int w, int h);
  ~CodeBookLAB ();

  virtual void UpdateMatchedCodeWord (float c[]);
  virtual bool IsBackground (float c[], float thresholds[], int i);
  virtual void WrapAroundLambda (float N);
  virtual void RemoveUnderutilizedCodeWords (int N);
};

///////////////////////////////////////////////////////////////////
// ColorSpace YCrCb
class CodeBookYCrCb : public CodeBook
{
public:
  CodeBookYCrCb (int w, int h);
  ~CodeBookYCrCb ();

  virtual void UpdateMatchedCodeWord (float c[]);
  virtual bool IsBackground (float c[], float thresholds[], int i);
  virtual void WrapAroundLambda (float N);
  virtual void RemoveUnderutilizedCodeWords (int N);

  float LuminanceDistance (float colors[], int indexToCodeWord);
  float CrominanceDistance (float colors[], int indexToCodeWord);
};

///////////////////////////////////////////////////////////////////
// ColorSpace HSV
class CodeBookHSV : public CodeBook
{
public:
  CodeBookHSV (int w, int h);
  ~CodeBookHSV ();

  virtual void UpdateMatchedCodeWord(float c[]);
  virtual bool IsBackground(float c[], float thresholds[], int i);
  virtual void WrapAroundLambda(float N);
  virtual void RemoveUnderutilizedCodeWords(int N);

  float ColorDistortion (float colors[], int indexToCodeWord);
  float ValueDistortion (float colors[], int indexToCodeWord);

};

///////////////////////////////////////////////////////////////////
// ColorSpace HLS
class CodeBookHLS : public CodeBook
{
public:
  CodeBookHLS (int w, int h);
  ~CodeBookHLS ();

  virtual void UpdateMatchedCodeWord (float c[]);
  virtual bool IsBackground (float c[], float thresholds[], int i);
  virtual void WrapAroundLambda (float N);
  virtual void RemoveUnderutilizedCodeWords (int N);

  float HLSDistorted (float values[], int indexToCodeWord);
};

#endif
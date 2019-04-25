#include "codebooksmanager.h"
#include <cstring>
#include <algorithm>
#include <iostream>
#include <windows.h>
#include <fstream>

#include "imageprocessing.h"

#include "codebook.h"

void CodeBookManager::CbImage ()
{
  std::vector<std::string> images = GetPaths("../resources/images.txt");
  if (images.size() > 1)
  {
    for (int i = 0; i < (int)images.size() - 1; i++)
    {
      cv::Mat frameBack = imread(images[i], cv::IMREAD_COLOR);
      im_rows = frameBack.rows;
      im_cols = frameBack.cols;
      Construct(frameBack);
    }
    cv::namedWindow("CodeBook BackGround Subtracion - Image");
    cv::Mat framef = imread(images[images.size() - 1], cv::IMREAD_COLOR);
    cv::Mat frame = framef.clone();
    cv::Mat_<uchar> mask = GetSubtractionMask(framef);

    // Estratégia equalização de histograma
#ifdef CODEBOOK_SHOW_MASK
    namedWindow("Edge detector");
    cv::Mat frameHECED = histogramEqualWithCannyEdgeDetector(framef, 0, 40);
    frameHECED = Closing(frameHECED, 2);
    frameHECED = fillTheHolesMask(dilation(frameHECED, 1));
    imshow("Edge detector", frameHECED);

    cv::Mat frame3 = frameHECED.clone();
    cv::Mat mask2 = mask.clone();
    mask2 = erosion(mask2, 5);
#endif

    // Opening to decrease noise
#ifdef CODEBOOK_MASK_OPENING_OPERATION
    mask = Opening(mask, 2);
#endif

    // Cobrir os buracos da imagem
#ifdef CODEBOOK_FILL_HOLES_MASK
    mask = fillTheHolesMask(mask);
#endif

    imshow("CodeBook BackGround Subtracion - Image", BackGroundSubtraction(frame, mask));
    //imwrite("resimage.jpg", BackGroundSubtraction(frame, mask));
    cv::waitKey(0);
  }
}

void CodeBookManager::CbVideo ()
{
  std::vector<std::string> videos = GetPaths("../resources/videos.txt");
  if (videos.size() > 1)
  {
    cv::VideoCapture captureBack;
    cv::VideoWriter out;
    for (int i = 0; i < (int)videos.size() - 1; i++)
    {
      captureBack.open(videos[i]);
      if (!captureBack.isOpened()) {
        std::cout << "Not found video source" << std::endl;
        getchar();
      }
      Construct(0, captureBack);
    }

    cv::VideoCapture captureFore(videos[videos.size() - 1]);
    if (!captureFore.isOpened()) {
      std::cout << "Not found video input" << std::endl;
      getchar();
      return;
    }

    cv::Mat frame;
    out.open("VideoOutput.mkv", cv::VideoWriter::fourcc('X', '2', '6', '4'), 60, cv::Size(im_cols, im_rows));
    while (captureFore.read(frame))
    {
      cv::Mat frameFore = frame.clone();
      cv::Mat_<uchar> mask = GetSubtractionMask(frameFore);

      // Opening to decrease noise
#ifdef CODEBOOK_MASK_OPENING_OPERATION
      mask = Opening(mask, 2);
#endif

      // Cobrir os buracos da imagem
#ifdef CODEBOOK_FILL_HOLES_MASK
      mask = fillTheHolesMask(mask);
#endif

      cv::Mat resframe = BackGroundSubtraction(frame, mask);
      out << resframe;
      out.write(resframe);
    }
    out.release();
    captureFore.release();
    CvCapture* capture = 0;
  }
}

void CodeBookManager::CbVideoStreaming ()
{
  cv::VideoCapture capture;
  capture.open(0);

  if (!capture.isOpened()) {
    std::cout << "Camera not found" << std::endl;
    getchar();
  }
  cv::Mat frame;
  capture >> frame;
  Construct(50, 0, capture);

  cv::namedWindow("CodeBook BackGround Subtracion - Video Streaming", 1);
  for (;;)
  {
    capture >> frame;
    cv::Mat frameStandard = frame.clone();
    cv::Mat_<uchar> mask = GetSubtractionMask(frameStandard);

    /***********Estratégia equalização de histograma*************/
    //cv::Mat frameHECED = cbook::histogramEqualWithCannyEdgeDetector(frameFore,0,30);
    //frameHECED = cbook::Closing(frameHECED, 4);
    //frameHECED = cbook::fillTheHolesMask(frameHECED);
    //imshow("Edge detector - RGB", frameHECED);

     // Opening to decrease noise
#ifdef CODEBOOK_MASK_OPENING_OPERATION
    mask = Opening(mask, 2);
#endif

    // Cobrir os buracos da imagem
#ifdef CODEBOOK_FILL_HOLES_MASK
    mask = fillTheHolesMask(mask);
#endif

    frame = BackGroundSubtraction(frame, mask);
    imshow("CodeBook BackGround Subtracion - Video Streaming", frame);
    if (cv::waitKey(30) == 0) break;
  }
  capture.release();
  CvCapture* cap = 0;
}

void CodeBookManager::TrainFrame (cv::Mat frame, int n)
{  
  TransformFromBGR(frame);

	// Variáveis auxiliares
	float* px_color = new float[3];
  float* threshold = new float[2];

  cv::Mat_<cv::Vec3b> _frame = frame;

	// Percorrer todo o frame
	for(int i = 0; i < im_rows ; ++i)
	{
		for(int j= 0; j < im_cols ; ++j)
		{
      GetColorAndThresholdFromSource(0, _frame, i, j, px_color, threshold);
        
      // Buscar dentro do CodeBook do referente píxel se há
      //   algum CodeWord que reconhece o novo píxel de entrada.
      int index = codebooks[(i*im_rows)+j]->SearchCodeWord(px_color, threshold);
           
      if (cbook_colorspace == CodeBookColorSpace::RGB)
      {
			  // Se não for encontrado nenhum CodeWord, crias-se um novo
			  // CodeWord para o CodeBook relacionado

        // Capturar os valores de cor do frame de entrada
        //    e calcular o valor de brilho 'I'.
			  float I = sqrt(pow(px_color[0], 2) + pow(px_color[1], 2) + pow(px_color[2], 2));
			  
        if(index == -1) {
			  	CodeWord codeWord;
			  	codeWord.SetAttributes(px_color, 3);
			  	float tuple[6] = {I, I, 1.f, (float)n, (float)n+1.f, (float)n+1.f};
			  	codeWord.SetTuple(tuple, 6);
			  	codebooks[(i*im_rows)+j]->codewords.push_back(codeWord);
			  } 
			  // Caso contrário, apenas atualizar os valores do CodeWord escolhido
			  else{
			  	codebooks[(i*im_rows)+j]->UpdateMatchedCodeWord(px_color);
			  	codebooks[(i*im_rows)+j]->codewords[0].tuple[3] = 
			  		max(codebooks[(i*im_rows)+j]->codewords[0].tuple[3],
			  		((float)n + 1.0f) - codebooks[(i*im_rows)+j]->codewords[0].tuple[5]);
			  	codebooks[(i*im_rows)+j]->codewords[0].tuple[5] = (float)n + 1.0f;
			  }
      }
      else  // LAB YCrCb HSV HLS
      {
        if (index == -1)
        {
          CodeWord codeWord;
          codeWord.attributes[0] = px_color[0];
          codeWord.attributes[1] = px_color[1];
          codeWord.attributes[2] = px_color[2];
          codeWord.tuple[0] = 1.f;
          codeWord.tuple[1] = (float)n;
          codeWord.tuple[2] = codeWord.tuple[3] = (float)n + 1.f;
          codebooks[(i*im_rows) + j]->codewords.push_back(codeWord);
        }
        else { // _codeWords[index]? ao invés de _codeWords[0]
          codebooks[(i*im_rows) + j]->UpdateMatchedCodeWord(px_color);
          codebooks[(i*im_rows) + j]->codewords[0].tuple[1] =
            max(codebooks[(i*im_rows) + j]->codewords[0].tuple[1],
            ((float)n + 1.f) - codebooks[(i*im_rows) + j]->codewords[0].tuple[3]);
          codebooks[(i*im_rows) + j]->codewords[0].tuple[3] = (float)n + 1.f;

        }
      }
		}
	}

  delete[] px_color;
  delete[] threshold;
}

cv::Mat CodeBookManager::GetSubtractionMask (cv::Mat frame)
{
  TransformFromBGR(frame);

  float* px_color = new float[3];
  float* threshold = new float[2];

  cv::Mat_<uchar> mask;
	mask.create(im_rows,im_cols);
  cv::Mat_<cv::Vec3b> _frame = frame;
	for(int i = 0; i < im_rows ; ++i)
	{
		for(int j= 0; j < im_cols ; ++j)
		{
      GetColorAndThresholdFromSource(1, _frame, i, j, px_color, threshold);

      int index = codebooks[(i*im_rows)+j]->SearchCodeWord(px_color, threshold);

			if(index == 0)
			{
				mask.at<uchar>(i, j) = 0;
				codebooks[(i*im_rows)+j]->UpdateMatchedCodeWord(px_color);
			}else{
				mask.at<uchar>(i, j) = 255;
			}
		}
	}

  delete[] px_color;
  delete[] threshold;

	return mask;
}

cv::Mat CodeBookManager::BackGroundSubtraction (cv::Mat frame, cv::Mat mask)
{
  cv::Mat_<cv::Vec3b> _frame = frame;
  for (int i = 0; i < im_rows; ++i)
  {
    for (int j = 0; j < im_cols; ++j)
    {
      if (mask.at<uchar>(i, j) == 0)
      {
        _frame.at<cv::Vec3b>(i, j)[0] = 0;
        _frame.at<cv::Vec3b>(i, j)[1] = 0;
        _frame.at<cv::Vec3b>(i, j)[2] = 0;
      }
    }
  }
  return frame;
}

void CodeBookManager::WrapAroundLambda (int N)
{
  for (int i = 0; i < (int)codebooks.size(); ++i)
  {
    codebooks[i]->WrapAroundLambda((float)N);
  }
}

void CodeBookManager::RemoveUnderutilizedCodeWords (int N)
{
  for (int i = 0; i < (int)codebooks.size(); i++)
  {
    codebooks[i]->RemoveUnderutilizedCodeWords(N);
  }
}

void CodeBookManager::InitializeCodeBooks (cv::Mat frame)
{
  im_cols = frame.cols;
  im_rows = frame.rows;
  AddCodebooks();
}

void CodeBookManager::Construct (cv::Mat frame)
{
  if (codebooks.empty())
  {
    InitializeCodeBooks(frame);
  }
  TrainFrame(frame, 0);
  WrapAroundLambda(1);
}

void CodeBookManager::Construct (int frameSpacing, cv::VideoCapture cap)
{
  //variável auxiliar para captura dos frames de um vídeo
  cv::Mat frame;
  //caso não seja possível ler um frame do vídeo, retorna sem
  //criar o CodeBook, caso contrário, apenas lê o primeiro frame
  //do vídeo
  if (!cap.read(frame))
  {
    return;
  }

  //Se a estrutura de CodeBooks for vazia, inicializa matriz
  // de CodeBooks
  if (codebooks.empty())
  {
    InitializeCodeBooks(frame);
  }
  //treina os CodeBooks com o frame lido
  TrainFrame(frame, 0);

  //captura o resto dos frames da cena de referência para
  // o treinamento dos CodeBooks, modelando o background da cena 
  int n;
  for (n = 1; cap.read(frame); n++)
  {
    TrainFrame(frame, n);
    for (int i = 0; i < frameSpacing; i++)
    {
      cap.read(frame);
    }
  }
  //Arrumar o valor de MNRL (lambda)
  WrapAroundLambda(n);
  RemoveUnderutilizedCodeWords(n);
}

void CodeBookManager::Construct (int NFrames, double delayPerFrame, cv::VideoCapture cap)
{
  //Variáveis auxiliares
  cv::Mat frame;
  //captura um frame do input
  cap >> frame;
  if (codebooks.empty())
  {
    InitializeCodeBooks(frame);
  }
  for (int n = 0; n < NFrames; n++)
  {
    TrainFrame(frame, n);
    Sleep((DWORD)delayPerFrame);
    cap >> frame;
  }
  WrapAroundLambda(NFrames);
  RemoveUnderutilizedCodeWords(NFrames);
}

CodeBookManager::CodeBookManager (CodeBookColorSpace csp)
{
  cbook_colorspace = csp;
	codebooks.clear();
	im_cols = 0; im_rows = 0;
}

CodeBookManager::CodeBookManager (int _im_cols, int _im_rows):
im_cols(_im_cols), im_rows(_im_rows)
{
	codebooks.clear();
  AddCodebooks();
}

CodeBookManager::~CodeBookManager ()
{
  for (int i = 0; i < codebooks.size(); i++)
    delete codebooks[i];
  codebooks.clear();
}

void CodeBookManager::AddCodebooks ()
{
  for (int i = 0; i < im_rows; i++)
  {
    for (int j = 0; j < im_cols; j++)
    {
      if(cbook_colorspace == CodeBookColorSpace::RGB)
        codebooks.push_back(new CodeBookRGB(i, j));
      else if (cbook_colorspace == CodeBookColorSpace::LAB)
        codebooks.push_back(new CodeBookLAB(i, j));
      else if (cbook_colorspace == CodeBookColorSpace::YCrCb)
        codebooks.push_back(new CodeBookYCrCb(i, j));
      else if (cbook_colorspace == CodeBookColorSpace::HSV)
        codebooks.push_back(new CodeBookHSV(i, j));
      else if (cbook_colorspace == CodeBookColorSpace::HLS)
        codebooks.push_back(new CodeBookHLS(i, j));
    }
  }
}

cv::Mat CodeBookManager::TransformFromBGR (cv::Mat frame)
{
  if (cbook_colorspace == CodeBookColorSpace::RGB) {}
  else if (cbook_colorspace == CodeBookColorSpace::LAB) {
    cv::cvtColor(frame, frame, cv::COLOR_BGR2Lab);
  } else if (cbook_colorspace == CodeBookColorSpace::YCrCb) {
    cv::cvtColor(frame,frame, cv::COLOR_BGR2YCrCb);
  } else if (cbook_colorspace == CodeBookColorSpace::HSV) {
    cv::cvtColor(frame, frame, cv::COLOR_BGR2HSV);
  } else if (cbook_colorspace == CodeBookColorSpace::HLS) {
    cv::cvtColor(frame, frame, cv::COLOR_BGR2HLS);
  }
  return frame;
}

void CodeBookManager::GetColorAndThresholdFromSource (int tmode, cv::Mat_<cv::Vec3b> frame, int i, int j,
                                                      float* px_colors, float* threshold)
{
  if (cbook_colorspace == CodeBookColorSpace::RGB)
  {
    px_colors[2] = frame(i, j)[0];
    px_colors[1] = frame(i, j)[1];
    px_colors[0] = frame(i, j)[2];
    //float I = sqrt(pow(c[0], 2) + pow(c[1], 2) + pow(c[2], 2));

    if (tmode == 0)
      threshold[0] = RGB_ERROR1;
    else if (tmode == 1)
      threshold[0] = RGB_ERROR2;
  }
  else if (cbook_colorspace == CodeBookColorSpace::LAB)
  {
    px_colors[0] = frame(i, j)[0];
    px_colors[1] = frame(i, j)[1];
    px_colors[2] = frame(i, j)[2];

    if (tmode == 0)
      threshold[0] = LAB_MAX_DISTANCE_1;
    else if (tmode == 1)
      threshold[0] = LAB_MAX_DISTANCE_2;
  }
  else if (cbook_colorspace == CodeBookColorSpace::YCrCb)
  {
    px_colors[0] = frame(i, j)[0];
    px_colors[1] = frame(i, j)[1];
    px_colors[2] = frame(i, j)[2];

    if (tmode == 0)
    {
      threshold[0] = YCRCB_ERROR1_1;
      threshold[1] = YCRCB_ERROR1_2;
    }
    else if (tmode == 1)
    {
      threshold[0] = YCRCB_ERROR2_1;
      threshold[1] = YCRCB_ERROR2_2;
    }
  }
  else if (cbook_colorspace == CodeBookColorSpace::HSV)
  {
    px_colors[0] = (float)(frame(i, j)[0] % 180) * 2.0f;
    px_colors[1] = frame(i, j)[1];
    px_colors[2] = frame(i, j)[2];

    if (tmode == 0)
    {
      threshold[0] = HSV_ERROR1_1;
      threshold[1] = HSV_ERROR1_2;
    }
    else if (tmode == 1)
    {
      threshold[0] = HSV_ERROR2_1;
      threshold[1] = HSV_ERROR2_2;
    }
  }
  else if (cbook_colorspace == CodeBookColorSpace::HLS)
  {
    px_colors[0] = (float)(frame(i, j)[0] % 180) * 2.0f;
    px_colors[1] = frame(i, j)[1];
    px_colors[2] = frame(i, j)[2];

    if (tmode == 0)
    {
      threshold[0] = HLS_ERROR1;
    }
    else if (tmode == 1)
    {
      threshold[0] = HLS_ERROR2;
    }
  }
}

std::vector<std::string> CodeBookManager::GetPaths (const char* nfile)
{
  std::ifstream file;
  file.open(nfile);
  std::vector<std::string> paths;
  std::string line;
  while (file.good())
  {
    getline(file, line);
    paths.push_back(line);
  }
  file.close();
  return paths;
}
#include "imageprocessing.h"

    cv::Mat erosion(cv::Mat frame, int sSize)
    {
        cv::Mat element;
        int erosion_type = cv::MORPH_RECT; //MORPH_ELLIPSE, MORPH_RECT MORPH_CROSS
        int erosion_size = sSize;
        element = cv::getStructuringElement(erosion_type, cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ), cv::Point(erosion_size,erosion_size));
        cv::erode(frame, frame, element);
        return frame;
    }

    cv::Mat dilation(cv::Mat frame, int sSize)
    {
        cv::Mat element;
        int dilation_type = cv::MORPH_RECT; //MORPH_ELLIPSE, MORPH_RECT MORPH_CROSS
        int dilation_size = sSize;
        element = cv::getStructuringElement(dilation_type, cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ), cv::Point(dilation_size,dilation_size));
        cv::dilate(frame, frame, element);
        return frame;
    }

    cv::Mat Opening(cv::Mat frame, int sSize)
    {
        frame = erosion(frame, sSize);
        frame = dilation(frame, sSize);
        return frame;
    }

    cv::Mat Closing(cv::Mat frame, int sSize)
    {
        frame = dilation(frame, sSize);
        frame = erosion(frame, sSize);
        return frame;
    }

    cv::Mat histogramEqualization(cv::Mat frame)
    {
        cv::Mat out;
        cv::Mat frameGrayscale = frame.clone();
        cvtColor( frameGrayscale, frameGrayscale, cv::COLOR_BGR2GRAY );
        equalizeHist( frameGrayscale, out );
        return out;
    }

    cv::Mat CannyEdgeDetector(cv::Mat frame, int lowThreshold = 0, int highThreshold = 30)
    {
        cv::Mat edges = frame.clone();
        cv::cvtColor(edges, edges, cv::COLOR_BGR2GRAY);
        GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
        Canny(edges, edges, lowThreshold, highThreshold, 3);
        return edges;
    }

    cv::Mat histogramEqualWithCannyEdgeDetector(cv::Mat frame, int lowThreshold = 0, int highThreshold = 30)
    {
        cv::Mat out;
        cv::Mat frameGrayscale = frame.clone();
        cvtColor( frameGrayscale, frameGrayscale, cv::COLOR_BGR2GRAY );
        equalizeHist( frameGrayscale, out );

        cv::Mat edges = out;
        GaussianBlur(edges, edges, cv::Size(7,7), 1.5, 1.5);
        Canny(edges, edges, lowThreshold, highThreshold, 3);


        return edges;
    }

    //http://bsd-noobz.com/opencv-guide/60-2-color-based-object-segmentation
    cv::Mat fillTheHolesMask(cv::Mat mask)
    {
        std::vector<std::vector<cv::Point> > contours;
        findContours(mask.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        cv::Mat dst = cv::Mat::zeros(mask.size(), mask.type());
        drawContours(dst, contours, -1, cv::Scalar::all(255), cv::FILLED);
        return dst;
    }

    bool outIndex(int w, int h, int rows, int cols)
    {
        if(w < 0 || h < 0 || w >= cols || h >= rows)
            return true;
        return false;
    }

    /*Função pendente*/
    cv::Mat detectForegroundByEdges(cv::Mat frame, int h, int w, cv::Mat mask)
    {
        cv::Mat maskRet = mask;
        int range = 1;
        int width = frame.cols;
        int height = frame.rows;
        bool stop = false;
        bool pass = true;

        while(!stop)
        {   
            w -= 1;
            if(outIndex(w,h,height,width)){stop = true; pass = false; continue;}
            if(mask.at<uchar>(w,h) == 0) {maskRet.at<uchar>(w,h) = 255;}else{stop = true; continue;}
            for(int i = 0 ; i < range ; i++)
            {
                h--;
                if(outIndex(w,h,height,width)){stop = true; pass = false; break;}
                if(mask.at<uchar>(w,h) == 0) {maskRet.at<uchar>(w,h) = 255;}else{stop = true; continue;}
            }
            if(stop) {break;}
            for(int i = 0 ; i < range*2 ; i++)
            {
                w++;
                if(outIndex(w,h,height,width)){stop = true; pass = false; break;}
                if(mask.at<uchar>(w,h) == 0) {maskRet.at<uchar>(w,h) = 255;}else{stop = true; continue;}
            }
            if(stop) {break;}
            for(int i = 0 ; i < range*2 ; i++)
            {
                h++;
                if(outIndex(w,h,height,width)){stop = true; pass = false; break;}
                if(mask.at<uchar>(w,h) == 0) {maskRet.at<uchar>(w,h) = 255;}else{stop = true; continue;}
            }
            if(stop) {break;}
            for(int i = 0 ; i < range*2 ; i++)
            {
                w--;
                if(outIndex(w,h,height,width)){stop = true; pass = false; break;}
                if(mask.at<uchar>(w,h) == 0) {maskRet.at<uchar>(w,h) = 255;}else{stop = true; continue;}
            }
            if(stop) {break;}
            for(int i = 0 ; i < range-1 ; i++)
            {
                h--;
                if(outIndex(w,h,height,width)){stop = true; pass = false; break;}
                if(mask.at<uchar>(w,h) == 0) {maskRet.at<uchar>(w,h) = 255;}else{stop = true; continue;}
            }
            h--;
            range++;
        }
        if(pass)
            return maskRet;
        else
            return mask;
    }

    float tookPixels(cv::Mat mask, int h, int w, int range)
    {
        h -= range;
        w -= range;
        int retSum = 0;
        for(int i = 0 ; i < (range*2) + 1 ; i++)
        {
            for(int j = 0 ; j < (range*2) + 1 ; j++)
            {
                if(h >= 0 && w >= 0 && h < mask.rows && w < mask.cols)
                {
                    if(mask.at<uchar>(h,w) != 0)
                        retSum++;    
                }
            }
        }
        return (float)retSum;  
    }


    cv::Mat statisticalMask(cv::Mat frame, cv::Mat mask, int range)
    {
        cv::Mat_<float> frameRet;
        frameRet.rows = mask.rows;
        frameRet.cols = mask.cols;
        cv::Mat_<cv::Vec3i> frameAux =  frame;
        cv::Mat_<uchar> frameMask = mask;

        float total = ((float)range*2.f) + 1.f;

        int width = frame.cols;
        int height = frame.rows;
        for(int i = 0; i < height ; i++)
        {
            for(int j = 0; j < width ; j++)
            {   
                frameRet(i,j) = tookPixels(frameMask, i, j, range) / total;
            }
        }
        return frameRet;
    }

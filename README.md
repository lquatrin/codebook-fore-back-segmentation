## Implementation of Real-time foreground–background segmentation using codebook model

---

Codebook Foreground-Background segmentation application, based on [*Kim et al.* paper](http://www.sciencedirect.com/science/article/pii/S1077201405000057) from 2005. Also, added HLS, HSV, LAB and YCrCb color spaces to compute codewords, instead of only RGB.

* Kyungnam Kim, Thanarat H. Chalidabhongse, David Harwood, Larry Davis, Real-time foreground–background segmentation using codebook model, Real-Time Imaging, Volume 11, Issue 3, 2005, Pages 172-185, ISSN 1077-2014, https://doi.org/10.1016/j.rti.2004.12.004.

* Abstract: We present a real-time algorithm for foreground–background segmentation. Sample background values at each pixel are quantized into codebooks which represent a compressed form of background model for a long image sequence. This allows us to capture structural background variation due to periodic-like motion over a long period of time under limited memory. The codebook representation is efficient in memory and speed compared with other background modeling techniques. Our method can handle scenes containing moving backgrounds or illumination variations, and it achieves robust detection for different types of videos. We compared our method with other multimode modeling techniques. In addition to the basic algorithm, two features improving the algorithm are presented—layered modeling/detection and adaptive codebook updating. For performance evaluation, we have applied perturbation detection rate analysis to four background subtraction algorithms and two videos of different types of scenes.

### Project settings

---

* Visual Studio 2017

* [OpenCV 4.0.1](https://opencv.org/)

    * [Documentation](https://docs.opencv.org/4.0.1/)
    
    * Must set OPENCV_PATH to "../opencv/build/"

    * Must copy .dll files to bin folder

* [OpenCV C++ Tutorial](https://www.opencv-srf.com)
    
### Application command line usage

---

```bash
*.exe [colorspace] [name or "live"]
```
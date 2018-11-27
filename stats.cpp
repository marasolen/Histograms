
#include "stats.h"
#include <stdio.h>
#include <cmath>

stats::stats(PNG & im){

  for (unsigned int x = 0; x < im.width(); x++) {
          vector<double> colHueX;
          vector<double> colHueY;
          vector<double> colSat;
          vector<double> colLum;
          vector<vector<int>> colHist;

          double colSumHueX = 0;
          double colSumHueY = 0;
          double colSumSat = 0;
          double colSumLum = 0;
          vector<int> colSumHist(36, 0);

          for (unsigned int y = 0; y < im.height(); y++) {
              double hueXSumToLeft = 0;
              double hueYSumToLeft = 0;
              double satSumToLeft = 0;
              double lumSumToLeft = 0;
              vector<int> histSum(36, 0);

              if (x > 0) {
                  hueXSumToLeft = sumHueX[x - 1][y];
                  hueYSumToLeft = sumHueY[x - 1][y];
                  satSumToLeft = sumSat[x - 1][y];
                  lumSumToLeft = sumLum[x - 1][y];
                  histSum = hist[x - 1][y];
              }

              HSLAPixel pixel = *(im.getPixel(x, y));
              colSumHueX += cos(pixel.h * M_PI / 180);
              colSumHueY += sin(pixel.h * M_PI / 180);
              colSumSat += pixel.s;
              colSumLum += pixel.l;
              colSumHist[pixel.h / 10]++;

              for (unsigned int i = 0; i < 36; i++) {
                  histSum[i] += colSumHist[i];
              }

              colHueX.push_back(hueXSumToLeft + colSumHueX);
              colHueY.push_back(hueYSumToLeft + colSumHueY);
              colSat.push_back(satSumToLeft + colSumSat);
              colLum.push_back(lumSumToLeft + colSumLum);
              colHist.push_back(histSum);
          }

          sumHueX.push_back(colHueX);
          sumHueY.push_back(colHueY);
          sumSat.push_back(colSat);
          sumLum.push_back(colLum);
          hist.push_back(colHist);
      }
}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
  return (lr.first - ul.first + 1) * (lr.second - ul.second + 1);
}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){

  int x1 = ul.first;
  int x2 = lr.first;


  int y1 = ul.second;
  int y2 = lr.second;

  // sums over entire  block
  double x = sumHueX[x2][y2];
  double y = sumHueY[x2][y2];
  if (y1 == 0 && x1 == 0) {
    // subtract nothing
  } else {
    x = x - sumHueX[x2][y1-1];
    y = y - sumHueY[x1-1][y2];
  }
  double avghue = atan2(y, x) * 180/PI;


  int as = sumSat[x2][y2];
  if (y1 == 0 && x1 == 0) {
    // subtract nothing
  } else {
    as = as - sumSat[x2][y1-1] - sumSat[x1-1][y2];
  }
  // get average saturation
  double avgsat = as/rectArea(ul, lr);


  double al = sumLum[x2][y2];
  if (y1 == 0 && x1 == 0) {
    // subtract nothing
  } else {
    al = al - sumLum[x2][y1-1] - sumLum[x1-1][y2];
  }
  // get average saturation
  double avglum = al/(double) rectArea(ul, lr);


  HSLAPixel *avgpixel = new HSLAPixel(avghue, avgsat, avglum);
  return *(avgpixel);
}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){
    int x1 = ul.first - 1;
    int y1 = ul.second - 1;
    int x2 = lr.first;
    int y2 = lr.second;

    vector<int> distn;
    double entropy = 0;

    assert(x2 > x1 && y2 > y1);

    if (x1 >= 0 && y1 >= 0) {
         distn = hist[x2][y2] - (hist[x1][y2] + (hist[x2][y1] - hist[x1][y1]));
    } else if (x1 >= 0) {
         distn = hist[x2][y2] - hist[x1][y2];
    } else if (y1 >= 0) {
         distn = hist[x2][y2] - hist[x2][y1];
    } else {
         distn = hist[x2][y2];
    }

    long area = rectArea(ul, lr);
    cout << area << endl;

    for (unsigned int i = 0; i < distn.size(); i++) {
        if (distn[i] > 0 ) {
            cout << distn[i] << endl;
            entropy += ((double) distn[i]/(double) area) * log2((double) distn[i]/(double) area);
        }
    }

    return -1 * entropy;
}

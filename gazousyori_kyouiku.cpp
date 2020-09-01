#include <iostream>
#include <opencv2/opencv.hpp>

#define FILENAME "C:/Users/KAISEKI/Pictures/Sample3.jpg" //画像ファイル名
#define MOJISUU 26
#define NOLABEL 0
#define MAXLABEL (65535)
#define NEIGHBORS 4
#define ROWVAL 0
#define HIGHVAL 255
#define TH 50

int delta[NEIGHBORS][2] = { {0,-1} ,{-1,0}, {1,0} ,{0,1} };

void ToBinarization(cv::Mat img);
int Labeling(cv::Mat src_img, cv::Mat labeled_img);


int main(int argc, const char* argv[])
{
	//画像変数の定義
	cv::Mat src_img, moji_img[MOJISUU], labeled_img, result_img[MOJISUU];
	//文字の配列
	char sorcemoji[MOJISUU] = { 'A','B','C','D','E','F','G','H','I','J','K','L','M','N',
		'O','P','Q','R','S','T','U','V','W','X','Y','Z' };
	unsigned int currentLabel;

	int nx, ny, sortlabel;

	for (int i = 0; i < MOJISUU; i++) {
		moji_img[i] = cv::imread("moji" + std::to_string(i) + ".jpg", 0);
		if (moji_img[i].empty()) {
			printf("Cannot read moji_image file");
			return(-1);
		}
		ToBinarization(moji_img[i]);
		//cv::Canny(moji_img[i], moji_img[i], 50, 200);
	}

	src_img = cv::imread(FILENAME, 0);
	if (src_img.empty()) {
		printf("Cannot read image file:%s\n", FILENAME);
		return(-1);
	}
	//画像の二値化
	ToBinarization(src_img);

	//ラベル画像の生成(初期値はラベルなし)
	labeled_img.create(src_img.size(), CV_16UC1);
	labeled_img.setTo(NOLABEL);

	//文字ごとにラベル1番～26番を割り当て
	sortlabel = Labeling(src_img, labeled_img);

	int* min_x = new int[sortlabel];
	int* min_y = new int[sortlabel];
	int* max_x = new int[sortlabel];
	int* max_y = new int[sortlabel];

	min_x = { 0 };
	min_y = { 0 };
	max_x = { 0 };
	max_y = { 0 };
	cv::Mat* donemoji_img = new cv::Mat[sortlabel];

	for (int t = 0; t < sortlabel; t++) {
		for (int y = 0; y < moji_img[t].rows; y++) {
			for (int x = 0; x < moji_img[t].cols; x++) {
				if (moji_img[t].at<unsigned char>(y, x) == 0) {
				}
			}
		}
		donemoji_img[t].create(max_y - min_y + 3, max_x - min_x + 3, moji_img[t].type());
		donemoji_img[t].setTo(255);

		for (int y = min_y - 1; y <= max_y + 1; y++) {
			for (int x = min_x - 1; x <= max_x + 1; x ++) {
				if (moji_img[t].at<unsigned char>(y, x) == 0)
					donemoji_img[t].at<unsigned char>(y - min_y + 1, x - min_x + 1) = moji_img[t].at<unsigned char>(y, x);
			}
		}
	}



	/*ラベル番号ごとに画像配列を割り当て*/
	for (int t = 0; t < sortlabel; t++) {
		result_img[t].create(src_img.size(), src_img.type());
		result_img[t].setTo(HIGHVAL);
		for (int y = 0; y < result_img[t].rows; y++) {
			for (int x = 0; x < result_img[t].cols; x++) {
				currentLabel = labeled_img.at<uint16_t>(y, x);
				if (currentLabel == t + 1) {
					result_img[t].at<unsigned char>(y, x) = ROWVAL;
				}
			}
		}
	}

	//画像連続表示
	int t = 0;

	//cv::imshow("src_img", src_img);
	while (1) {
		if (t == MOJISUU)
			break;
		std::string a = std::to_string(t);
		cv::imshow("result_img", result_img[t]);
		if (cv::waitKey(1000) >= 0) break;
		t++;
	}

	//int startx, starty, bx, by, difx, dify;
	//int diff[8][2] = { { -1,0 }, { -1,-1 }, { 0,-1 }, { 1,-1 }, { 1,0 }, { 1,1 }, { 0,1 }, { -1,1 } };

	////ライントレース
	//for (int i = 0; i < MOJISUU; i++) {
	//	for (int y = 0; y < moji_img[i].rows; y++) {
	//		for (int x = 0; x < moji_img[i].cols; x++) {
	//			if (moji_img[i].at<unsigned char>(y, x) == 0) {
	//				bx = x;
	//				by = y;
	//				for (int id = 0; id < 8; id++) {
	//					nx = x + diff[id][0];
	//					ny = y + diff[id][1];
	//					if (moji_img[i].at<unsigned char>(nx, ny) == 0) {
	//						
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	return(0);
}

//画像の二値化関数
void ToBinarization(cv::Mat img)
{
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			if (img.at<unsigned char>(y, x) < TH) {
				img.at<unsigned char>(y, x) = ROWVAL;
			}
			else {
				img.at<unsigned char>(y, x) = HIGHVAL;
			}
		}
	}
}

//ラベリングアルゴリズム
int Labeling(cv::Mat src_img, cv::Mat labeled_img)
{
	//変数宣言
	int nx, ny;
	unsigned int currentLabel, neighborLabel, minNeighborLabel, labelNo = 0;
	bool isUpdated;

	//更新がなくなるまで走査操作処理を繰り返す
	do {
		isUpdated = false;
		for (int y = 0; y < src_img.rows; y++) {
			for (int x = 0; x < src_img.cols; x++) {
				if (src_img.at<unsigned char>(y, x) == ROWVAL) {	//注目画像が黒画素の場合
					currentLabel = labeled_img.at<uint16_t>(y, x);	//注目画素の現ラベル値を取得
					minNeighborLabel = MAXLABEL;					//最小近傍ラベルの初期値設定(上限値)
					for (int idxN = 0; idxN < NEIGHBORS; idxN++) {	//近傍画素を探索
						nx = x + delta[idxN][0];
						ny = y + delta[idxN][1];
						if (nx >= 0 && nx <= labeled_img.cols && ny >= 0 && ny < labeled_img.rows) {
							neighborLabel = labeled_img.at<uint16_t>(ny, nx);
							if (neighborLabel != NOLABEL && neighborLabel < minNeighborLabel) {
								minNeighborLabel = neighborLabel;
								//近傍画素のラベル値がこれまでの最小ならば更新
							}
						}
					}
					if (currentLabel == NOLABEL) {	//注目画素のラベルがない場合(1回目)
						if (minNeighborLabel < MAXLABEL) {	//近傍画素にラベル値があれば
							labeled_img.at<uint16_t>(y, x) = minNeighborLabel;	//最小近傍ラベル値に更新
						}
						else {	//近傍画素にラベル値がなければ
							labeled_img.at<uint16_t>(y, x) = ++labelNo; //新規ラベルを注目画素に付与
						}
						isUpdated = true;	//更新フラグを立てる
					}
					else {	//注目画素のラベルがある場合(2回目以降)
						if (minNeighborLabel < currentLabel) { //現ラベルより近傍最小ラベル値が小さければ
							labeled_img.at<uint16_t>(y, x) = minNeighborLabel; //最小近傍ラベル値に更新
							isUpdated = true;
						}
					}
				}
			}
		}
	} while (isUpdated);

	/*それぞれの文字に対して、飛び飛びのラベル番号が付与されている。
	文字数はA～Zの26文字なため、飛び飛びのラベル番号を詰め、ラベル番号1～26番で
	整理する。*/
	int sortlabel = 1;
	int countLabel[MAXLABEL] = {};
	for (int y = 0; y < src_img.rows; y++) {
		for (int x = 0; x < src_img.cols; x++) {
			currentLabel = labeled_img.at<uint16_t>(y, x);
			if (currentLabel != 0) {
				if (countLabel[currentLabel] == 0) {
					labeled_img.at<uint16_t>(y, x) = sortlabel;
					countLabel[currentLabel] = sortlabel;
					sortlabel++;
				}
				else {
					labeled_img.at<uint16_t>(y, x) = countLabel[currentLabel];
				}
			}
		}
	}

	return(sortlabel - 1);
}
#include <Windows.h>
#include <gdiplus.h>
#include <iostream>
#include <vector>
#include <string>
#define ENTER 1 //Defines each button
#define CYCLE 2
#define IMGPROCESS 3

std::vector<std::string> listImages; //Vector for the names of images
int imageWidthNum, imageHeightNum; //Integer values for the image resolution
char resetStr[50]=""; //Reset String

HWND ImageNameText, ImageWidth, ImageHeight, PictureHeader; //Each textbox

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow) {
	static const char ClassName[] = "MyImageViewer";
	WNDCLASSEX wc;
	MSG msg;
	HWND hwnd;

	Gdiplus::GdiplusStartupInput gdiStartup;
	ULONG_PTR gdiToken;

	Gdiplus::GdiplusStartup(&gdiToken, &gdiStartup, NULL);

	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = ClassName;
	wc.lpszMenuName = NULL;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Error register class", "Error", MB_ICONERROR);
		return 0;
	}

	listImages.emplace_back("whitePic.JPG"); //Adds a white picture to the listImages vector to stabilize the program

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, ClassName, "Image Viewer", WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, 1400, 1005, NULL, NULL, hInstance, NULL); //Creation of the window

	if (hwnd == NULL) {
		MessageBox(NULL, "Error create window", "Error", MB_ICONERROR);
		return 0;
	}

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiToken);
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	static unsigned int cxClient, cyClient, positionX, positionY, counter;
	switch (msg) {
		case WM_SIZE: {
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			break;
		}
		case WM_CREATE: { //Making the

            //Window is about 1500 by 1005 pixels

            //Makes the drop down menu for the reset button
            HMENU hBaseMenu = CreateMenu();
            HMENU hSubMenu = CreateMenu();
            AppendMenu(hBaseMenu, MF_POPUP, (UINT_PTR)hSubMenu, "Menu");
            AppendMenu(hSubMenu, MF_STRING, 101, "Reset Window");
            SetMenu(hwnd, hBaseMenu);

            //Picture header textbox -> Set to basic value of "[No Picture Inputted]"
            PictureHeader=CreateWindowExW(0, L"STATIC", L"[No Picture Inputted]", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_CENTER,680, 5, 200, 40, hwnd, NULL, GetModuleHandleW(NULL), NULL);

            //The title boxes for the user input textboxes
            CreateWindowExW(0, L"STATIC", L"File Name", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_CENTER,30, 867, 80, 30, hwnd, NULL, GetModuleHandleW(NULL), NULL);
            CreateWindowExW(0, L"STATIC", L"Image Width", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_CENTER,335, 862, 80, 35, hwnd, NULL, GetModuleHandleW(NULL), NULL);
            CreateWindowExW(0, L"STATIC", L"Image Height", WS_BORDER | WS_VISIBLE | WS_CHILD | ES_CENTER,440, 862, 80, 35, hwnd, NULL, GetModuleHandleW(NULL), NULL);

            //The 3 user input textboxes
            ImageNameText=CreateWindowExW(0, L"EDIT", L"", WS_BORDER | WS_VISIBLE | WS_CHILD,30, 900, 300, 30, hwnd, NULL, GetModuleHandleW(NULL), NULL);
            ImageWidth=CreateWindowExW(0, L"EDIT", L"", WS_BORDER | WS_VISIBLE | WS_CHILD,335, 900, 100, 30, hwnd, NULL, GetModuleHandleW(NULL), NULL);
            ImageHeight=CreateWindowExW(0, L"EDIT", L"", WS_BORDER | WS_VISIBLE | WS_CHILD,440, 900, 100, 30, hwnd, NULL, GetModuleHandleW(NULL), NULL);

            //The 3 buttons
            CreateWindowW(L"BUTTON", L"ENTER", WS_VISIBLE | WS_CHILD, 545, 900, 70, 30 , hwnd, (HMENU)ENTER, GetModuleHandleW(NULL), NULL);
            CreateWindowW(L"BUTTON", L"PROCESS IMAGE", WS_VISIBLE | WS_CHILD, 620, 900, 140, 30 , hwnd, (HMENU)CYCLE, GetModuleHandleW(NULL), NULL);
            CreateWindowW(L"BUTTON", L"CYCLE IMAGES", WS_VISIBLE | WS_CHILD, 765, 900, 140, 30 , hwnd, (HMENU)IMGPROCESS, GetModuleHandleW(NULL), NULL);

            break;
		}
		case WM_PAINT: {
			hdc = BeginPaint(hwnd, &ps);
			Gdiplus::Graphics graph(hdc);
			std::wstring openedFile;

			if (listImages.size() == 0) { // no file detected
				// Show text images not found
				Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));
				Gdiplus::FontFamily ff(L"Times New Roman");
				Gdiplus::Font font(&ff, 15, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
				Gdiplus::PointF point(cxClient / 2, cyClient / 2);
				//graph.DrawString(L"Images can't be found", -1, &font, point, &brush);
				break;
			}

			openedFile = std::wstring(listImages[counter].begin(), listImages[counter].end());
			Gdiplus::Image img(openedFile.c_str());

			if (img.GetLastStatus() != Gdiplus::Ok) {
                // Handle the error (e.g., print a message or exit)
                // For now, let's print an error message to the console
                std::cout << "Error loading image: " << openedFile.c_str() << std::endl;
                break;
            }

			static int CtrWidth, CtrHeight; //Window is about 1400 by 1005 pixels
            if(imageWidthNum==1280&&imageHeightNum==720) {
                CtrWidth=1280, CtrHeight=720; //Changes image resolution to 720p
            }
            else if(imageWidthNum==854&&imageHeightNum==480) {
                CtrWidth=854, CtrHeight=480; //Changes image resolution to 480p
            }
            else if(imageWidthNum==640&&imageHeightNum==360) {
                CtrWidth=640, CtrHeight=360; //Changes image resolution to 360p
            }
            else if(imageWidthNum==426&&imageHeightNum==240) {
                CtrWidth=426, CtrHeight=240; //Changes image resolution to 240p
            }
            else if(imageWidthNum==256&&imageHeightNum==144) {
                CtrWidth=256, CtrHeight=144; //Changes image resolution to 144p
            }
            else {
                CtrWidth=imageWidthNum, CtrHeight=imageHeightNum; //Keeps the image the same resolution
            }

            positionX=((1400-CtrWidth)/2)-10,positionY=100;

			graph.DrawImage(&img, positionX, positionY, CtrWidth, CtrHeight);

			Gdiplus::Status drawStatus = graph.DrawImage(&img, positionX, positionY, CtrWidth, CtrHeight);
            if (drawStatus != Gdiplus::Ok) {
                std::cout << "Error drawing image: " << drawStatus << std::endl;
            }

			EndPaint(hwnd, &ps);
			break;
		}
		case WM_COMMAND: {
            if(LOWORD(wParam)==ENTER) {
                static char fnameUserInput[500], widthUserInput[500], heightUserInput[500];
                GetWindowText(ImageNameText, fnameUserInput, 50); //Grabs the values of each textbox
                GetWindowText(ImageWidth, widthUserInput, 50);
                GetWindowText(ImageHeight, heightUserInput, 50);
                imageWidthNum=std::stoi(widthUserInput), imageHeightNum=std::stoi(heightUserInput);
                if(imageWidthNum>1280&&imageHeightNum>720) { //Checks if the image resolution is greater than 720p
                    imageWidthNum=1280, imageHeightNum=720; //Auto adjusts image down to 720p to fit into the window
                }

                if(listImages.size()<2) { //Lets you input one image but only one
                    listImages.emplace_back(fnameUserInput);
                    counter++;
                    InvalidateRect(hwnd, NULL, TRUE); //Draws the image
                    char pictureHeaderStr[50]="Original Picture";
                    SetWindowText(PictureHeader, pictureHeaderStr); //Changes the header to "Original Picture"
                }
                SetWindowText(ImageNameText, resetStr); //Resets the values of the text boxes to make them blank
                SetWindowText(ImageWidth, resetStr);
                SetWindowText(ImageHeight, resetStr);
            }
            if(LOWORD(wParam)==CYCLE) { //Allows you to cycle through your 2 images
                if(listImages.size()<=2) { //If you only have 1 image to cycle between (Not counting the white picture), it will not let you do anything
                    break;
                }
                else { //Else meaning you have 2 pictures
                    if(counter==2) { //If the window has the second picture open, it changes to the first
                        counter=1;
                        char pictureHeaderStr[50]="Original Picture";
                        SetWindowText(PictureHeader, pictureHeaderStr);
                    }
                    else if(counter==1) { //If the window has the first picture open, it changes to the second
                        counter=2;
                        char pictureHeaderStr[50]="Edited Picture";
                        SetWindowText(PictureHeader, pictureHeaderStr);
                    }
                }
				InvalidateRect(hwnd, NULL, TRUE); //Draws the image
            }
            if(LOWORD(wParam)==IMGPROCESS) { //Button to process the user inputted image

            }
            if(LOWORD(wParam)==101) { //Menu Reset Button
                counter=0;
                if(listImages.size()==3) { //Checks the vector size to see if there are multiple pictures to choose from
                    listImages.erase(listImages.begin()+2); //Erase the second inputted picture
                }
                listImages.erase(listImages.begin()+1); //Erases the first inputted picture
                char pictureHeaderStr[50]="[No Picture Inputted]";
                SetWindowText(PictureHeader, pictureHeaderStr); //Resets the picture header to its beginning value
                InvalidateRect(hwnd, NULL, TRUE); //Draws the image
                SetWindowText(ImageNameText, resetStr); //Resets the values of the text boxes to make them blank
                SetWindowText(ImageWidth, resetStr);
                SetWindowText(ImageHeight, resetStr);
            }
            break;
		}
		case WM_CLOSE: {
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY: {
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
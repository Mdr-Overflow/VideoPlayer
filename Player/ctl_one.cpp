#include <windows.h>
#include <iostream>
#include "resource.h"
#include <vector>
#include<string>
#include<cstring>
#include <cstdlib>
#include <fstream>
#include <sys/stat.h>



#include <time.h>

#include <sstream>
#include <filesystem>
#define IDC_MAIN_EDIT	101
using namespace std;
HWND g_hToolbar = NULL;


vector<int> ver = {0};

int CurrentChannel = -1;

int k = 0;

string GetFileName(char* pth) {

	char* pch;
	char* temp;
	pch = strtok(pth, "\\");
	while ((temp = strtok(NULL, "\\")) != NULL)
	{
		pch = temp;
	}
	return string(pch);
}
char ProjectPath[1024];
int stage = 0;
bool done = FALSE;
class Video {
private :
	
	string date;
	
public:
	string name ;
	Video() {};

	Video(string num) {
		name = num;
		char pth[300];
		strcpy(pth, ProjectPath);
		strcat(pth, name.c_str());
		struct tm* clock;               

		struct stat attrib;         

		stat(pth, &attrib);     

		clock = gmtime(&(attrib.st_mtime));


		string str;
		
		str = str + to_string(clock->tm_mday);
		str = str + "-";

		str = str + to_string(clock->tm_mon+1);
		str = str + "-";
		
		str = str + to_string(clock->tm_year%100);
		date = str;



		
	}
	string GetVideoName() { return name; }
	string GetVideoDate() { return date; }

	string GetVideoPath() {
		char v[1024];
		strcat(v, ProjectPath);
		strcat(v, "/");
		strcat(v, name.c_str());
		return string(v);

	}

	


	
};
class Channel {
private:
	string name;
	int subs=0;
	
public:
	vector<Video> videos;
	vector<string> videoss;
	string GetSub() { return to_string(subs); }
	Channel() {}
	Channel(string num) { name = num; }
	void SetSub(string n) { subs = stoi(n); }
	vector<Video> GetVideoList() { return videos; }
	Video GetVideoByPoz(int i) { return videos[i]; }
	void DeleteVideoByPoz(int poz) { videos.erase(videos.begin() + poz); }
	void AddVideo(string vname) {
		Video aux(vname);
		videoss.push_back(vname);
		videos.emplace_back(vname);}
	string GetName() { return name; }
	
	
															
};

vector<Channel> Channels;

void AfisVideos(HWND hwnd, Channel ch) {
	
	vector<Video> v = ch.GetVideoList();

	for (int i = 0; i < v.size(); i++)
	{
		char text[1024];
		strcpy(text, v[i].GetVideoName().c_str());
		char path[1024];
		strcpy(path, ProjectPath);

		strcat(path, text);
		int index = SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)text);/* (LPARAM)buf*/


		SendDlgItemMessage(hwnd, IDC_LIST, LB_SETITEMDATA, (WPARAM)index, (LPARAM)i);

	}
}
	void AfisCanal(HWND hwnd) {
		
		

		for (int i = 0; i < Channels.size(); i++)
		{
			char text[1024];
			strcpy(text, Channels[i].GetName().c_str());
			int index = SendDlgItemMessage(hwnd, IDC_LIST, LB_ADDSTRING, 0, (LPARAM)text);


			SendDlgItemMessage(hwnd, IDC_LIST, LB_SETITEMDATA, (WPARAM)index, (LPARAM)i);

		}

}
	void save() {
		
		ofstream fis;
		fis.open("C:/ctl_one/date.txt", ios::trunc);
		
		
		
		for (int i = 0; i < Channels.size(); i++) {
			
			fis << Channels[i].GetName() << "," << Channels[i].GetSub();
			
			for (int j = 0; j < Channels[i].videos.size(); j++) {
				
				fis << "," << Channels[i].videoss[j];
			}
			
			
			
			fis << endl;
		}
		
		fis.close();
	}


	BOOL CALLBACK ToolDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
	{
		switch (Message)
		{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_OK:
			{
				int len = GetWindowTextLength(GetDlgItem(hwnd, IDC_NUME));
				if (len > 0)
				{


					char* buf;

					buf = (char*)GlobalAlloc(GPTR, len + 1);
					GetDlgItemText(hwnd, IDC_TEXT, buf, len + 1);
					string stri = string(buf);
					Channel aux(stri);
					Channels.push_back(aux);
					//save();
					ver[0] = 1;
					SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
					AfisCanal(hwnd);

				}
				DestroyWindow(hwnd);
				return NULL; 
				
			}
				
			case IDC_CANCEL: {
				DestroyWindow(hwnd);
				return NULL; }
				
			}
			break;
		default:
			return FALSE;
		}
		return TRUE;
	}
void init() {

	char pBuf[256];
	size_t len = sizeof(pBuf);
	int bytes = GetModuleFileName(NULL, pBuf, len);
	string spBuf = string(pBuf);

	string stopth;

	
	string BufName = GetFileName(pBuf);


	stopth = spBuf.substr(0, spBuf.find(string(BufName)));


	strcpy(ProjectPath, stopth.c_str());


	stage = 0;
	ifstream fis;
	fis.open("C:/ctl_one/date.txt", ios::app);
	string dat;
	
	while (getline(fis, dat))
	{

		stringstream ss(dat);
		vector<string>v;
		while (ss.good())
		{
			string substr;
			getline(ss, substr, ',');
			if(substr!="")v.push_back(substr);
		}
		Channel aux(v[0]);
		aux.SetSub(v[1]);
		for (int i = 2; i < v.size(); i++) {
			aux.AddVideo(v[i]);
		}
		Channels.push_back(aux);


	}

	fis.close();

}
string DoFileOpen(HWND hwnd)
{
	string str="";
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "MP4";

	if (GetOpenFileName(&ofn))
	{
		HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
		return string(szFileName);
		//LoadTextFileToEdit(hEdit, szFileName);
	}
	return str;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (k == 0) {
		init();
		k++;
	}

	
	if (ver[0] == 1) {
		SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);

		AfisCanal(hwnd);
		ver[0] = 0;
	}
	if (stage == 0) {


		if (done == FALSE) {
			SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
			AfisCanal(hwnd);

			done = TRUE;
		}
	}
	else {
		if (done == FALSE) {
			SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
			AfisVideos(hwnd, Channels[CurrentChannel]);

			done = TRUE;
		}
	}
	
	switch(Message)
	{
		case WM_INITDIALOG:
		

			
			

			
				
					SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
					AfisCanal(hwnd);

		
				
		break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
			case IDC_ADD:
			{
				if (stage == 0) {
					g_hToolbar = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1),
						hwnd, ToolDlgProc);

					if (g_hToolbar != NULL)
					{
						ShowWindow(g_hToolbar, SW_SHOW);
					}
					else
					{
						MessageBox(hwnd, "CreateDialog returned NULL", "Warning!",
							MB_OK | MB_ICONINFORMATION);
					}



					break;

				}
				else {
					string str = DoFileOpen(hwnd);
					if (str != "") {


						char pBuf[256];
						size_t len = sizeof(pBuf);
						int bytes = GetModuleFileName(NULL, pBuf, len);
						string spBuf = string(pBuf);
						string stopth;
						char cstr[1024];
						strcpy(cstr, str.c_str());
						char topth[1024];
						string BufName = GetFileName(pBuf);
						string strFname = GetFileName(cstr);

						if (CurrentChannel > -1) { Channels[CurrentChannel].AddVideo(strFname); }
						stopth = spBuf.substr(0, spBuf.find(string(BufName)));


						strcpy(topth, ProjectPath);
						strcat(topth, strFname.c_str());
						CopyFile(str.c_str(), topth, 0);

						//system(cmd);
						done = FALSE;
						
						

					}
				}
			}
			
			break;
			case IDC_REMOVE:
			{
				if(stage==0){
				HWND hList = GetDlgItem(hwnd, IDC_LIST);
				int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
				if (count != LB_ERR)
				{
					if (count != 0)
					{
						
						int i;
						int* buf = (int*)GlobalAlloc(GPTR, sizeof(int) * count);
						SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)buf);


						for (i = count - 1; i >= 0; i--)
						{
							SendMessage(hList, LB_DELETESTRING, (WPARAM)buf[i], 0);
							Channels.erase(Channels.begin() + i);
						}
						save();
						
					}
					else
					{
						MessageBox(hwnd, "No items selected.", "Warning", MB_OK);
					}
				}
				else
				{
					MessageBox(hwnd, "Error counting items :(", "Warning", MB_OK);
				}
			}
		else {
					HWND hList = GetDlgItem(hwnd, IDC_LIST);
					int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
					if (count != LB_ERR)
					{
						if (count != 0)
						{
							

							int i;
							int* buf = (int*)GlobalAlloc(GPTR, sizeof(int) * count);
							SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)buf);

						

							for (i = count - 1; i >= 0; i--)
							{
								SendMessage(hList, LB_DELETESTRING, (WPARAM)buf[i], 0);
								Channels[CurrentChannel].DeleteVideoByPoz(i);
							}
							save();
							
						}
						else
						{
							MessageBox(hwnd, "No items selected.", "Warning", MB_OK);
						}
					}
					else
					{
						MessageBox(hwnd, "Error counting items :(", "Warning", MB_OK);
					}
			}

				
				
	break;
			}

				case IDC_OPEN:
				{
					if (stage == 0) {
						

						HWND hList = GetDlgItem(hwnd, IDC_LIST);
						int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
						if (count != LB_ERR)
						{
							if (count == 1)
							{
								
								if (stage == 0) {
									int* ind = (int*)GlobalAlloc(GPTR, sizeof(int));;
									Channel ch;

									SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)ind);
									SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);

									AfisVideos(hwnd, Channels[*ind]);
									stage = 1;
									CurrentChannel = *ind;
									EnableWindow(GetDlgItem(hwnd, IDC_BACK), true);
								}

							}
							else

							{
								if (count == 0) {
									MessageBox(hwnd, "No items selected.", "Warning", MB_OK);
								}
								if (count > 1) { MessageBox(hwnd, "Select only 1 item.", "Warning", MB_OK); }
							}
						}
						else
						{
							MessageBox(hwnd, "Error counting items :(", "Warning", MB_OK);
						}
					}

					/*

					/////////////////////////////
					//////////////////////////////
					//



					*/


					else {
						HWND hList = GetDlgItem(hwnd, IDC_LIST);
						int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
						if (count != LB_ERR)
						{
							if (count == 1)
							{
								
								
								int* ind = (int*)GlobalAlloc(GPTR, sizeof(int)*count);;
								Channel ch;

								SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)ind);
									Video auxv= Channels[CurrentChannel].GetVideoByPoz(*ind);
									
									
									
									char cmd[300];
									strcpy(cmd, "\"");
									string strcmd;
									//strcmd.append("\"");
									strcat(cmd, ProjectPath);
									
									strcat(cmd, auxv.GetVideoName().c_str());
									strcat(cmd, "\"");
									Channels[CurrentChannel].AddVideo(string(cmd));
									
									


									//strcmd.append(string("\""));

									//strcpy(cmd, strcmd.c_str());
									system(cmd);

									Channels[CurrentChannel].videos.erase(Channels[CurrentChannel].videos.begin() + (Channels[CurrentChannel].videos.size())-1);

								

							}
							else

							{
								if (count == 0) {
									MessageBox(hwnd, "No items selected.", "Warning", MB_OK);
								}
								if (count > 1) { MessageBox(hwnd, "Select only 1 item.", "Warning", MB_OK); }
							}
						}
						else
						{
							MessageBox(hwnd, "Error counting items :(", "Warning", MB_OK);
						}






					}}








				break;






				case IDC_DETAILS :
					if(stage ==0){
						HWND hList = GetDlgItem(hwnd, IDC_LIST);
						int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
						if (count != LB_ERR)
						{
							if (count == 1)
							{
								
								if (stage == 0) {
									int* ind = (int*)GlobalAlloc(GPTR, sizeof(int));;
									Channel ch;

									SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)ind);
								
									ch = Channels[*ind];
									string msg="Channel Name: ";
									msg = msg + ch.GetName();
									msg = msg + "\n" + "Subs: " + ch.GetSub();
									MessageBox(hwnd,msg.c_str(), "Canal", MB_OK);
									
								}

							}
							else

							{
								if (count == 0) {
									MessageBox(hwnd, "No items selected.", "Warning", MB_OK);
								}
								if (count > 1) { MessageBox(hwnd, "Select only 1 item.", "Warning", MB_OK); }
							}
						}
						else
						{
							MessageBox(hwnd, "Error", "Warning", MB_OK);
						}


					
					
					}
					else {
						HWND hList = GetDlgItem(hwnd, IDC_LIST);
						int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
						if (count != LB_ERR)
						{
							if (count == 1)
							{
								
								
									int* ind = (int*)GlobalAlloc(GPTR, sizeof(int));;
									Channel ch;

									SendMessage(hList, LB_GETSELITEMS, (WPARAM)count, (LPARAM)ind);

									ch = Channels[CurrentChannel];
									Video vid = ch.GetVideoByPoz(*ind);
									string msg = "Video Name: ";
									msg = msg + vid.GetVideoName();
									msg = msg + "\n" + "Date: " + vid.GetVideoDate();
									msg = msg + "\n" + "Location: " + string(ProjectPath)+ vid.GetVideoName();
									MessageBox(hwnd, msg.c_str(), "Video", MB_OK);

								
							}
							else

							{
								if (count == 0) {
									MessageBox(hwnd, "No items selected.", "Warning", MB_OK);
								}
								if (count > 1) { MessageBox(hwnd, "Select only 1 item.", "Warning", MB_OK); }
							}
						}
						else
						{
							MessageBox(hwnd, "Error counting items :(", "Warning", MB_OK);
						}
					}
				break;
				case IDC_BACK: {
					
					SendDlgItemMessage(hwnd, IDC_LIST, LB_RESETCONTENT, 0, 0);
					stage = 0;
					done = FALSE;
					EnableWindow(GetDlgItem(hwnd, IDC_BACK), false);

					
					
					
					break; }
				case IDC_LIST:
					switch(HIWORD(wParam))
					{
						case LBN_SELCHANGE:
						{
							

							HWND hList = GetDlgItem(hwnd, IDC_LIST);
							int count = SendMessage(hList, LB_GETSELCOUNT, 0, 0);
							if(count != LB_ERR)
							{
								

								if(count == 1)
								{

									int index;
									int err = SendMessage(hList, LB_GETSELITEMS, (WPARAM)1, (LPARAM)&index);
									if(err != LB_ERR)
									{
										

										int data = SendMessage(hList, LB_GETITEMDATA, (WPARAM)index, 0);

										SetDlgItemInt(hwnd, IDC_SHOWCOUNT, data, FALSE);
									}
									else 
									{
										MessageBox(hwnd, "Error getting selected item :(", "Warning", MB_OK);
									}
								}
								else 
								{
								
									SetDlgItemText(hwnd, IDC_SHOWCOUNT, "-");
								}
							}
							else
							{
								MessageBox(hwnd, "Error counting items :(", "Warning", MB_OK);
							}
						}
						break;
					}
				break;
			}
		break;
		case WM_CLOSE:
			save();
			EndDialog(hwnd, 0);

		break;
		default:
			
			return FALSE;
	}
	
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	
	

	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, DlgProc);
}

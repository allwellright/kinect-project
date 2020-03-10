
#include "stdafx.h"
#include "Mykinect.h"
#include "MFC_DEMO01.h"
#include "MFC_DEMO01Dlg.h"
#include"KinectJointFilter.h"
#include "afxdialogex.h"
#include<Windows.h>
#include<opencv2/opencv.hpp>	//opencvͷ�ļ�
#include <sstream>				//����ת�ַ���
#include<cstring>
//#include<afx.h>
 
using namespace std;			//�����ռ�
using namespace cv;
using namespace Sample;

//extern void CMFC_DEMO01Dlg::DrawPicToHDC(IplImage* img, UINT ID);

//ȫ�ֱ�������
static DWORD  framenumber = 0,depthnumber=0;			//����֡���
float  spinemid_xin;		//����
float  spinemid_yin;
float  spinemid_xout;
float  spinemid_yout;
float  spinemid_x;
float  spinemid_y;
float  spinebase_yin, spinebase_yout, spinebase_y;
float rightfoot_yin = 0, rightfoot_yout = 0, rightfoot_y = 0;
float rightAnkle_yin = 0, rightAnkle_yout = 0, rightAnkle_y = 0;
float leftfoot_yin = 0, leftfoot_yout = 0, leftfoot_y = 0;
float base_foot_in = 0, base_foot_out = 0, base_foot = 0;
float spinetemp = 0;
double SumHeight = 0;
const double thresh_x = 0.15;		//��ֵ
const double thresh_y = 0.2;
double rightfoot_zin, leftfoot_zin, average_zin;
double rightfoot_zout, leftfoot_zout, average_zout;
const double PI = 3.1415926;
struct vector1 {
	double x;
	double y;
	double z;
	double length;
	vector1(Joint a, Joint b)
	{
		x = a.Position.X - b.Position.X;
		y = a.Position.Y - b.Position.Y;
		z = a.Position.Z - b.Position.Z;
		length = sqrt(x * x + y * y + z * z);
	}
	double operator*(vector1 b)
	{
		return (x*b.x + y * b.y + z * b.z);
	}
};
int flag = 0;		//�¶ױ�־λ
int JumpFlag = 0;
/// Initializes the default Kinect sensor
HRESULT CBodyBasics::InitializeDefaultSensor()
{
	//�����ж�ÿ�ζ�ȡ�����ĳɹ����
	HRESULT hr;

	//����kinect
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	//�ҵ�kinect�豸
	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;//��ȡ�Ǽ�
		IDepthFrameSource* pDepthFrameSource = NULL;//��ȡ�����Ϣ
		IBodyIndexFrameSource* pBodyIndexFrameSource = NULL;//��ȡ������ֵͼ

		//��kinect
		hr = m_pKinectSensor->Open();

		//coordinatemapper
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		//bodyframe    ÿ��ͼ����ͨ��source��reader��frame������
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		//depth frame
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_DepthFrameSource(&pDepthFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameSource->OpenReader(&m_pDepthFrameReader);
		}

		//body index frame
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyIndexFrameSource(&pBodyIndexFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameSource->OpenReader(&m_pBodyIndexFrameReader);
		}

		SafeRelease(pBodyFrameSource);
		SafeRelease(pDepthFrameSource);
		SafeRelease(pBodyIndexFrameSource);
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		std::cout << "Kinect initialization failed!" << std::endl;
		return E_FAIL;
	}

	//skeletonImg,���ڻ��Ǽܡ�������ֵͼ��MAT
	skeletonImg.create(cDepthHeight, cDepthWidth, CV_8UC3);
	skeletonImg.setTo(0);

	//depthImg,���ڻ������Ϣ��MAT
	depthImg.create(cDepthHeight, cDepthWidth, CV_8UC1);
	depthImg.setTo(0);

	return hr;
}


/// Main processing function
void CBodyBasics::Update(int flag)
{
	//ÿ�������skeletonImg
	skeletonImg.setTo(0);

	//�����ʧ��kinect���򲻼�������
	if (!m_pBodyFrameReader)
	{
		return;
	}

	IBodyFrame* pBodyFrame = NULL;//�Ǽ���Ϣ
	IDepthFrame* pDepthFrame = NULL;//�����Ϣ
	IBodyIndexFrame* pBodyIndexFrame = NULL;//������ֵͼ

	//��¼ÿ�β����ĳɹ����
	HRESULT hr = S_OK;

	//---------------------------------------��ȡ������ֵͼ����ʾ---------------------------------
	if (SUCCEEDED(hr)){
		hr = m_pBodyIndexFrameReader->AcquireLatestFrame(&pBodyIndexFrame);//��ñ�����ֵͼ��Ϣ
	}
	if (SUCCEEDED(hr)){
		BYTE *bodyIndexArray = new BYTE[cDepthHeight * cDepthWidth];//������ֵͼ��8Ϊuchar�������Ǻ�ɫ��û���ǰ�ɫ
		pBodyIndexFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, bodyIndexArray);

		//�ѱ�����ֵͼ����MAT��
	uchar* skeletonData = (uchar*)skeletonImg.data;
		for (int j = 0; j < cDepthHeight * cDepthWidth; ++j){
			*skeletonData = bodyIndexArray[j]; ++skeletonData;
			*skeletonData = bodyIndexArray[j]; ++skeletonData;
			*skeletonData = bodyIndexArray[j]; ++skeletonData;
		}
		delete[] bodyIndexArray;
	}
	SafeRelease(pBodyIndexFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����

	//-----------------------��ȡ������ݲ���ʾ--------------------------
	if (SUCCEEDED(hr))
	{
		hr = m_pDepthFrameReader->AcquireLatestFrame(&pDepthFrame);//����������
	}
	if (SUCCEEDED(hr))
	{
		Mat temp(cDepthHeight, cDepthWidth, CV_16UC1);    //����ͼ�����
		pDepthFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, (UINT16 *)temp.data); //�Ȱ����ݴ���16λ��ͼ�������
		temp.convertTo(depthImg, CV_8UC1, 255.0 / 4500);   //�ٰ�16λת��Ϊ8λ

		//pDepthFrame->CopyFrameDataToArray(cDepthHeight * cDepthWidth, depthArray);

	//	//��������ݻ���MAT��
		//uchar* depthData = (uchar*)depthImg.data;
		//for (int j = 0; j < cDepthHeight * cDepthWidth; ++j)
		//{
		//	*depthData = depthArray[j];
		//	++depthData;
		//}
		//delete[] depthArray;
	}
	depthnumber++;
	SafeRelease(pDepthFrame);		//����Ҫ�ͷţ�����֮���޷�����µ�frame����
	IplImage *src;
	src = &IplImage(depthImg);
	CMFC_DEMO01Dlg *pDlg0 = CMFC_DEMO01Dlg::s_pDlg;
	pDlg0->DrawPicToHDC(src, IDC_PIC_STATIC);
	//�������
	//stringstream ostream;
	//ostream << "hello, world.";
	//GetDlgItem(IDC_OUT_EDIT1)->SetWindowText(ostream.str().c_str());
	//CString str=L"��ǰ��jdhj";
	//HWND hWnd = AfxGetMainWnd()->m_hWnd;
	//SetDlgItemText(hWnd, IDC_OUT_EDIT1, str);
	//cv::imshow("depthImg", depthImg);			
	//cv::waitKey(5);

	//-----------------------------��ȡ�Ǽܲ���ʾ----------------------------

	if (SUCCEEDED(hr))
	{
		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);//��ȡ�Ǽ���Ϣ
	}
	if (SUCCEEDED(hr))
	{
		IBody* ppBodies[BODY_COUNT] = { 0 };//ÿһ��IBody����׷��һ���ˣ��ܹ�����׷��������

		if (SUCCEEDED(hr))
		{
			//��kinect׷�ٵ����˵���Ϣ���ֱ�浽ÿһ��IBody��
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			//��ÿһ��IBody�������ҵ����ĹǼ���Ϣ�����һ�����
			ProcessBody(BODY_COUNT, ppBodies,flag);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);//�ͷ�����
		}
	}
	SafeRelease(pBodyFrame);//����Ҫ�ͷţ�����֮���޷�����µ�frame����

}


//���ֵ�״̬
void CBodyBasics::DrawHandState(const DepthSpacePoint depthSpacePosition, HandState handState)
{
	//����ͬ�����Ʒ��䲻ͬ��ɫ
	CvScalar color;
	switch (handState)
	{
	case HandState_Open:
		color = cvScalar(255, 0, 0);	//�����ſ���״̬������ɫ��ʾ
		break;
	case HandState_Closed:
		color = cvScalar(0, 255, 0);	//���Ǳպϵ�״̬������ɫ��ʾ
		break;
	case HandState_Lasso:
		color = cvScalar(0, 0, 255);	//���ǽ����ſ��ͱպϵ�״̬���ú�ɫ��ʾ
		break;
	default:							//���û��ȷ�������ƣ��Ͳ�Ҫ��
		return;
	}

	circle(skeletonImg,
		cvPoint(depthSpacePosition.X, depthSpacePosition.Y),
		20, color, -1);
}

/// Handle new body data
void CBodyBasics::ProcessBody(int nBodyCount, IBody** ppBodies,int flag)
{
	//��¼��������Ƿ�ɹ�
	HRESULT hr;
	FilterDoubleExponential filterKinect;	//������˫ָ���˲���
	//����ÿһ��IBody
	for (int i = 0; i < nBodyCount; ++i)
	{
		IBody* pBody = ppBodies[i];
		if (pBody)//��û�и���������pBody�������bTracked��ʲô����
		{
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);

			filterKinect.Update(pBody);		//ƽ�����ȶ�����Ǽ�

			if (SUCCEEDED(hr) && bTracked)
			{
				Joint joints[JointType_Count];//�洢�ؽڵ���
				HandState leftHandState = HandState_Unknown;//����״̬
				HandState rightHandState = HandState_Unknown;//����״̬

				//��ȡ������״̬
				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				//�洢�������ϵ�еĹؽڵ�λ��
				DepthSpacePoint *depthSpacePosition = new DepthSpacePoint[_countof(joints)];

				//��ùؽڵ���
				hr = pBody->GetJoints(_countof(joints), joints);
				if (SUCCEEDED(hr))
				{
					for (int j = 0; j < _countof(joints); ++j)
					{
						//���ؽڵ���������������ϵ��-1~1��ת���������ϵ��424*512��
						m_pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthSpacePosition[j]);
						circle(skeletonImg, CvPoint(depthSpacePosition[j].X, depthSpacePosition[j].Y), 3, cvScalar(0, 255, 255), 1, 8, 0);
					}
					filterKinect.Update(joints);		//ƽ������ÿ��������

					//------------------------hand state left  and  right-------------------------------
					DrawHandState(depthSpacePosition[JointType_HandLeft], leftHandState);
					DrawHandState(depthSpacePosition[JointType_HandRight], rightHandState);

					//---------------------------body-------------------------------
					DrawBone(joints, depthSpacePosition, JointType_Head, JointType_Neck);
					DrawBone(joints, depthSpacePosition, JointType_Neck, JointType_SpineShoulder);
					DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_SpineMid);
					DrawBone(joints, depthSpacePosition, JointType_SpineMid, JointType_SpineBase);
					DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_ShoulderRight);
					DrawBone(joints, depthSpacePosition, JointType_SpineShoulder, JointType_ShoulderLeft);
					DrawBone(joints, depthSpacePosition, JointType_SpineBase, JointType_HipRight);
					DrawBone(joints, depthSpacePosition, JointType_SpineBase, JointType_HipLeft);

					// -----------------------Right Arm ------------------------------------ 
					DrawBone(joints, depthSpacePosition, JointType_ShoulderRight, JointType_ElbowRight);
					DrawBone(joints, depthSpacePosition, JointType_ElbowRight, JointType_WristRight);
					DrawBone(joints, depthSpacePosition, JointType_WristRight, JointType_HandRight);
					DrawBone(joints, depthSpacePosition, JointType_HandRight, JointType_HandTipRight);
					DrawBone(joints, depthSpacePosition, JointType_WristRight, JointType_ThumbRight);

					//----------------------------------- Left Arm--------------------------
					DrawBone(joints, depthSpacePosition, JointType_ShoulderLeft, JointType_ElbowLeft);
					DrawBone(joints, depthSpacePosition, JointType_ElbowLeft, JointType_WristLeft);
					DrawBone(joints, depthSpacePosition, JointType_WristLeft, JointType_HandLeft);
					DrawBone(joints, depthSpacePosition, JointType_HandLeft, JointType_HandTipLeft);
					DrawBone(joints, depthSpacePosition, JointType_WristLeft, JointType_ThumbLeft);

					// ----------------------------------Right Leg--------------------------------
					DrawBone(joints, depthSpacePosition, JointType_HipRight, JointType_KneeRight);
					DrawBone(joints, depthSpacePosition, JointType_KneeRight, JointType_AnkleRight);
					DrawBone(joints, depthSpacePosition, JointType_AnkleRight, JointType_FootRight);

					// -----------------------------------Left Leg---------------------------------
					DrawBone(joints, depthSpacePosition, JointType_HipLeft, JointType_KneeLeft);
					DrawBone(joints, depthSpacePosition, JointType_KneeLeft, JointType_AnkleLeft);
					DrawBone(joints, depthSpacePosition, JointType_AnkleLeft, JointType_FootLeft);

					//������⺯��
					if (joints[JointType_SpineBase].Position.Z > 0.5&&joints[JointType_SpineBase].Position.Z < 3.5)
					{
						if(flag==1)
						Detection(joints);
						else if (flag == 2)
						{
							SumHeight += BodyHeight(joints);
							if (framenumber % 50 == 0)
							{
								CMFC_DEMO01Dlg *pDlg0 = CMFC_DEMO01Dlg::s_pDlg;
								SumHeight = SumHeight / 50;
								std::ostringstream stream;
								stream << SumHeight;
								string str1 = stream.str();
								str1 += "\r\n ";
								CString cstr = str1.c_str();
								pDlg0->m_outedit.SetSel(-1);
								pDlg0->m_outedit.ReplaceSel(cstr);
								
								SumHeight = 0;
							}
						}
						else if (flag == 3)
						{
							JumpDistance(joints);
						}
					}
					else
						cout << "Ϊ�˼���׼ȷ�ȣ��뾡��վ����Kinect 0.5--3.5 ��֮�䣬лл��ϣ�" << endl;
				}
				delete[] depthSpacePosition;
			}
		}
	}
	framenumber++;
	//MFC��pic�ؼ���ʾ
	IplImage *src;
	src = &IplImage(skeletonImg);
	CMFC_DEMO01Dlg *pDlg1 = CMFC_DEMO01Dlg::s_pDlg;
	pDlg1->DrawPicToHDC(src, IDC_PIC2_STATIC);
	//Opencv��ʾ
	//namedWindow("skeletonImg", 0);
	//resizeWindow("skeletonImg", 640, 480);
	//cv::imshow("skeletonImg", skeletonImg);
	//cv::waitKey(5);
}

//��2��������֮��ľ���
double CBodyBasics::Distance(Joint p1, Joint p2)
{
	double dist = 0;
	dist = sqrt(pow(p2.Position.X - p1.Position.X, 2) +
		pow(p2.Position.Y - p1.Position.Y, 2) + pow(p2.Position.Z - p1.Position.Z, 2));
	return dist;
}

//��⺯��:Ϊ�˼��׼ȷ�뾡��վ�ں���λ�ã���Kinect��ȡȫ�������
void CBodyBasics::Detection(Joint joints[])
{
	static double tin, tout;
	//double tframe;
	CMFC_DEMO01Dlg *pDlg0 = CMFC_DEMO01Dlg::s_pDlg;		//ʵ����һ��CMFC_DEMO01Dlg ָ��
	//����ÿ����10֡�ĸ߶Ȳ�Ӷ������ٶȣ�1,11,12,22
	//���30֡ÿ�룬��ô10֡����0.33�룬
	if (RaiseHands(joints) == 1)
	{
		string str1 = "˫�ָ߾�\r\n ";			
		CString cstr = str1.c_str();		
											
		pDlg0->m_outedit.SetSel(-1);
		pDlg0->m_outedit.ReplaceSel(cstr);
		cout << str1;
	}
	else if (RaiseHands(joints) == 2)
	{
		string str1 = "���ָ߾�\r\n ";
		CString cstr = str1.c_str();

		pDlg0->m_outedit.SetSel(-1);
		pDlg0->m_outedit.ReplaceSel(cstr);
		cout << str1;
	}
	else if (RaiseHands(joints) == 3)
	{
		string str1 = "���ָ߾�\r\n ";
		CString cstr = str1.c_str();

		pDlg0->m_outedit.SetSel(-1);
		pDlg0->m_outedit.ReplaceSel(cstr);
		cout << str1;
	}
	if (framenumber % 11 == 1)		//framenumber��֡���кţ��Լ������
	{
		tin = static_cast<double>(GetTickCount());
		//cout << "tin��" << tin << endl;
		spinemid_xin = joints[JointType_SpineMid].Position.X;
		spinemid_yin = joints[JointType_SpineMid].Position.Y;
		rightfoot_yin = joints[JointType_KneeRight].Position.Y;
		leftfoot_yin = joints[JointType_KneeLeft].Position.Y;
		spinebase_yin = joints[JointType_SpineBase].Position.Y;
		rightAnkle_yin = joints[JointType_AnkleRight].Position.Y;
		base_foot_in = spinebase_yin - rightAnkle_yin;
		//cout << "basefootinΪ��" << base_foot_in << endl;
		//cout << "��ǰSpineHeightin�ĸ߶�Ϊ" << SpineHeightin << "  m"<<endl;
	}
	if (!(framenumber % 11))
	{
		tout = static_cast<double>(GetTickCount());
		//cout << frmamenumber << endl;
		//cout <<"tout��"<< tout << endl;
		//cout << "ÿ10֡����һ���½����ٶ�" << endl;
		spinemid_xout = joints[JointType_SpineMid].Position.X;
		spinemid_yout = joints[JointType_SpineMid].Position.Y;
		rightfoot_yout = joints[JointType_KneeRight].Position.Y;
		leftfoot_yout = joints[JointType_KneeLeft].Position.Y;
		rightAnkle_yout = joints[JointType_AnkleRight].Position.Y;
		spinebase_yout = joints[JointType_SpineBase].Position.Y;
		base_foot_out = spinebase_yout - rightAnkle_yout;
		//cout << "��ǰ֡��Ϊ��" << base_foot_out << endl;
		//  cout << "***********************************" << endl;
		//  cout << "��ǰSpineHeightin�ĸ߶�Ϊ" << SpineHeightin << "  m" << endl;
		//tframe = (tout - tin) / getTickFrequency();
		// cout <<tframe << endl;
		//  cout << getTickFrequency()<<endl;
		//cout << "��ǰSpineHeightout�ĸ߶�Ϊ" << SpineHeightout << "  m" << endl;
		//SpineV = (SpineHeightin - SpineHeightout) / tframe;
		spinemid_x = spinemid_xout - spinemid_xin;
		spinemid_y = spinemid_yout - spinemid_yin;
		rightfoot_y = rightfoot_yout - rightfoot_yin;
		leftfoot_y = leftfoot_yout - leftfoot_yin;
		base_foot = base_foot_out - base_foot_in;
		//cout << "Spinemid_x�Ƕ��٣���" << base_foot << endl;
		//cout << "Spinemid_y�Ƕ��٣���" << spinemid_y << endl;
		//�ϱļ�⣺˫������泬��0.15�ף��������������������վ��ʱ��������0.15��
		if ((leftfoot_y>0.15&&rightfoot_y > 0.15) || (spinetemp>0.01&&spinetemp + 0.15<joints[JointType_SpineMid].Position.Y))		//y������Ϊ��
		{
			string str1 = "�ϱ�\r\n " ;			//��������Ϊ�˰����������mfc��ʾ��,����Բ��ùܣ���ͬ
			CString cstr = str1.c_str();		//ɾ���༭���е����ݣ�����һ��GetDlgItem(IDC_EDIT1)->SetWindowText("");
												//�����������༭����һ���ؼ�������m_edit1.SetWindowText("");
			pDlg0->m_outedit.SetSel(-1);
			pDlg0->m_outedit.ReplaceSel(cstr);
			cout << str1;						//����̨�����ʾ����ͬ��Ϊ�˷�����ԣ�����ͬʱ����ʾ������̨
		}	
		//else if (base_foot < -thresh_y)	//�¶׼�⣺��Ҫ����Ȳ����������ɣ��β������֮��ľ�����ٳ���0.2��
		else if (Distance(joints[JointType_HipLeft], joints[JointType_AnkleLeft])*(1 + 0.15) < Distance(joints[JointType_HipLeft], joints[JointType_KneeLeft]) + Distance(joints[JointType_KneeLeft], joints[JointType_AnkleLeft]))
		{
			flag++;
			if (flag == 2)			//�¶�״̬��Ҫʱ�䣬�������һ�±�־λ�����Ƽ�ʱ����������⵽���β����¶ף������ظ����ֽ��
			{
				flag = 0;
				//�¶�������ⷽ���������Լ��hipleft��knee��ankle����֮��ļнǹ�ϵ�;����ϵ���н�С��160�ȣ����Զ��Լ�						
				//��ֵ����˵�����¶�,��������֮�ʹ��ڵ����ߵ�1.15�����ң�Ҳ����˵�����¶�
				string str1 = "�¶�\r\n ";
				CString cstr = str1.c_str();
				pDlg0->m_outedit.SetSel(-1);
				pDlg0->m_outedit.ReplaceSel(cstr);
				cout << str1;
			}
		}
		//x�᷽������Ϊ��
		//���������ƶ�������ֵthresh_x�����ж�����
		if (spinemid_x > thresh_x)		
		{
			string str1 = "����\r\n ";
			CString cstr = str1.c_str();
			pDlg0->m_outedit.SetSel(-1);
			pDlg0->m_outedit.ReplaceSel(cstr);
			cout << str1;
		}
		else if (spinemid_x < -thresh_x)		////���������ƶ�������ֵthresh_x�����ж�����
		{
			string str1 = "����\r\n ";
			CString cstr = str1.c_str();
			pDlg0->m_outedit.SetSel(-1);
			pDlg0->m_outedit.ReplaceSel(cstr);
			cout << str1;
		}			
	}
	//���ݹ��ɶ�������HipLeft��AnkleLeft��AnkleLeft֮��ľ����ϵ��0.15��һ������ֵ���ɸ���ʵ�������΢����
/*	if (Distance(joints[JointType_HipLeft], joints[JointType_AnkleLeft])*(1 + 0.15) < Distance(joints[JointType_HipLeft], joints[JointType_KneeLeft]) + Distance(joints[JointType_KneeLeft], joints[JointType_AnkleLeft]))
	{
		flag++;
		if (flag == 2)			//�¶�״̬��Ҫʱ�䣬�������һ�±�־λ�����Ƽ�ʱ����������⵽���β����¶ף������ظ����ֽ��
		{
			flag = 0;
			cout << "�¶�1111111\n";
		}
	}
*/

}

double CBodyBasics::BodyHeight(Joint joints[])
{
	double bodyheight = 0;
	bodyheight += Distance(joints[3], joints[2]);
	bodyheight += Distance(joints[2], joints[20]);
	bodyheight += Distance(joints[20], joints[1]);
	bodyheight += Distance(joints[1], joints[0]);
	if (NumofLeft(joints) >= NumofRight(joints))
	{
		bodyheight += Distance(joints[12], joints[13]);
		bodyheight += Distance(joints[13], joints[14]);
		bodyheight += Distance(joints[14], joints[15]);
	}
	else
	{
		bodyheight += Distance(joints[16], joints[17]);
		bodyheight += Distance(joints[17], joints[18]);
		bodyheight += Distance(joints[18], joints[19]);
	}

	return bodyheight;
}
//�ѹؽڵ�֮��������������ȷ�����ð��ߣ���ȷ�����ú��ߡ�
/// Draws one bone of a body (joint to joint)
void CBodyBasics::DrawBone(const Joint* pJoints, const DepthSpacePoint* depthSpacePosition, JointType joint0, JointType joint1)
{
	TrackingState joint0State = pJoints[joint0].TrackingState;
	TrackingState joint1State = pJoints[joint1].TrackingState;

	// If we can't find either of these joints, exit
	if ((joint0State == TrackingState_NotTracked) || (joint1State == TrackingState_NotTracked))
	{
		return;
	}

	// Don't draw if both points are inferred
	if ((joint0State == TrackingState_Inferred) && (joint1State == TrackingState_Inferred))
	{
		return;
	}

	CvPoint p1 = cvPoint(depthSpacePosition[joint0].X, depthSpacePosition[joint0].Y),
		p2 = cvPoint(depthSpacePosition[joint1].X, depthSpacePosition[joint1].Y);

	// We assume all drawn bones are inferred unless BOTH joints are tracked
	if ((joint0State == TrackingState_Tracked) && (joint1State == TrackingState_Tracked))
	{
		//�ǳ�ȷ���ĹǼܣ��ð�ɫֱ��
		line(skeletonImg, p1, p2, cvScalar(255, 255, 255), 4);
	}
	else
	{
		//��ȷ���ĹǼܣ��ú�ɫֱ��
		line(skeletonImg, p1, p2, cvScalar(0, 0, 255), 4);
	}
}


//�������ͼ��
void CBodyBasics::SaveDepthImg()
{
	stringstream stream0,stream1;
	string str,filepath,str1;
	CMFC_DEMO01Dlg *pDlg0 = CMFC_DEMO01Dlg::s_pDlg;
	//filepath = "D:\\pic\\";			//���ù̶�����·��
	//��ѡ���ļ��������ȡ��ǰѡ���·��
	filepath = (pDlg0->m_edit).GetBuffer(0);
	if (filepath.empty())
		filepath = "D:\\pic\\";			//����Ĭ�Ϲ̶�����·��
	else
		filepath += "/";

	//��ѡ���ļ��������ȡ��ǰѡ���·��
	//stream0 << pDlg0->m_edit;
	//stream0 >> filepath;
	stream1 << depthnumber;        //��long����������
	stream1 >> str;					//ת��Ϊ string
	str1 = "�ɹ������ " + str + " ֡���ͼ\r\n";
	CString cstr = str1.c_str();
	
	if (depthImg.data)
	{
		imwrite(filepath + str + "depth.bmp", depthImg);
		pDlg0->m_outedit.SetSel(-1);
		pDlg0->m_outedit.ReplaceSel(cstr);
		//HWND hWnd = AfxGetMainWnd()->m_hWnd;
		//SetDlgItemText(hWnd, IDC_OUT_EDIT1,cstr);
		cout << str1 ;
		//cout << str + "depth.bmp" << endl;
	}
	else
	{
		CString cfail = "û�����ݣ�����ʧ��\r\n";
		pDlg0->m_outedit.ReplaceSel(cfail);
	}
		
}

//�������ͼ����
void CBodyBasics::SaveSkeletonImg()
{
	stringstream stream0, stream1;
	string str, filepath, str1;
	CMFC_DEMO01Dlg *pDlg0 = CMFC_DEMO01Dlg::s_pDlg;
	//filepath = "D:\\pic\\";			//���ù̶�����·��
	filepath = pDlg0->m_edit.GetBuffer(0);
	//stream0 << pDlg0->m_edit;
	//stream0 >> filepath;
	if (filepath.empty())
		filepath = "D:\\pic\\";			//����Ĭ�Ϲ̶�����·��
	else
		filepath += "/";

	//��ѡ���ļ��������ȡ��ǰѡ���·��
	//stream0 << pDlg0->m_edit;
	//stream0 >> filepath;
	stream1 << framenumber;        //��long����������
	stream1 >> str;					//ת��Ϊ string
	str1 = "�ɹ������ " + str + " ֡����ͼ\r\n";
	CString cstr = str1.c_str();

	if (skeletonImg.data)
	{
		imwrite(filepath + str + "skeleton.bmp", skeletonImg);
		pDlg0->m_outedit.SetSel(-1);
		pDlg0->m_outedit.ReplaceSel(cstr);
		//HWND hWnd = AfxGetMainWnd()->m_hWnd;
		//SetDlgItemText(hWnd, IDC_OUT_EDIT1,cstr);
		cout << str1;
		//cout << str + "depth.bmp" << endl;
	}
	else
	{
		CString cfail = "û�����ݣ�����ʧ��\r\n";
		pDlg0->m_outedit.ReplaceSel(cfail);
	}

}


//���ȸ��ٵ��Ľڵ���
int CBodyBasics::NumofLeft(Joint joints[])       
{
	int num = 0;
	for (int i = 12;i <= 15;i++)
	{
		if (joints[i].TrackingState == TrackingState_Tracked)
		{
			num++;
		}
	}
	return num;
}  

//���ȼ�⵽�Ľڵ���
int CBodyBasics::NumofRight(Joint joints[])    
{
	int num = 0;
	for (int i = 16;i <= 19;i++)
	{
		if (joints[i].TrackingState == TrackingState_Tracked)
		{
			num++;
		}
	}
	return num;
}


int CBodyBasics::RaiseHands(Joint a[])               
{
	vector1 angle89(a[8], a[9]);
	vector1 angle910(a[9], a[10]);
	vector1 angle208(a[20], a[8]);
	vector1 angle120(a[20], a[1]);
	vector1 angle65(a[6], a[5]);
	vector1 angle54(a[5], a[4]);
	vector1 angle204(a[20], a[4]);
	vector1 angle1213(a[12], a[13]);
	vector1 angle1314(a[13], a[14]);
	vector1 angle1617(a[16], a[17]);
	vector1 angle1718(a[17], a[18]);

	double Rightarm, Leftarm, Rightshoulder, Leftshoulder, Rightleg, Leftleg;
	Rightarm = angle89 * angle910 / angle89.length / angle910.length;
	Leftarm = angle65 * angle54 / angle65.length / angle54.length;
	Rightshoulder = angle208 * angle120 / angle208.length / angle120.length;
	Leftshoulder = angle120 * angle204 / angle204.length / angle120.length;
	Rightleg = angle1617 * angle1718 / angle1617.length / angle1718.length;
	Leftleg = angle1213 * angle1314 / angle1213.length / angle1314.length;


	Rightarm = 180 - acos(Rightarm) / PI * 180;
	Leftarm = 180 - acos(Leftarm) / PI * 180;
	Rightshoulder = 180 - acos(Rightshoulder) / PI * 180;
	Leftshoulder = 180 - acos(Leftshoulder) / PI * 180;
	Rightleg = 180 - acos(Rightleg) / PI * 180;
	Leftleg = 180 - acos(Leftleg) / PI * 180;

	if (Rightarm > 160 && Leftarm > 160 && Rightshoulder >= 80 && Rightshoulder <= 98 && Leftshoulder >= 80 && Leftshoulder <= 98 && a[21].Position.Y>a[3].Position.Y&&a[24].Position.Y>a[3].Position.Y)
	{
		cout << "˫�ָ߾�" << endl;
		return 1;
		/*cout << "���ֱ۵ĽǶ�Ϊ" << Rightarm << endl;
		cout << "�Ҽ��ĽǶ�Ϊ" << Rightshoulder << endl;
		cout << "���ֱ۵ĽǶ�Ϊ" << Leftarm << endl;
		cout << "����ĽǶ�Ϊ" << Leftshoulder << endl << endl;*/
	}
	else if (Rightarm > 160 && Rightshoulder >= 80 && Rightshoulder <= 98 && a[24].Position.Y>a[3].Position.Y)
	{
		cout << "���ָ߾�" << endl;
		return 2;
		/*cout << "���ֱ۵ĽǶ�Ϊ" << Rightarm << endl;
		cout << "�Ҽ��ĽǶ�Ϊ" << Rightshoulder << endl;
		cout << "���ֱ۵ĽǶ�Ϊ" << Leftarm << endl;
		cout << "����ĽǶ�Ϊ" << Leftshoulder << endl << endl;*/
	}
	else if (Leftarm > 160 && Leftshoulder >= 80 && Leftshoulder <= 98 && a[21].Position.Y>a[3].Position.Y)
	{
		cout << "���ָ߾�" << endl;
		return 3;
		/*cout << "���ֱ۵ĽǶ�Ϊ" << Rightarm << endl;
		cout << "�Ҽ��ĽǶ�Ϊ" << Rightshoulder << endl;
		cout << "���ֱ۵ĽǶ�Ϊ" << Leftarm << endl;
		cout << "����ĽǶ�Ϊ" << Leftshoulder << endl << endl;*/
	}
	else
	{
		return 0;
	}
}

void  CBodyBasics::JumpDistance(Joint joints[])
{
	static double tin, tout;
	//double tframe;
	double thresh_x = 0.15;
	
	framenumber++;

	//if (framenumber % 11 == 1)		//framenumber��֡���кţ��Լ������
	if (RaiseHands(joints) == 2 && JumpFlag == 0)
	{
		JumpFlag++;

		tin = static_cast<double>(GetTickCount());
		//cout << "tin��" << tin << endl;

		rightfoot_zin = joints[JointType_FootLeft].Position.Z;
		leftfoot_zin = joints[JointType_FootRight].Position.Z;
		average_zin = (rightfoot_zin + leftfoot_zin) / 2;
		cout << "YES" << endl;
		Sleep(5000);
		return;
		//cout << "basefootinΪ��" << base_foot_in << endl;
		//cout << "��ǰSpineHeightin�ĸ߶�Ϊ" << SpineHeightin << "  m"<<endl;

	}
	else if (RaiseHands(joints) == 2 && JumpFlag == 1)
	{
		tout = static_cast<double>(GetTickCount());
		rightfoot_zout = joints[JointType_FootLeft].Position.Z;
		leftfoot_zout = joints[JointType_FootRight].Position.Z;
		average_zout = (rightfoot_zout + leftfoot_zout) / 2;
		//cout << "Spinemid_x�Ƕ��٣���" << base_foot << endl;
		//cout << "Spinemid_y�Ƕ��٣���" << spinemid_y << endl;
		//�ϱļ�⣺˫������泬��0.15�ף��������������������վ��ʱ��������0.15��
		///if ((leftfoot_y>0.15&&rightfoot_y > 0.15) || (spinetemp>0.01&&spinetemp + 0.15<joints[JointType_SpineMid].Position.Y))		//y������Ϊ��
		

		if (fabs(average_zout - average_zin) > 0.2)
		{
			double res = fabs(average_zout - average_zin);
			CMFC_DEMO01Dlg *pDlg0 = CMFC_DEMO01Dlg::s_pDlg;
			std::ostringstream stream;
			stream << res;
			string str1 = stream.str();
			str1 += "\r\n ";
			CString cstr = str1.c_str();
			pDlg0->m_outedit.SetSel(-1);
			pDlg0->m_outedit.ReplaceSel(cstr);
			//cout << fabs(average_zout - average_zin) << endl;
		}
		JumpFlag = 0;
	}
}


/**************************************************************************************************/
//���캯��
CBodyBasics::CBodyBasics() :
m_pKinectSensor(NULL),
m_pCoordinateMapper(NULL),
m_pBodyFrameReader(NULL),
m_pDepthFrameReader(NULL),
m_pBodyIndexFrameReader(NULL)
{
	//pDlg = new CMFC_DEMO01Dlg();
}

// ��������
CBodyBasics::~CBodyBasics()
{
	SafeRelease(m_pBodyFrameReader);
	SafeRelease(m_pCoordinateMapper);
	SafeRelease(m_pDepthFrameReader);
	SafeRelease(m_pBodyIndexFrameReader);

	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}
	SafeRelease(m_pKinectSensor);
}




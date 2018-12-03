/*---------------------------------------------------------*/
/*
   	Weixu ZHU (Harry)
   		zhuweixu_harry@126.com
	
	Version 1.0
	Version 1.1 : change opengl debug layout

*/
/*---------------------------------------------------------*/

#include <stdio.h>
#include "GLTools.h"

#include "Vector3.h"
#include "Quaternion.h"

#define PI 3.1415926

#include "tcp_socket.h"

/*------------ Function Control Channels ------------*/
double CH1 = 0, CH1_MAX = 60, CH1_MIN = -60, CH1_STEP = 0.3;
double CH2 = 0, CH2_MAX = 60, CH2_MIN = -60, CH2_STEP = 0.3;
int CMDCH1 = 0;
int CMDCH2 = 0;

/* --------------- TCP declare --------------------*/
int tcpOpen();
int tcpSendSpeed(int16_t L, int16_t R);
int tcpSendPara(float kp, float ki, float kd);
int tcpGetSpeed(int16_t *L, int16_t *R);
int tcpGetSpeedWithDebug(int16_t *L, int16_t *R, int16_t *Lerror, float *Loutput);

/* --------------- variables --------------------*/
int16_t tL, tR;
int16_t mL, mR;
float Kp, Ki, Kd;

/* --------------- MainLoop functions --------------------*/
int function_exit()
{
	CH1 = 0;
	CH2 = 0;
	function_step(0.1);
	return 0;
}

int function_init()
{
	tL = 0;
	tR = 0;
	Kp = 0;
	Ki = 0;
	Kd = 0;

	tcpOpen();
	
	dataCount = 0;

	return 0;
}

int function_step(double time)	// time in s
{
	tL = (int)CH1;
	tR = (int)CH2;

	int ret;
	if (CMDCH1 == 1)
	{
		scanf("%d %d", &tL, &tR);
		tcpSendSpeed(tL, tR);
		CH1 = tL; CH2 = tR;
		CMDCH1 = 0;
	}
	else if (CMDCH2 == 1)
	{
		scanf("%f %f %f", &Kp, &Ki, &Kd);
		tcpSendPara(Kp, Ki, Kd);
				#ifdef datadebug
				printf("just sent tcp PID %f %f %f\n",Kp,Ki,Kd);
				#endif
		CMDCH2 = 0;
	}
	else
	{
		tcpSendSpeed(tL, tR);
				#ifdef datadebug
				printf("just sent speed\n");
				#endif
	}

	int16_t Lerror;
	float oL;
	//tcpGetSpeedWithDebug(&mL, &mR, &Lerror, &oL);
	tcpGetSpeed(&mL, &mR);
				#ifdef datadebug
				printf("just get speed\n");
				#endif
				printf("tL = %d, mL = %d, Lerror = %d, Loutput = %f\n",tL, mL, Lerror, oL);

	dataCount++; if (dataCount >= MAXLOG) dataCount = 0;
	dataLog[0][dataCount] = tL;
	dataLog[1][dataCount] = mL;
	dataLog[2][dataCount] = oL;

	dataLog[3][dataCount] = tR;
	dataLog[4][dataCount] = mR;
		
	return 0;
}

/* --------------- OpenGL draw functions --------------------*/
int function_draw()
{
	drawPlot(dataCount - 100, dataCount, 0, -1, 0);	// tL
	drawPlot(dataCount - 100, dataCount, 1,	-1,  0);// mL
	drawPlot(dataCount - 100, dataCount, 2,	-1, 0);	// oL
	drawPlot(dataCount - 100, dataCount, 3,	0,  0);	// tR
	drawPlot(dataCount - 100, dataCount, 4,	0,  0);	// mR

	return 0;
}

int function_draw2()
{
	return 0;
}

/* --------------- TCP --------------------*/
TCPSocket *tcpSocket;
int tcpOpen()
{
	tcpSocket = new TCPSocket("192.168.1.201",8080);
	if (tcpSocket->Open() != 0) {printf("can't open socket\n"); return -1;}
}

int tcpSendSpeed(int16_t L, int16_t R)
{
	uint8_t data[] = {
		reinterpret_cast<uint8_t*>(&tL)[1],
		reinterpret_cast<uint8_t*>(&tL)[0],
		reinterpret_cast<uint8_t*>(&tR)[1],
		reinterpret_cast<uint8_t*>(&tR)[0],
	};
	tcpSocket->Write( (char*)data , 4);
}

int tcpSendPara(float kp, float ki, float kd)
{
	uint8_t data[] = {
		reinterpret_cast<uint8_t*>(&kp)[3],
		reinterpret_cast<uint8_t*>(&kp)[2],
		reinterpret_cast<uint8_t*>(&kp)[1],
		reinterpret_cast<uint8_t*>(&kp)[0],

		reinterpret_cast<uint8_t*>(&ki)[3],
		reinterpret_cast<uint8_t*>(&ki)[2],
		reinterpret_cast<uint8_t*>(&ki)[1],
		reinterpret_cast<uint8_t*>(&ki)[0],

		reinterpret_cast<uint8_t*>(&kd)[3],
		reinterpret_cast<uint8_t*>(&kd)[2],
		reinterpret_cast<uint8_t*>(&kd)[1],
		reinterpret_cast<uint8_t*>(&kd)[0],
	};

	tcpSocket->Write( (char*)data , 12);
}

int tcpGetSpeed(int16_t *L, int16_t *R)
{
	int16_t data1, data2;
	char rdata[200]; int len;
	tcpSocket->Read(rdata, &len, 300);
	if (len != 4) return -1;

	reinterpret_cast<int16_t&>(data1) = rdata[0]<<8 | rdata[1];
	reinterpret_cast<int16_t&>(data2) = rdata[2]<<8 | rdata[3];
	*L = data1; *R = data2;
	return 0;
}

int tcpGetSpeedWithDebug(int16_t *L, int16_t *R, int16_t *Lerror, float *Loutput)
{
	int16_t data1, data2, data3;
	float data4;

	char rdata[200]; int len;
	tcpSocket->Read(rdata, &len, 300);
	if (len != 10) return -1;

	reinterpret_cast<int16_t&>(data1) = rdata[0]<<8 | rdata[1];
	reinterpret_cast<int16_t&>(data2) = rdata[2]<<8 | rdata[3];
	reinterpret_cast<int16_t&>(data3) = rdata[4]<<8 | rdata[5];

	uint32_t nData1, nData2, nData3, nData4, nData32;
	nData1 = 0xFF & rdata[6];
	nData2 = 0xFF & rdata[7];
	nData3 = 0xFF & rdata[8];
	nData4 = 0xFF & rdata[9];
	nData32 = nData1<<24 | nData2<<16 | nData3<<8 | nData4;
				#ifdef datadebug
				printf("rdata %x %x %x %x %x\n",rdata[6],rdata[7],rdata[8],rdata[9],nData32);
				printf("ndata %x %x %x %x %x\n",nData1,  nData2,  nData3,  nData4,  nData32);
				#endif
	data4 = *(reinterpret_cast<float*>(&nData32));
				#ifdef datadebug
				printf("data4 = %f\n",data4);
				#endif

	*L = data1; *R = data2; *Lerror = data3; *Loutput = data4;
	return 0;
}

/* --------------- draw obj --------------------*/
#ifndef BOX
#include "Box.h"
void Box::draw()
{
	Vector3 axis = this->q.getAxis();
	double ang = this->q.getAng();

	glTranslatef(this->l.x, this->l.y, this->l.z);
	glRotatef(ang*180/PI,axis.x,axis.y,axis.z);
	if ((this->x != 0) && (this->y != 0) && (this->z != 0))
	{
		glScalef(this->x, this->y, this->z);
		glutSolidCube(1);	
		glScalef(1/this->x, 1/this->y, 1/this->z);
	}
	glRotatef(-ang*180/PI,axis.x,axis.y,axis.z);
	glTranslatef(-this->l.x, -this->l.y, -this->l.z);
}
#endif

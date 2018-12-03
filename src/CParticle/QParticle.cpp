/*----------------------------------------*/
/*	Weixu ZHU (Harry)
		zhuweixu_harry@126.com
	Version 2.0
		combine Particle and QParticle
	Version 2.1
		use vector3 to denote rotation, Quaternion not valid
*/
/*----------------------------------------*/

#include "QParticle.h"

/*--- Constructor -----------------------------*/
QParticle::QParticle()
{
	Vector3 v(0,0,0);
	setl(v);
	setv(v);
	seta(v);
	setq(Quaternion(0,0,1,0));
	setvq(0,0,1,0);
	setaq(0,0,1,0);
}

QParticle::QParticle(double x,double y,double z)
{
	Vector3 v(0,0,0);
	setl(Vector3(x,y,z));
	setv(v);
	seta(v);
	setq(Quaternion(0,0,1,0));
	setvq(0,0,1,0);
	setaq(0,0,1,0);
}

QParticle::QParticle(const Vector3& _x)
{
	Vector3 v(0,0,0);
	setl(_x);
	setv(v);
	seta(v);
	setq(Quaternion(0,0,1,0));
	setvq(0,0,1,0);
	setaq(0,0,1,0);
}

QParticle::QParticle(double x,double y,double z, 
					double qx,double qy,double qz,double qw)
{
	Vector3 v(0,0,0);
	setl(Vector3(x,y,z));
	setv(v);
	seta(v);
	setq(Quaternion(qx,qy,qz,qw));
	setvq(0,0,1,0);
	setaq(0,0,1,0);
}

QParticle::QParticle(const Vector3& _x, const Quaternion& _y)
{
	Vector3 v(0,0,0);
	setl(_x);
	setv(v);
	seta(v);
	Quaternion q(0,0,1,0);
	setq(_y);
	setvq(0,0,1,0);
	setaq(0,0,1,0);
}

QParticle::~QParticle()
{
}

/*--- set -----------------------------*/
QParticle& QParticle::setl(const Vector3& _x)
	{ l = _x; return *this;}
QParticle& QParticle::setl(double x,double y,double z)
	{ return setl(Vector3(x,y,z));}
QParticle& QParticle::setv(const Vector3& _x)
	{ v = _x; return *this;}
QParticle& QParticle::setv(double x,double y,double z)
	{ return setv(Vector3(x,y,z));}
QParticle& QParticle::seta(const Vector3& _x)
	{ a = _x; return *this;}
QParticle& QParticle::seta(double x,double y,double z)
	{ return seta(Vector3(x,y,z));}

QParticle& QParticle::setq(const Quaternion& _x)
	{ q = _x; return *this;}
QParticle& QParticle::setq(double qx,double qy,double qz,double qw)
	{ return setq(Quaternion(qx,qy,qz,qw));}
//QParticle& QParticle::setvq(const Quaternion& _x)
//	{ vq = _x; return *this;}
QParticle& QParticle::setvq(double qx,double qy,double qz,double qw)
{
	vq = Vector3(qx, qy, qz).nor() * qw;
	return *this;
}
//QParticle& QParticle::setaq(const Quaternion& _x)
//	{ aq = _x; return *this;}
QParticle& QParticle::setaq(double qx,double qy,double qz,double qw)
{
	aq = Vector3(qx, qy, qz).nor() * qw;
	return *this;
}

/*--- getFront and Up -----------------------------*/
Vector3 QParticle::getFront() const
{
	Vector3 a(1,0,0);
	a = q.toRotate(a);
	return a;
}
Vector3 QParticle::getUp() const
{
	Vector3 a(0,0,1);
	a = q.toRotate(a);
	return a;
}

/*--- run -----------------------------*/
int QParticle::run(double time)
{
	l += v * time;
	v += a * time;

	q *= Quaternion(vq, vq.len() * time);
	vq += aq * time;

	return 0;
}

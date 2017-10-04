/*
#ifndef MOTOR_INCLUDED
#define MOTOR_INCLUDED

#include "mbed.h"

//gpio2
#define MOTOR1 (0x1 << 6)
#define MOTOR2 (0x1 << 7)
#define MOTOR3 (0x1 << 8)

class Motor
{
public:
    Motor();
    void drive(int ch);
    void off();
    int duty1;
    int duty2;
    int duty3;
    int dir1;
    int dir2;
    int dir3;
};
#endif
*/
#ifndef MOTOR_INCLUDED
#define MOTOR_INCLUDED

#include "mbed.h"

//gpio2
#define MOTOR1 (0x1 << 6)
#define MOTOR2 (0x1 << 7)
#define MOTOR3 (0x1 << 8)

//pwmの差の設定
#define Diff 100

//ローパス   A:前の値　B:今の値   M:合計
#define M 40
#define A (37)
#define B (M - A)

//逆ローパス C:前の値  D:今の値  E:合計
#define E 40
#define C 37
#define D (E - C)

class Motor
{
public:
    Motor();
    void init();
    void drive(int ch);
    void drive_L(int ch);
    void off();
    int duty1;
    int duty2;
    int duty3;
    int dir1;
    int dir2;
    int dir3;

private:
    int odir1;
    int odir2;
    int odir3;
    int npwm1;
    int npwm2;
    int npwm3;
    int opwm1;
    int opwm2;
    int opwm3;
    int pwm_sa;
    int oduty1;
    int oduty2;
    int oduty3;
};

#endif

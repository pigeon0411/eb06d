
#ifndef  __EXTRN_FUNC_H
#define	 __EXTRN_FUNC_H
enum IRIS_CTL_MODE_TYPE
{
IRIS_CAM_IRIS,
IRIS_JIGUANG_ZOOM,//�������Ƕ�
IRIS_JIGUANG_CURRENT,//�������
IRIS_CAM_OSD_ENTER
};

extern enum IRIS_CTL_MODE_TYPE iris_ctl_mode;


extern void dome_func_control(uchar action,uchar prePoint);
extern void cam_filt_cutter_set(u8 mode);
extern u8 RS485_SendBytes(u16 bytes_size,u8 *send_buff);

void extern_io_output(u8 mode);
void extern_io2_output(u8 mode);
void motor_lens_voltage_set(u8 mode);
void save_system_para(u8 mode);
void load_system_para(void);
void extern_io3_output(u8 mode);


#endif

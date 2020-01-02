#pragma once
#include "ComDef.h"
#include "Gloable_Cross.h"
using namespace Gloable_Cross;
class CCrossFunManager
{
public:
	CCrossFunManager();
	~CCrossFunManager();

	/*************************************ԭTestDllDemo�����ӿ⺯������*********************************/
	/*��������������һ��������
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          ControllerParameter ��Ҫ���õĲ���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_AddControllerParameter(const char *IPAddr, const struct ControllerParameter *pParameter, char **errmsg);

	/*��������������һ�������������в���
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          ControllerParameter ��Ҫ���õĲ���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_SetControllerParameter(const char *IPAddr, const struct ControllerParameter *pParameter, char **errmsg);

	/*�������������ÿ�������ʱ��
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          pTime ��Ҫ���õ�ʱ��
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_SetControllerTime(const char *IPAddr, const struct tm *pTime, char **errmsg);

	/*��������������DDC,�൱�������λ
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_RebootController(const char *IPAddr, char **errmsg);

	/*�������������ÿ��������ָ��������ã�������
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_ResetControler(const char *IPAddr, char **errmsg);


	/*��������������Զ�����������س���
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int  BES200_FunIAP(const char *IPAddr, char **errmsg);
	/*������������ȡ��������ʱ��
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          pTime ��ȡ����ʱ��ʹ���ڴ˽ṹ����
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_GetControllerTime(const char *IPAddr, struct tm *pTime, char **errmsg);

	/*������������ȡ���������������ò���
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          pParameter ��ȡ���Ŀ�������������Ϣ������ڴ˽ṹ����
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_GetControllerParameter(const char *IPAddr, struct ControllerParameter *pParameter, char **errmsg);

	/*�����������㲥��ʽ������������ͨ�ţ���ȡ���п���������Ϣ
	*����������pInfo ��ȡ���Ŀ�������������Ϣ������ڴ˽ṹ��������
	*          size pInfo�ṹ������ĳ���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��info��������BES200_Free(errmsg)��BES200_Free(info)
	*/
	int BES200_GetControllerInfoByBroadcast(struct ControllerInfo **pInfo, int *size, char **errmsg);


	/*�����������㲥��ʽ������������ͨ�ţ�����MAC��ַ����������������Ϣ����Ҫ��ͨ��MAC�޸�IP��������Ϣ
	*����������pInfo ����Ҫ���õĿ���������Ϣ���ڴ˽ṹ���У�����ݽṹ���е�MAC��ַ�����ö�Ӧ��DDC����������������
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��info��������BES200_Free(errmsg)��BES200_Free(info)
	*/
	int BES200_SetControllerInfoByBroadcast(const struct ControllerInfo *pInfo, char **errmsg);

	/*����������ɾ��һ������������ɾ���͸ÿ�������ص�ģ�顢��
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_DeleteController(const char *IPAddr, char **errmsg);

	/*�����������ڿ�����������һ��ģ�鲢���ú�ģ�����Ӧ����
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          ModuleID ģ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          pParameter ģ����������ò���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_AddModule(const char *IPAddr, uint16_t ModuleID, const struct ModuleParameter *pParameter, char **errmsg);

	/*��������������ģ����������ò���
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          ModuleID ģ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          pParameter ģ����������ò���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_SetModuleParameter(const char *IPAddr, uint16_t ModuleID, const struct ModuleParameter *pParameter, char **errmsg);

	/*����������ɾ��һ��ģ�飬��ɾ����ģ����صĵ�
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          ModuleID ģ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          pParameter ģ����������ò���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_DeleteModule(const char *IPAddr, uint16_t ModuleID, char **errmsg);

	/*������������ȡģ�������������Ϣ
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          ModuleID ģ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          pParameter ģ����������ò���������ڴ˽ṹ����
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_GetModuleParameter(const char *IPAddr, uint16_t ModuleID, struct ModuleParameter *pParameter, char **errmsg);

	/*������������ȡģ��Ĵ������
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          ModuleID ģ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          ErrorCode ģ��Ĵ�����룬���嶨���***
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_GetModuleErrorCode(const char *IPAddr, uint16_t ModuleID, uint8_t *ErrorCode, char **errmsg);

	/*�������������һ���߼���
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          LogicalPointID ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          LogicalPointParameter ��Ҫ���õĲ���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_AddLogicalPoint(const char *IPAddr, uint16_t LogicalPointID, const struct LogicalPointParameter *pParameter, char **errmsg);

	/*��������������һ���߼�������в���
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	LogicalPointID ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          LogicalPointParameter ��Ҫ���õĲ���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_SetLogicalPointParameter(const char *IPAddr, uint16_t LogicalPointID, const struct LogicalPointParameter *pParameter, char **errmsg);

	/*����������ɾ��һ���߼�������в���
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          LogicalPointID ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          LogicalPointParameter ��Ҫɾ����Ĳ���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_DeleteLogicalPoint(const char *IPAddr, uint16_t LogicalPointID, char **errmsg);

	/*������������ȡһ���߼�������в���
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          LogicalPointID ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          LogicalPointParameter ��Ҫ��ȡ�Ĳ���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_GetLogicalPointParameter(const char *IPAddr, uint16_t LogicalPointID, struct LogicalPointParameter *pParameter, char **errmsg);

	/*������������ȡһ���߼���ı�����Ϣ��������ĵ�ǰֵ����ı���״̬������״α�������ֵ�͵�ķ���������󱨾�ֵ
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          LogicalPointID ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          LogicalPointAlarmInfo Ҫ��ȡ�ı�����Ϣ
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)*/
	int BES200_GetLogicalPointAlarmInfor(const char *IPAddr, uint16_t LogicalPointID, struct LogicalPointAlarmInfor *pParameter, char **errmsg);

	/*�������������һ������
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          SceneID ������IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          SceneParameter ��Ҫ���õĳ��������в���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_AddScene(const char *IPAddr, uint16_t SceneID, const struct SceneParameter *pParameter, char **errmsg);

	/*�����������޸�һ������
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          SceneID ������IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          SceneParameter ��Ҫ�޸ĵĳ��������в���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_SetSceneParameter(const char *IPAddr, uint16_t SceneID, const struct SceneParameter *pParameter, char **errmsg);

	/*����������ɾ��һ������
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          SceneID ������IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          SceneType ��Ҫɾ���ĳ���������
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_DeleteScene(const char *IPAddr, uint16_t SceneID, char **errmsg);

	/*������������ȡһ��������������Ϣ
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          SceneID ������IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          SceneType ��Ҫ��ȡ�ĳ���������
	*          SceneParameter ��Ҫ��ȡ�ĳ��������в���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_GetSceneParameter(const char *IPAddr, uint16_t SceneID, struct SceneParameter *pParameter, char **errmsg);

	/*�������������һ���ƻ�
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          SchedulerID �ƻ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          SchedulerParameter ��Ҫ���õļƻ������в���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_AddScheduler(const char *IPAddr, uint16_t SchedulerID, const struct SchedulerParameter *pParameter, char **errmsg);

	/*��������������һ���ƻ������в���
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          SchedulerID �ƻ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          SchedulerParameter ��Ҫ���õļƻ������в���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_SetSchedulerParameter(const char *IPAddr, uint16_t SchedulerID, const struct SchedulerParameter *pParameter, char **errmsg);

	/*����������ɾ��һ���ƻ�
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          SchedulerID �ƻ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_DeleteScheduler(const char *IPAddr, uint16_t SchedulerID, char **errmsg);

	/*������������ȡһ���ƻ������в���
	*����������IPAddr ��������IP��ַ�������ַһ����ͨ���������ӵķ�ʽ�����ϵ�
	*          SchedulerID �ƻ���IDʶ���룬����Ψһ����ֻ�����ʱ����ָ�����Ժ������޸�
	*          SchedulerParameter ��Ҫ���õļƻ������в���
	*          errmsg ����������ó�����洢�Ŵ������ϸ��Ϣ�������������֮������ͷŴ˿ռ�
	*����ֵ�������Ĵ������ BES200_ERROR_OK  BES200_ERROR_CONNECT  BES200_ERROR_SEND BES200_ERROR_RECIVE BES200_ERROR_REFUSED
	*ע�⣺���ô˺���֮��һ��Ҫ�ͷ�errmsg��������BES200_Free(errmsg)
	*/
	int BES200_GetSchedulerParameter(const char *IPAddr, uint16_t SchedulerID, struct SchedulerParameter *pParameter, char **errmsg);


	void BES200_Free(char **errmsg);


	int BES200_SetLogicalPointValue(const char *IPAddr, uint16_t LogicalPointID, const struct LogicalPointvalue *pParameter, char **errmsg);

	int BES200_SetLogicalPointValueByName(const char *IPAddr, const char * LogicalPointName, float Value, char **errmsg);


	uint16_t BES200_htons(uint16_t n);
	  

	uint16_t BES200_ntohs(uint16_t n);
	
};

